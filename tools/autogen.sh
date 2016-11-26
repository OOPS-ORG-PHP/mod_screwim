#!/bin/sh

export LIBTOOL=/usr/bin/libtool
export AUTOMAKE=/usr/bin/automake
export ACLOCAL=/usr/bin/aclocal

# chekc config.h of mod_screwim
if [ ! -f ../config.h ]; then
	echo "ERROR: Not found ../config.h" >& /dev/stderr
	echo "       First, move location to \"../\" and run \"./configure\""
	exit 1
fi

rm -f configure autotool/{config.guess,config.sub,ltmain.sh}
autoreconf --install
rm -rf autom4te* *~

exit 0
