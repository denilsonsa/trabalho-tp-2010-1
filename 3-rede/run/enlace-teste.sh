#!/bin/bash

source run/common.sh

echo "Simple test with three hosts connected to each other using a physical hub."
echo "(plus a physical-layer client just to sniffer and tamper with the traffic)"

run ./fisica-teste   10001 :11001
run ./enlace-teste a 10002 :11002
run ./enlace-teste b 10003 :11003
run ./enlace-teste c 10004 :11004

run ./fisica-hub 11001 :10001  11002 :10002  11003 :10003  11004 :10004

finish
