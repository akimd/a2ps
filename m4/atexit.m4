## --------------------------------------------------------- ##
##  Check if atexit is available, and replace if necessary   ##
##                           demaille@inf.enst.fr            ##
## --------------------------------------------------------- ##
dnl
dnl acconfig.h should contains these two lines
dnl /* Define to rpl_exit if the replacement function should be used.  */
dnl #undef exit
dnl and atexit.c should be available

# serial 2

AC_DEFUN([ad_FUNC_ATEXIT],
  [AC_CHECK_FUNCS(atexit)
   if test $ac_cv_func_atexit = no; then
     AC_CHECK_FUNCS(on_exit)
     AC_LIBOBJ(atexit)
     if test $ac_cv_func_on_exit = no; then
       AC_DEFINE_UNQUOTED(exit, rpl_exit,
			  [Define to rpl_exit if the replacement function should be used])
     fi
   fi])
