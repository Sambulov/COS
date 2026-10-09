#!/usr/bin/env python3
import argparse
import json
import locale
import os
import shutil
import socket
import struct
import subprocess
import sys
import tempfile
import time
import logging
import threading

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class OpenOCDManager:
    def __init__(self, log_file=None, allow_exec=False):
        self.process = None
        self.temp_dir = None
        self.running = False
        self.log_file = log_file
        self.allow_exec = allow_exec   # команда exec разрешена только ключом --allow-exec
        self.stdout_thread = None
        self.stderr_thread = None
        self.stop_logging = threading.Event()
        self.lock = threading.Lock()   # start/stop могут прийти от разных клиентов сразу

    def _log_output(self, stream, stream_name):
        """Читает поток и выводит в лог/консоль."""
        for line in iter(stream.readline, ''):
            if self.stop_logging.is_set():
                break
            if line.strip():
                # Выводим в консоль сервера
                logger.info(f"[OpenOCD-{stream_name}] {line.rstrip()}")
                # Если указан файл, пишем туда
                if self.log_file:
                    with open(self.log_file, 'a', encoding='utf-8') as f:
                        f.write(f"[{stream_name}] {line}")
        stream.close()

    def start_openocd(self, files):
        if self.running and self.process and self.process.poll() is None:
            return False, "OpenOCD already running"
        if self.running:
            self._cleanup()

        try:
            self.temp_dir = tempfile.mkdtemp(prefix="openocd_")
            file_paths = []
            for file_info in files:
                name = file_info['name']
                content = file_info['content']
                safe_name = os.path.basename(name)
                file_path = os.path.join(self.temp_dir, safe_name)
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                file_paths.append(file_path)
                logger.info(f"Saved config: {file_path}")

            cmd = ['openocd']
            cmd.extend(['-c', 'bindto 0.0.0.0'])
            for cfg in file_paths:
                cmd.extend(['-f', cfg])
            logger.info(f"Starting OpenOCD: {' '.join(cmd)}")
            
            # Запускаем процесс с PIPE для захвата вывода
            self.process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1  # построчный буфер
            )
            
            # Сбрасываем флаг остановки логов
            self.stop_logging.clear()
            
            # Запускаем потоки для чтения stdout и stderr
            self.stdout_thread = threading.Thread(
                target=self._log_output,
                args=(self.process.stdout, 'stdout'),
                daemon=True
            )
            self.stderr_thread = threading.Thread(
                target=self._log_output,
                args=(self.process.stderr, 'stderr'),
                daemon=True
            )
            self.stdout_thread.start()
            self.stderr_thread.start()
            
            # Даём немного времени на запуск
            time.sleep(1.0)
            retcode = self.process.poll()
            if retcode is not None:
                # Процесс завершился - ошибка
                self.stop_logging.set()
                self.stdout_thread.join(timeout=1)
                self.stderr_thread.join(timeout=1)
                # stderr уже выведен через _log_output, но можно добавить финальную ошибку
                self._cleanup()
                return False, "OpenOCD exited immediately (see logs above)"
            
            self.running = True
            return True, "OpenOCD started successfully"
        except FileNotFoundError:
            self._cleanup()
            return False, "OpenOCD executable not found in PATH"
        except Exception as e:
            logger.error(f"Failed to start OpenOCD: {e}")
            self._cleanup()
            return False, str(e)

    def stop_openocd(self):
        if not self.running or self.process is None:
            return False, "No OpenOCD running"
        try:
            if self.process.poll() is None:
                logger.info("Terminating OpenOCD")
                self.process.terminate()
                try:
                    self.process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    logger.warning("Force killing OpenOCD")
                    self.process.kill()
                    self.process.wait()
            
            # Останавливаем потоки чтения логов
            self.stop_logging.set()
            if self.stdout_thread and self.stdout_thread.is_alive():
                self.stdout_thread.join(timeout=2)
            if self.stderr_thread and self.stderr_thread.is_alive():
                self.stderr_thread.join(timeout=2)
                
            self._cleanup()
            return True, "OpenOCD stopped"
        except Exception as e:
            logger.error(f"Stop error: {e}")
            self._cleanup()
            return False, str(e)

    def _cleanup(self):
        if self.temp_dir and os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir, ignore_errors=True)
        if self.process:
            if self.process.stdout:
                self.process.stdout.close()
            if self.process.stderr:
                self.process.stderr.close()
        self.process = None
        self.temp_dir = None
        self.running = False
        self.stdout_thread = None
        self.stderr_thread = None

