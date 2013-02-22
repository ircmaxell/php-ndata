dnl config.m4 for ndata extension
PHP_ARG_ENABLE(ndata, whether to enable ndata support, [--enable-ndata   Enable NData Support])

if test "$PHP_NDATA" = "yes"; then
  AC_MSG_CHECKING([Checking for supported PHP versions])
  PHP_NDATA_FOUND_VERSION=`${PHP_CONFIG} --version`
  PHP_NDATA_FOUND_VERNUM=`echo "${PHP_NDATA_FOUND_VERSION" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
  if test "$PHP_NDATA_FOUND_VERNUM" -lt "50300"; then
    AC_MSG_ERROR([not supported. Need a PHP version >= 5.3.0 (found $PHP_NDATA_FOUND_VERSION)])
  else
    AC_MSG_RESULT([supported ($PHP_NDATA_FOUND_VERSION)])
  fi

  AC_DEFINE(HAVE_NDATA, 1, [Compile with ndata support])

  PHP_NEW_EXTENSION(ndata, php_ndata.c narray.c, $ext_shared)
fi
