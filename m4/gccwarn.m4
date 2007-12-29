dnl Check if the compiler supports useful warning options.  There's a few that
dnl we don't use, simply because they're too noisy:
dnl
dnl     -Wconversion (useful in older versions of gcc, but not in gcc 2.7.x)
dnl     -Wredundant-decls (system headers make this too noisy)
dnl   -Wpointer-arith I have problems with the glibc.
dnl     -Wtraditional (combines too many unrelated messages, only a few useful)
dnl     -Wcast-qual because with char * cp; const char * ccp;
dnl                 cp = (char *) ccp;
dnl                 is the only portable way to do cp = ccp;
dnl     -pedantic
dnl

#serial 3
AC_DEFUN([ad_GCC_WARNINGS],
[AC_ARG_ENABLE(warnings,
     	       [  --enable-warnings       enable compiler warnings])
 if test "$enable_warnings" = "yes"; then
   CF_GCC_WARNINGS
 fi])

AC_DEFUN([CF_GCC_WARNINGS],
[if test -n "$GCC"; then
  AC_CACHE_CHECK([for gcc warning options], ac_cv_prog_gcc_warn_flags,
  [changequote(,)dnl
  cat > conftest.$ac_ext <<EOF
#line __oline__ "configure"
int main(int argc, char *argv[]) { return argv[argc-1] == 0; }
EOF
  changequote([,])dnl
  cf_save_CFLAGS="$CFLAGS"
  ac_cv_prog_gcc_warn_flags="-W -Wall"
  for cf_opt in \
   Wbad-function-cast \
   Wcast-align \
   Wmissing-declarations \
   Wmissing-prototypes \
   Wnested-externs \
   Wshadow \
   Wstrict-prototypes \
   Wwrite-strings
  do
    CFLAGS="$cf_save_CFLAGS $ac_cv_prog_gcc_warn_flags -$cf_opt"
    if AC_TRY_EVAL(ac_compile); then
      test -n "$verbose" && AC_MSG_RESULT(... -$cf_opt)
	ac_cv_prog_gcc_warn_flags="$ac_cv_prog_gcc_warn_flags -$cf_opt"
	test "$cf_opt" = Wcast-qual && ac_cv_prog_gcc_warn_flags="$ac_cv_prog_gcc_warn_flags -DXTSTRINGDEFINES"
    fi
  done
  rm -f conftest*
  CFLAGS="$cf_save_CFLAGS"])
fi
EXTRA_CFLAGS="${ac_cv_prog_gcc_warn_flags}"
AC_SUBST(EXTRA_CFLAGS)
])dnl
