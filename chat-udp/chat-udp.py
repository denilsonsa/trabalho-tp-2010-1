#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4 sw=4

import fcntl
import os
import os.path
import select, socket
import sys


def print_help(where=sys.stdout):
    basename = os.path.basename(sys.argv[0])
    where.write(
        'Usage: {0} <local port> <remote host>:<port>'.format(basename)
    )


def parse_arguments(args):
    d = {}
    try:
        d['localport'] = int(args[0])
        d['remotehost'], d['remoteport'] = args[1].rsplit(':', 2)
        d['remoteport'] = int(d['remoteport'])
        return d
    except StandardError as e:
        sys.stderr.write('Error passing parameters: ' + str(e) + '\n')
        print_help(sys.stderr)
        sys.exit(1)


def main(localport, remotehost, remoteport):
    # Creating the UDP socket
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind( ("", localport) )
    # If I would be using TCP, maybe it would be a wise idea to set SO_REUSEADDR
    # http://www.cim.mcgill.ca/~franco/OpSys-304-427/messages/node77.html
    # http://oclug.on.ca/archives/oclug/2001-February/003117.html
    # http://stackoverflow.com/questions/1241144/socket-remains-open-after-program-has-closed-c

    # Setting stdin as non-blocking
    # http://stackoverflow.com/questions/375427/non-blocking-read-on-a-stream-in-python/1810703#1810703
    stdin_flags = fcntl.fcntl(sys.stdin, fcntl.F_GETFL)
    fcntl.fcntl(sys.stdin, fcntl.F_SETFL, stdin_flags | os.O_NONBLOCK)

    dest_addr = (remotehost, remoteport)

    print '*** "Ready to serve!" - Peasant from Warcraft II'
    keep_going = True
    try:
        while keep_going:
            # Windows select() function is handled by WinSock, and does
            # not support non-socket descriptors.
            rlist, wlist, xlist = select.select( [udp_sock, sys.stdin], [], [])
            for ready_fd in rlist:
                if ready_fd is sys.stdin:
                    data = sys.stdin.read()
                    if data == '':
                        keep_going = False
                        break
                    else:
                        # Note: send() and sendto() might send fewer
                        # bytes than requested. Since the purpose is to
                        # send very small messages, I'm NOT checking for
                        # the returned value.
                        udp_sock.sendto(data, dest_addr)
                        print "*** Sent to {0}".format(str(dest_addr))
                elif ready_fd is udp_sock:
                    data, address = udp_sock.recvfrom(4096)
                    print "*** Hey, got something from the tubes:"
                    print data
                    print "*** xoxo from {0}".format(str(address))

        print "*** Kthxbye"
    except KeyboardInterrupt:
        print "*** Don't be so rude... A simple EOF would be enough. :'-("

    # Not needed, since the program will terminate anyway.
    udp_sock.close()


if __name__ == '__main__':
    args_dict = parse_arguments(sys.argv[1:])
    main(**args_dict)
