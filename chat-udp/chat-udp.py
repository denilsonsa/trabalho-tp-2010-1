#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4 sw=4

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
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind( ("", localport) )
    keep_going = True

    while keep_going:
        # Windows select() function is handled by WinSock, and does not
        # support non-socket descriptors.
        rlist, wlist, xlist = select.select( [udp_sock, sys.stdin], [], [])
        for ready_fd in rlist:
            if ready_fd is sys.stdin:
                data = sys.stdin.readline()
                if data == '':
                    keep_going = False
                    break
                else:
                    print "Hey, I'm not implemented yet to send your '%s'" % (data,)
            elif ready_fd is udp_sock:
                data, address = udp_sock.recvfrom(4096)
                print "Hey, got something from the tubes: '%s'" % (data,)
                print "xoxo from %s" % (str(address),)
    print "Kthxbye"


if __name__ == '__main__':
    args_dict = parse_arguments(sys.argv[1:])
    main(**args_dict)
