dnl config.m4 for extension mod_scwreim

PHP_ARG_WITH(scwreim, for scwreim support,
dnl Make sure that the comment is aligned:
[  --with-scwreim             Include scwreim support])

dnl Otherwise use enable:

if test "$PHP_scwreim" != "no"; then
  dnl Action..
  PHP_NEW_EXTENSION(screwim, php_screw.c zencode.c, $ext_shared)
fi
