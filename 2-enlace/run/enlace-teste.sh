#!/bin/bash

source run/common.sh

echo "Simple test with two hosts directly connected to each other."
echo "(plus a physical hub just to sniffer the traffic)"

run ./fisica-teste   10001 :11001
run ./enlace-teste a 10002 :11002
run ./enlace-teste b 10003 :11003
run ./enlace-teste c 10004 :11004

run ./fisica-hub 11001 :10001  11002 :10002  11003 :10003  11004 :10004

finish
