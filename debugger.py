#!/usr/bin/env python3
import asyncio
import time
import socket
import fcntl
import os
import errno

NETLOADER_COMM_PORT = 17491

async def find3DS(port = None):
    if port == None:
        port = NETLOADER_COMM_PORT
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(('0.0.0.0', port))
    sock.setblocking(0)
    try:
        while True:
            sock.sendto(b"3dsboot", ('<broadcast>', port))
            try:
                data, address = sock.recvfrom(2048)
            except BlockingIOError:
                pass
            else:
                if data == b"boot3ds":
                    sock.close()
                    return address
            await asyncio.sleep(1)
    except asyncio.CancelledError:
        pass
    sock.close()

async def main_loop():
    import argparse
    parser = argparse.ArgumentParser(description='This is a description of sorts')
    parser.add_argument("-o", "--host", default="127.0.0.1",
                        help="What host to connect to????")
    parser.add_argument("-p", "--port", default=NETLOADER_COMM_PORT,
                        help="What port to connect to????")

    args = parser.parse_args()
    
    async def handle_client(reader, writer):
        while not writer.is_closing():
            line = await reader.readline()
            if line == b"":
                break
            print(line.decode(), end="")
        
    while True:
        server = await asyncio.start_server(handle_client, args.host, args.port)
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main_loop())