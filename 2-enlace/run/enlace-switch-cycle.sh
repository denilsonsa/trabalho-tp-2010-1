#!/bin/bash

source run/common.sh

echo "Two hosts, each one connected to one switch."
echo "However, each switch has a double link to the other switch."
echo "This creates a link cycle."

run ./enlace-teste a 10001 :11001

run ./enlace-switch 11001 :10001  20001 :22001  20002 :22002
run ./enlace-switch 11002 :10002  22001 :20001  22002 :20002

run ./enlace-teste b 10002 :11002

finish
