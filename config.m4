dnl config.m4 for extension mod_screwim

PHP_ARG_WITH(screwim, for screwim support,
dnl Make sure that the comment is aligned:
[  --with-screwim          Include screwim support])

PHP_ARG_ENABLE(screwim-decrypt, whether to enable ScrewIm decrypt api support,
[  --enable-screwim-decrypt Support screwim_decrypt function [[default=no]]], no, no)

AC_DEFINE_UNQUOTED(SCREWIM_NAME, "ScrewIm", [Define to ScrewIm Name])
AC_DEFINE_UNQUOTED(SCREWIM_STRING, "PHP Screw Improved", [Define to ScrewIm Full Name])
AC_DEFINE_UNQUOTED(SCREWIM_VERSION, "1.0.3", [Define to ScrewIm version])

dnl {{{ +-- function config_screwim_random ()
function config_screwim_random () {
	if test $# -ne 3 ; then
		echo "ERROR: sreandom start_range end_range var_name" >& /dev/sterr
		exit 1
	fi

	local start=$1
	local end=$2
	local vname=$3
	local r

	r=$RANDOM
	r=$(( $r % $end + $start ))
	if test $r -gt $end ; then
		r=$(( $r - $end + $start ))
	fi

	eval "$vname=$r"
}
dnl }}}

dnl {{{ +-- function config_screwim_seq() {
function config_screwim_seq() {
	local start=$1
	local end=$2

	while test true
	do
		echo $start
		let start+=1
		test $start -gt $end && break
	done
}
dnl }}}

config_screwim_random 5 10 seed_no

SCREWIM_ENC_DATA=
for i in `config_screwim_seq 1 $seed_no`
do
	config_screwim_random 500 32767 rand_no
	SCREWIM_ENC_DATA="$SCREWIM_ENC_DATA, $rand_no"
done
SCREWIM_ENC_DATA="$(echo $SCREWIM_ENC_DATA | sed 's/^,[ ]\+//g')"

AC_DEFUN([AC_SCREWIM_UNDEFINE], [
	cp confdefs.h confdefs.h.tmp
	grep -v $1 < confdefs.h.tmp > confdefs.h
    rm confdefs.h.tmp
])


if test "$PHP_SCREWIM" != "no"; then
	CFLAGS="${CFLAGS} -Wall"
	AC_DEFINE_UNQUOTED(SCREWIM_ENC_DATA, $SCREWIM_ENC_DATA, [Define to ScrewIm encrypt SEED key])
	PHP_NEW_EXTENSION(screwim, php_screwim.c zencode.c, $ext_shared,, \\$(CFALGS))

	if test "$PHP_SCREWIM_DECRYPT"; then
		AC_DEFINE(SCREWIM_DECRYPT, 1, [define to support ScrewIm decrypt api ])
	fi

	if test "$PHP_SCREWIM_SHARED" = "yes"; then
		AC_SCREWIM_UNDEFINE(PACKAGE_BUGREPORT)
		AC_SCREWIM_UNDEFINE(PACKAGE_NAME)
		AC_SCREWIM_UNDEFINE(PACKAGE_STRING)
		AC_SCREWIM_UNDEFINE(PACKAGE_TARNAME)
		AC_SCREWIM_UNDEFINE(PACKAGE_URL)
		AC_SCREWIM_UNDEFINE(PACKAGE_VERSION)
	fi
fi
