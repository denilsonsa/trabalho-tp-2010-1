#!/bin/bash

run() {
	echo "$@"
	xterm -geometry 50x10 -bg black -bd white -fg white -title "$*" -e "$@" &
	# -bg = background color
	# -bd = border color
	# -fg = foreground color
}

finish() {
	echo "Press Ctrl+C here to close all windows opened by this script."
	wait
}
