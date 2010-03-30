#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4 sw=4

# This is a Python version of cods-aula-3/client-udp.c

import sys
import socket

#from __future__ import with_statement # This isn't required in Python 2.6
#from contextlib import closing


def main(argv):
    if len(argv) != 3:
        sys.stderr.write("Erro na inicializacao: client <remote server IP> <remote server Port>\n")
        sys.exit(1)

    servIP = argv[1]
    servPort = int(argv[2])

    #sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    # The last parameter can be omitted:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    addr = (servIP, servPort)

    buffer = raw_input("digite uma mensagem:\n")
    bytes_sent = sock.sendto(buffer, addr)
    # I could have specified the flags:
    # socket.sendto(buffer, 0, addr)

    sys.stdout.write("Bytes sent: %d\n" % (bytes_sent,))
    sock.close()


if __name__ == '__main__':
    main(sys.argv)