def recv_exact(sock, num_bytes):
    data = b''
    while len(data) < num_bytes:
        chunk = sock.recv(num_bytes - len(data))
        if not chunk:
            raise ConnectionError("Socket closed")
        data += chunk
    return data

# Размер порции записи в сокет. Одиночный send на несколько килобайт уходит одним
# сегментом TCP и молча пропадает там, где MTU пути меньше (туннель с MTU 1328 и
# без ICMP «нужна фрагментация»), поэтому пишем порциями и не даём ядру склеить их.
CHUNK = 1024
REQUEST_TIMEOUT = 30.0   # с: сколько ждём запрос от клиента


def send_all(sock, data, chunk=CHUNK):
    view = memoryview(data)
    for i in range(0, len(view), chunk):
        sock.sendall(view[i:i + chunk])
        if (i + chunk) < len(view):
            time.sleep(0.001)


def send_message(sock, msg_bytes, chunk=CHUNK):
    send_all(sock, struct.pack('>I', len(msg_bytes)), chunk)
    send_all(sock, msg_bytes, chunk)

def recv_message(sock):
    raw_len = recv_exact(sock, 4)
    msg_len = struct.unpack('>I', raw_len)[0]
    if msg_len > 10 * 1024 * 1024:
        raise ValueError("Message too large")
    return recv_exact(sock, msg_len)

# --- выполнение команд оболочки на этой машине (команда exec) ---
EXEC_MAX_OUTPUT = 256 * 1024   # байт: больше не отдаём клиенту, лишнее обрезаем
EXEC_MAX_TIMEOUT = 3600.0      # с: предел ожидания одной команды


def console_encoding():
    """Кодировка консоли этой машины: в ней cmd.exe отдаёт вывод."""
    try:
        import ctypes
        return 'cp%d' % ctypes.windll.kernel32.GetOEMCP()
    except Exception:
        return None


def decode_output(data):
    """Вывод команды приходит в консольной кодировке машины — пробуем по очереди."""
    for enc in ('utf-8', console_encoding(), locale.getpreferredencoding(False), 'cp866'):
        if not enc:
            continue
        try:
            return data.decode(enc)
        except (UnicodeDecodeError, LookupError):
            continue
    return data.decode('utf-8', errors='replace')


def kill_tree(proc):
    """Снять процесс вместе с потомками: shell=True оставляет живых детей."""
    if (proc.poll() is None) and (os.name == 'nt'):
        try:
            res = subprocess.run(['taskkill', '/F', '/T', '/PID', str(proc.pid)],
                                 stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                                 stderr=subprocess.DEVNULL, timeout=10)
            if res.returncode != 0:
                logger.warning(f"taskkill returned {res.returncode}: the process tree may survive")
        except Exception as e:
            logger.warning(f"taskkill failed: {e}")
    if proc.poll() is None:
        try:
            proc.kill()
        except Exception:
            pass


