# Process this file with autoconf to produce a configure script.
# Configure template for screwim

AC_PREREQ(2.63)
AC_INIT([screwim], [1.0.6], [https://github.com/OOPS-ORG-PHP/mod_screwim])

#AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-override foreign no-dependencies])

AC_CONFIG_SRCDIR([screw.c])
AC_CONFIG_HEADER([config.h])
AC_PREFIX_DEFAULT([/usr/local])

AC_DEFUN([AC_DATAROOTDIR_CHECKED])

AC_ARG_ENABLE(
	[decode],
	AC_HELP_STRING([--enable-decode], [enable decode option [Default: yes]])
	[],[]
)

#AC_REQUIRE_AUX_FILE([tap-driver.sh])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_MAKE_SET

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([getopt.h])

# Checks for library functions
AC_CHECK_LIB(z, deflate,, [AC_MSG_ERROR(z not found!)])
AC_CHECK_FUNCS(getopt_long,, [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])

AC_DEFINE(NO_PHP_MOD, 1, [No php module build])

AC_MSG_CHECKING(checking for support decoding mode)
if test "$enable_decode" != "no"; then
	AC_DEFINE([SUPPORT_DECODE], [1], [support decode mode])
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
