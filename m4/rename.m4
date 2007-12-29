# -*- Autoconf -*-

## -------------------------------------------------- ##
##  Check if rename(2) is available and working.      ##
##  Replace if necessary                              ##
##                           akim@epita.fr            ##
## -------------------------------------------------- ##

# rename.c should be available

# serial 2

AC_DEFUN([ad_CHECK_FUNC_RENAME],
[AC_CHECK_FUNCS(rename)
if test $ac_cv_func_rename = yes; then
  AC_CACHE_CHECK([whether rename works], ac_cv_func_rename_works,
  [ac_cv_func_rename_works=no
   : >conftest.1
   : >conftest.2
   AC_TRY_RUN([#include <stdio.h>
int main () { exit (rename ("conftest.1", "conftest.2")); }],
   test -f conftest.1 || ac_cv_func_rename_works=yes,,:)])
fi])

AC_DEFUN([ad_REPLACE_FUNC_RENAME],
[AC_REQUIRE([ad_CHECK_FUNC_RENAME])dnl
AC_REQUIRE([AC_HEADER_STAT])dnl
if test "$ac_cv_func_rename" = no ||
   test "$ac_cv_func_rename_works" = no; then
  AC_CHECK_FUNCS(link)
  # FIXME: Define MVDIR
  AC_LIBOBJ(rename)
  AC_DEFINE_UNQUOTED(rename, rpl_rename,
   [Define to the replacement function if should be used.])
fi])
