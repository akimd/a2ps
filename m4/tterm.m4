dnl Make all the needed checks to use tinyterm.[ch]
dnl Akim.Demaille@inf.enst.fr

#serial 2

AC_DEFUN([ad_TINYTERM_CHECKS],
[AC_REQUIRE([AM_HEADER_TIOCGWINSZ_NEEDS_SYS_IOCTL])
 AC_CHECK_HEADERS(termios.h sys/types.h unistd.h limits.h)
 AC_REQUIRE([jm_AC_HEADER_INTTYPES_H])
])
