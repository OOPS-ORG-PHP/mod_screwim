dnl config.m4 for extension mod_scwreim

PHP_ARG_WITH(scwreim, for scwreim support,
dnl Make sure that the comment is aligned:
[  --with-scwreim             Include scwreim support])

AC_DEFINE_UNQUOTED(SCREWIM_NAME, "ScrewIm", [ ])
AC_DEFINE_UNQUOTED(SCREWIM_STRING, "PHP Screw Improved", [ ])
AC_DEFINE_UNQUOTED(SCREWIM_VERSION, "1.0.0", [ ])

if test "$PHP_scwreim" != "no"; then
  PHP_NEW_EXTENSION(screwim, php_screwim.c zencode.c, $ext_shared)
fi