def run_shell(cmd, timeout, cwd=None):
    """Выполнить команду оболочки и вернуть её код возврата и вывод."""
    started = time.time()
    proc = subprocess.Popen(cmd, shell=True, cwd=cwd, stdin=subprocess.DEVNULL,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    timed_out = False
    try:
        out, err = proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        timed_out = True
        kill_tree(proc)                # по времени снимаем всё дерево процессов
        try:
            out, err = proc.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            # потомок держит пайп открытым: ждать его нельзя, отдаём что успели
            proc.kill()
            out, err = b'', b''
    cut = False
    if len(out) > EXEC_MAX_OUTPUT:
        out, cut = out[:EXEC_MAX_OUTPUT], True
    if len(err) > EXEC_MAX_OUTPUT:
        err, cut = err[:EXEC_MAX_OUTPUT], True
    return {"status": "ok", "code": proc.returncode, "out": decode_output(out),
            "err": decode_output(err), "truncated": cut,
            "elapsed": round(time.time() - started, 3), "timeout": timed_out}

def handle_client(conn, manager):
    try:
        conn.settimeout(REQUEST_TIMEOUT)
        data = recv_message(conn)
        request = json.loads(data.decode('utf-8'))
        command = request.get('command')
        if command == 'start':
            with manager.lock:
                manager.stop_openocd()
                files = request.get('files')
                if not files or not isinstance(files, list):
                    response = {"status": "error", "message": "Missing files list"}
                else:
                    success, msg = manager.start_openocd(files)
                    response = {"status": "ok" if success else "error", "message": msg}
        elif command == 'stop':
            with manager.lock:
                success, msg = manager.stop_openocd()
            response = {"status": "ok" if success else "error", "message": msg}
        elif command == 'exec':
            if not manager.allow_exec:
                response = {"status": "error",
                            "message": "exec is disabled: restart the launcher with --allow-exec"}
            else:
                cmd = request.get('cmd')
                if not cmd or not isinstance(cmd, str):
                    response = {"status": "error", "message": "Missing cmd"}
                else:
                    try:
                        timeout = float(request.get('timeout') or REQUEST_TIMEOUT)
                    except (TypeError, ValueError):
                        timeout = REQUEST_TIMEOUT
                    timeout = max(1.0, min(timeout, EXEC_MAX_TIMEOUT))
                    cwd = request.get('cwd') or None
                    logger.info(f"Exec: {cmd} (timeout {timeout:g} s, cwd {cwd or os.getcwd()})")
                    try:
                        response = run_shell(cmd, timeout, cwd)
                    except Exception as e:
                        logger.error(f"Exec failed: {e}")
                        response = {"status": "error", "message": f"{type(e).__name__}: {e}"}
        else:
            response = {"status": "error", "message": f"Unknown command: {command}"}
        send_message(conn, json.dumps(response).encode('utf-8'))
        logger.info(f"Response: {response}")
    except Exception as e:
        logger.error(f"Handle client error: {e}")
        try:
            send_message(conn, json.dumps({"status": "error", "message": str(e)}).encode('utf-8'))
        except:
            pass
    finally:
        conn.close()

def parse_address(addr_str, default_host='127.0.0.1', default_port=12345):
    if ':' in addr_str:
        host, port_str = addr_str.rsplit(':', 1)
        try:
            port = int(port_str)
        except ValueError:
            port = default_port
    else:
        host = addr_str
        port = default_port
    return host, port

def main():
    parser = argparse.ArgumentParser(description="OpenOCD Control Server")
    parser.add_argument('--address', default='0.0.0.0:12345',
                        help='Bind address in format ip:port (default: 0.0.0.0:12345)')
    parser.add_argument('--log-file', help='File to write OpenOCD output (optional)')
    parser.add_argument('--allow-exec', action='store_true',
                        help='Allow the exec command (shell commands on this machine)')
    args = parser.parse_args()

    host, port = parse_address(args.address)
    manager = OpenOCDManager(log_file=args.log_file, allow_exec=args.allow_exec)
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((host, port))
    server.listen(5)
    logger.info(f"Server listening on {host}:{port}")
    if args.log_file:
        logger.info(f"OpenOCD logs will be appended to {args.log_file}")
    if args.allow_exec:
        logger.info("exec is enabled: shell commands from clients are allowed")

    try:
        while True:
            conn, addr = server.accept()
            logger.info(f"Connection from {addr}")
            threading.Thread(target=handle_client, args=(conn, manager), daemon=True).start()
    except KeyboardInterrupt:
        logger.info("Shutting down")
        manager.stop_openocd()
    finally:
        server.close()

if __name__ == '__main__':
    main()