#!/usr/bin/env python3
import argparse
import json
import socket
import struct
import sys
import os

def send_message(sock, msg_bytes):
    sock.sendall(struct.pack('>I', len(msg_bytes)) + msg_bytes)

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

def send_command(server_addr, command_dict):
    host, port = parse_address(server_addr)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((host, port))
        send_message(sock, json.dumps(command_dict).encode('utf-8'))
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
    parser = argparse.ArgumentParser(description="OpenOCD Control Client")
    parser.add_argument('--server', default='127.0.0.1:12345',
                        help='Server address in format ip:port (default: 127.0.0.1:12345)')
    parser.add_argument('--stop', action='store_true', help='Send STOP command')
    parser.add_argument('config_files', nargs='*', help='Configuration files to send')
    args = parser.parse_args()

    if args.stop:
        resp = send_command(args.server, {"command": "stop"})
        print(f"Stop response: {resp}")
        if resp.get('status') != 'ok':
            sys.exit(1)
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
        resp = send_command(args.server, {"command": "start", "files": files_info})
        print(f"Start response: {resp}")
        if resp.get('status') != 'ok':
            sys.exit(1)

if __name__ == '__main__':
    main()