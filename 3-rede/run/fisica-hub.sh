#!/bin/bash

source run/common.sh

NUMHOSTS=4

echo "A single physical hub connected to $NUMHOSTS hosts."


HUBPARAMS=""
for i in `seq $NUMHOSTS`;
do
	# 10000 are client ports
	# 11000 are hub ports
	run ./fisica-teste $((10000 + i)) :$((11000 + i))
	HUBPARAMS="$HUBPARAMS $((11000 + i)) :$((10000 + i))"
done

run ./fisica-hub $HUBPARAMS

finish
