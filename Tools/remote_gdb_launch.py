#!/usr/bin/env python3
import argparse
import json
import socket
import struct
import sys
import os
import time

# Размер порции записи в сокет. Одиночный send на несколько килобайт уходит одним
# сегментом TCP и молча пропадает там, где MTU пути меньше (туннель с MTU 1328 и
# без ICMP «нужна фрагментация»), поэтому пишем порциями и не даём ядру склеить их.
CHUNK = 1024


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
    raw_len = b''
    while len(raw_len) < 4:
        chunk = sock.recv(4 - len(raw_len))
        if not chunk:
            raise ConnectionError("Server closed connection")
        raw_len += chunk
    msg_len = struct.unpack('>I', raw_len)[0]
    data = b''
    while len(data) < msg_len:
        chunk = sock.recv(msg_len - len(data))
        if not chunk:
            raise ConnectionError("Server closed connection")
        data += chunk
    return data


def send_command(server_addr, command_dict, timeout=30.0, chunk=CHUNK):
    host, port = parse_address(server_addr)
    payload = json.dumps(command_dict).encode('utf-8')
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(timeout)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        sock.connect((host, port))
        send_message(sock, payload, chunk)
        resp_data = recv_message(sock)
        return json.loads(resp_data.decode('utf-8'))

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
    for stream in (sys.stdout, sys.stderr):   # вывод чужой команды может не влезть в консоль
        try:
            stream.reconfigure(errors='replace')
        except Exception:
            pass
    parser = argparse.ArgumentParser(description="OpenOCD Control Client")
    parser.add_argument('--server', default='127.0.0.1:12345',
                        help='Server address in format ip:port (default: 127.0.0.1:12345)')
    parser.add_argument('--stop', action='store_true', help='Send STOP command')
    parser.add_argument('--exec', dest='exec_cmd', metavar='CMD',
                        help='Run a shell command on the launcher machine and show its output')
    parser.add_argument('--exec-timeout', type=float, default=60.0,
                        help='Timeout of the command in seconds (default: 60)')
    parser.add_argument('--cwd', help='Working directory for --exec (default: the launcher one)')
    parser.add_argument('--timeout', type=float, default=None,
                        help='Response timeout in seconds (default: 30, with --exec: command timeout + 15)')
    parser.add_argument('--chunk', type=int, default=CHUNK,
                        help='Chunk size of one socket write in bytes (default: %d)' % CHUNK)
    parser.add_argument('config_files', nargs='*', help='Configuration files to send')
    args = parser.parse_args()

    if args.stop:
        request = {"command": "stop"}
    elif args.exec_cmd:
        request = {"command": "exec", "cmd": args.exec_cmd, "timeout": args.exec_timeout}
        if args.cwd:
            request["cwd"] = args.cwd
    else:
        if not args.config_files:
            print("Error: No configuration files provided.", file=sys.stderr)
            sys.exit(1)
        files_info = []
        for path in args.config_files:
            if not os.path.isfile(path):
                print(f"Error: File not found: {path}", file=sys.stderr)
                sys.exit(1)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            files_info.append({"name": os.path.basename(path), "content": content})
        request = {"command": "start", "files": files_info}
        print("Sending configs: " + ", ".join("%s (%d B)" % (f["name"], len(f["content"]))
                                              for f in files_info))

    timeout = args.timeout or (args.exec_timeout + 15 if args.exec_cmd else 30.0)
    try:
        resp = send_command(args.server, request, timeout=timeout, chunk=args.chunk)
    except socket.timeout:
        print("Error: no response from %s in %.0f s" % (args.server, timeout), file=sys.stderr)
        sys.exit(1)
    except (ConnectionError, OSError) as e:
        print("Error: connection to %s failed: %s" % (args.server, e), file=sys.stderr)
        sys.exit(1)

    if args.exec_cmd:
        if resp.get('status') != 'ok':
            print("Error: " + str(resp.get('message')), file=sys.stderr)
            sys.exit(1)
        out, err = resp.get('out') or '', resp.get('err') or ''
        if out:
            print(out, end='' if out.endswith('\n') else '\n')
        if err:
            print(err, file=sys.stderr, end='' if err.endswith('\n') else '\n')
        tail = []
        if resp.get('timeout'):
            tail.append('TIMEOUT')
        if resp.get('truncated'):
            tail.append('output truncated')
        print("Exit code: %s%s (%.2f s)" % (resp.get('code'),
                                            (' — ' + ', '.join(tail)) if tail else '',
                                            resp.get('elapsed') or 0.0), file=sys.stderr)
        sys.exit(0 if resp.get('code') == 0 else 1)

    print(("Stop response: " if args.stop else "Start response: ") + str(resp))
    if resp.get('status') != 'ok':
        sys.exit(1)

if __name__ == '__main__':
    main()