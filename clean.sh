#!/bin/bash

[ -f Makefile ] && make distclean

rm -rf include autom4te* build modules
rm -f Makefile.g* Makefile.f* Makefile.o* Makefile
rm -f config.log config.nice config.g* config.s* config.h* ac* configure* mkinstalldirs
rm -f ltmain.sh missing run-tests.php install* libtool tools/screw .deps
