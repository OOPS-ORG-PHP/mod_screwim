#!/bin/sh

export LIBTOOL=/usr/bin/libtool
export AUTOMAKE=/usr/bin/automake
export ACLOCAL=/usr/bin/aclocal
rm -f configure autotool/{config.guess,config.sub,ltmain.sh}
autoreconf --install
rm -rf autom4te* *~

exit 0
