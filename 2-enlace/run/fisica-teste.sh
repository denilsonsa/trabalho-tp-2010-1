#!/bin/bash

source run/common.sh

echo "Simple test with two hosts directly connected to each other."

run ./fisica-teste 10001 :10002
run ./fisica-teste 10002 :10001

finish
