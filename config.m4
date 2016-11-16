dnl config.m4 for extension php_screw
dnl don't forget to call PHP_EXTENSION(php_screw)
dnl If your extension references something external, use with:

PHP_ARG_WITH(php_screw, for php_screw support,
dnl Make sure that the comment is aligned:
[  --with-php_screw             Include php_screw support])

dnl Otherwise use enable:

if test "$PHP_php_screw" != "no"; then
  dnl Action..
  PHP_NEW_EXTENSION(php_screw, php_screw.c zencode.c, $ext_shared)
fi
