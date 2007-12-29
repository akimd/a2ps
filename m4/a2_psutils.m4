dnl                                                        -*- Autoconf -*-
dnl Testing delegations for a2ps: a recent version of psutils
dnl
dnl Usage: a2_PSUTILS(major, minor)
dnl
dnl If psutils version MAJOR, patch level MINOR is available
dnl     SUBST(PSUTILS) to <nothing>
dnl else
dnl     SUBST(PSUTILS) to `#'
dnl Akim.Demaille@inf.enst.fr

# serial 2

AC_DEFUN([a2_PSUTILS],
[ad_CHECK_PROG(psselect)
ad_CHECK_PROG(psnup)
if test "$COM_psselect" = "#"; then
  COM_PSUTILS="#"
else
  # There is one.  Check version > MAJOR.MINOR
  ac_prog_psselect_banner=`psselect -v 2>&1 | sed 1q`
  ac_prog_psselect_release=`set -- $ac_prog_psselect_banner && echo $[3]`
  ac_prog_psselect_patch=`set -- $ac_prog_psselect_banner && echo $[5]`
  test ${ac_prog_psselect_release}0 -lt $1[0] && COM_PSUTILS="#"
  test ${ac_prog_psselect_patch}0 -lt $2[0] && COM_PSUTILS="#"
fi
if test "$COM_PSUTILS" = "#"; then
  AC_MSG_WARN([===========================================================])
  AC_MSG_WARN([a2ps works much better with the psutils.  Available at])
  AC_MSG_WARN([  http://www.dcs.ed.ac.uk/home/ajcd/psutils/])
  AC_MSG_WARN([You *really* should install them *before* installing a2ps.])
  AC_MSG_WARN([===========================================================])
fi
AC_SUBST(COM_PSUTILS)])
