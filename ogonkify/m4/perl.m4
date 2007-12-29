#serial 2

dnl From Jim Meyering.
dnl Find a new-enough version of Perl.
dnl

AC_DEFUN([jm_PERL],
[
  dnl FIXME: don't hard-code 5.003
  dnl FIXME: should we cache the result?
  AC_MSG_CHECKING([for perl5.003 or newer])
  if test "${PERL+set}" = set; then
    # `PERL' is set in the user's environment.
    candidate_perl_names="$PERL"
    perl_specified=yes
  else
    candidate_perl_names='perl perl5'
    perl_specified=no
  fi

  found=no
  AC_SUBST(PERL)
  PERL="$missing_dir/missing perl"
  for perl in $candidate_perl_names; do
    # Run test in a subshell; some versions of sh will print an error if
    # an executable is not found, even if stderr is redirected.
    if ( $perl -e 'require 5.003' ) > /dev/null 2>&1; then
      PERL=$perl
      found=yes
      break
    fi
  done

  AC_MSG_RESULT($found)
  test $found = no && AC_MSG_WARN([
*** You don't seem to have perl5.003 or newer installed.
*** Because of that, you may be unable to regenerate certain files
*** if you modify the sources from which they are derived.] )
])

dnl The same, but the path to perl is wanted.
dnl
dnl By Akim Demaille, do not flame Jim for this :)
AC_DEFUN(jm_PATH_PERL,
[
  dnl FIXME: don't hard-code 5.003
  dnl FIXME: should we cache the result?
  if test "${PERL+set}" = set; then
    # `PERL' is set in the user's environment.
    candidate_perl_names="$PERL"
    perl_specified=yes
  else
    candidate_perl_names='perl perl5'
    perl_specified=no
  fi

  found=no
  for perl_name in $candidate_perl_names; do
    # Run test in a subshell; some versions of sh will print an error if
    # an executable is not found, even if stderr is redirected.
    AC_PATH_PROG(PERL, $perl_name, no)
    if test "$PERL" != "no" \
       && ( $PERL -e 'require 5.003' ) > /dev/null 2>&1; then
      found=yes
      break
    fi
  done

  AC_SUBST(PERL)
  if test $found = no; then
    PERL="/usr/local/bin/perl"
    AC_MSG_WARN([
*** You don't seem to have perl5.003 or newer installed.
*** Because of that, you may be unable to regenerate certain files
*** if you modify the sources from which they are derived.] )
  fi
])
