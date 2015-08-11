#!/bin/python
import socket
import sys
import re


SOCKET_PATTERN = "^socketfile[ \t]*?=[ \t]*?([0-9a-zA-Z\\/.]+?)[ \t]*?(?:#.*)?$"
DEFAULT_CONFIGFILE = "ddns.conf"
DEFAULT_SOCKETFILE = "ddns.sock"
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
socket_regex = re.compile(SOCKET_PATTERN)
socket_filename = DEFAULT_SOCKETFILE
with open(DEFAULT_CONFIGFILE) as conf_file:
    for line in conf_file:
        if line[0] == "#":
            continue
        m = socket_regex.match(line)
        if m:
            socket_filename = m.group(1)
            break

try:
    sock.connect(socket_filename)
except:
    print("Failed To Connect", file=sys.stderr)
    sys.exit(1)

try:
    while True:
    # Send data
        message = input("Message: ")
        if message == "q":
            break
        sock.send(message.encode('ascii'))

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print(data, file=sys.stderr)
finally:
    sock.close()
