#!/bin/bash

source run/common.sh

NUMHOSTS=4

echo "A single link-layer switch connected to $NUMHOSTS hosts."
echo "(plus a physical-layer client just to sniffer and tamper with the traffic)"

run ./fisica-teste 10000 :11000

SWITCHPARAMS="11000 :10000"
for i in `seq $NUMHOSTS`;
do
	# 10000 are client ports
	# 11000 are hub ports
	run ./enlace-teste $i $((10000 + i)) :$((11000 + i))
	SWITCHPARAMS="$SWITCHPARAMS $((11000 + i)) :$((10000 + i))"
done

run ./enlace-switch $SWITCHPARAMS

finish
