#!/bin/bash

source run/common.sh

echo "Three link-layer switches, each connected to 3 hosts."

run ./enlace-teste a 10001 :11001
run ./enlace-teste b 10002 :11002
run ./enlace-teste c 10003 :11003

run ./enlace-teste i 20001 :21001
run ./enlace-teste j 20002 :21002
run ./enlace-teste k 20003 :21003

run ./enlace-teste x 30001 :31001
run ./enlace-teste y 30002 :31002
run ./enlace-teste z 30003 :31003

run ./enlace-switch 11000 :10000  11001 :10001  11002 :10002  11003 :10003
run ./enlace-switch 10000 :11000  21001 :20001  21002 :20002  21003 :20003  30000 :31000
run ./enlace-switch 31000 :30000  31001 :30001  31002 :30002  31003 :30003

finish
