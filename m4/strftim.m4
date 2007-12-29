## --------------------------------------------------------- ##
##  Check if there is a functioning strftime, and replace    ##
##                           demaille@inf.enst.fr            ##
## --------------------------------------------------------- ##

# serial 2

AC_DEFUN([ad_FUNC_STRFTIME],
[ AC_FUNC_STRFTIME
  AC_STRUCT_TIMEZONE
  AC_CHECK_FUNCS(tzset mblen mbrlen)
  AC_CHECK_HEADERS(limits.h)
  AC_FUNC_MKTIME
  if test $ac_cv_func_strftime = no; then
    AC_LIBOBJ(strftime)
    if test $ac_cv_func_working_mktime = no; then
       AC_DEFINE_UNQUOTED(mktime, rpl_mktime,
	[Define to rpl_mktime if the replacement function should be used.])
    fi
  fi])
