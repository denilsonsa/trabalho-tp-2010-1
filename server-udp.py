#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4 sw=4

# This is a Python version of cods-aula-3/server-udp.c

import sys
import socket

#from __future__ import with_statement # This isn't required in Python 2.6
#from contextlib import closing

def main(argv):
    BUFFERSIZE = 1024
    if len(argv) != 2:
        sys.stderr.write("Erro na inicializacao: server <local port>\n")
        sys.exit(1)

    #sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    # The last parameter can be omitted:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # socket.INADDR_ANY does not work...
    #sock.bind((socket.INADDR_ANY, int(argv[1])))
    sock.bind(("", int(argv[1])))

    while True:
        print "esperando mensagens..."
        # flags parameter can be omitted
        (buffer, addr) = sock.recvfrom(BUFFERSIZE, 0)
        print "mensagem recebida de %s: %s (%d bytes)" % (str(addr), buffer, len(buffer))


if __name__ == '__main__':
    main(sys.argv)
