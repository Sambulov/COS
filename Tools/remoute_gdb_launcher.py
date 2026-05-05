#!/usr/bin/env python3
import argparse
import json
import os
import shutil
import socket
import struct
import subprocess
import sys
import tempfile
import time
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class OpenOCDManager:
    def __init__(self):
        self.process = None
        self.temp_dir = None
        self.running = False

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
            for cfg in file_paths:
                cmd.extend(['-f', cfg])
            logger.info(f"Starting OpenOCD: {' '.join(cmd)}")
            self.process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            time.sleep(1.0)
            retcode = self.process.poll()
            if retcode is not None:
                _, stderr = self.process.communicate()
                error_msg = stderr.strip() if stderr else "OpenOCD exited immediately"
                logger.error(f"OpenOCD failed: {error_msg}")
                self._cleanup()
                return False, f"OpenOCD launch failed: {error_msg}"
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
            self._cleanup()
            return True, "OpenOCD stopped"
        except Exception as e:
            logger.error(f"Stop error: {e}")
            self._cleanup()
            return False, str(e)

    def _cleanup(self):
        if self.temp_dir and os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir, ignore_errors=True)
        self.process = None
        self.temp_dir = None
        self.running = False

def recv_exact(sock, num_bytes):
    data = b''
    while len(data) < num_bytes:
        chunk = sock.recv(num_bytes - len(data))
        if not chunk:
            raise ConnectionError("Socket closed")
        data += chunk
    return data

def send_message(sock, msg_bytes):
    sock.sendall(struct.pack('>I', len(msg_bytes)) + msg_bytes)

def recv_message(sock):
    raw_len = recv_exact(sock, 4)
    msg_len = struct.unpack('>I', raw_len)[0]
    if msg_len > 10 * 1024 * 1024:
        raise ValueError("Message too large")
    return recv_exact(sock, msg_len)

def handle_client(conn, manager):
    try:
        data = recv_message(conn)
        request = json.loads(data.decode('utf-8'))
        command = request.get('command')
        if command == 'start':
            files = request.get('files')
            if not files or not isinstance(files, list):
                response = {"status": "error", "message": "Missing files list"}
            else:
                success, msg = manager.start_openocd(files)
                response = {"status": "ok" if success else "error", "message": msg}
        elif command == 'stop':
            success, msg = manager.stop_openocd()
            response = {"status": "ok" if success else "error", "message": msg}
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
    """Разбирает строку вида host:port. Если порт не указан, используется default_port."""
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
    parser.add_argument('--address', default='127.0.0.1:12345',
                        help='Bind address in format ip:port (default: 127.0.0.1:12345)')
    args = parser.parse_args()

    host, port = parse_address(args.address)
    manager = OpenOCDManager()
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((host, port))
    server.listen(5)
    logger.info(f"Server listening on {host}:{port}")

    try:
        while True:
            conn, addr = server.accept()
            logger.info(f"Connection from {addr}")
            handle_client(conn, manager)
    except KeyboardInterrupt:
        logger.info("Shutting down")
        manager.stop_openocd()
    finally:
        server.close()

if __name__ == '__main__':
    main()