#!/bin/bash

TERMINAL=xterm

run() {
	echo "$@"

	# XXX: Edit the terminal command here!
	$TERMINAL -geometry 50x10 -bg black -bd white -fg white -title "$*" -e "$@" &

	# For xterm:
	# -bg = background color
	# -bd = border color
	# -fg = foreground color
}

finish() {
	echo "Press Ctrl+C here to close all windows opened by this script."
	wait
}


# Some sanity checks below:

if ! which $TERMINAL &>/dev/null ; then
	echo "ERROR: '$TERMINAL' was not found. Please either install it"
	echo "or edit 'run/common.sh' to run another terminal instead."
	exit 1
fi

if [ -z "$DISPLAY" ] ; then
	echo "ERROR: \$DISPLAY var is unset. You won't be able to launch"
	echo "X11 terminal windows. (are you running this over ssh?)"
	exit 1
fi
