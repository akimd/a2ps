# -*- Autoconf -*-

# Check for the existence of a program, resolving it either to the
# full name or short name.

# This is ugly work, don't use it.

# serial 2


AC_DEFUN([ad_ENABLE_PATHS],
[AC_MSG_CHECKING(whether paths should be hardcoded)
AC_ARG_ENABLE(paths,
              [  --enable-paths          hard code the path of the tools])
test "$enable_paths" = "yes" || enable_paths=no
AC_MSG_RESULT($enable_paths)])

# ad_CHECK_PROG(PROGRAM[, COMMENT ON FAILURE])
# --------------------------------------------
# defines $COM_PROGRAM to `#' if PROGRAM is not available, to `' otherwise
# and defines $PROGRAM to PROGRAM if paths should not be hardcoded,
# and to the path to PROGRAM otherwise.
AC_DEFUN([ad_CHECK_PROG],
[if test "$enable_paths" = "no"; then
  AC_CHECK_PROG(COM_$1, $1, yes, no)
  $1="$1";
  if test "[$]COM_$1" = "yes"; then
    COM_$1="";
  else
    COM_$1="#";
  fi
else
  AC_PATH_PROG($1, $1, [#])
  if test "$1" = "#"; then #  not found
    $1="$1" # let the name of the program as path
    COM_$1="#"
  else
    COM_$1=""
  fi
fi
m4_ifval([$2],
[if test "[$]COM_$1" = "#"; then
  AC_MSG_WARN([============================================================
$2])
  AC_MSG_WARN([============================================================])
fi])
AC_SUBST($1)
AC_SUBST(COM_$1)])

# ad_CHECK_PROG(PROGRAM[, COMMENT ON FAILURE])
# --------------------------------------------
# defines $COM_PROGRAM to `#' if PROGRAM is not available, to `' otherwise
# and defines $PROGRAM to PROGRAM if paths should not be hardcoded,
# and to the path to PROGRAM otherwise.
AC_DEFUN([ad_CHECK_PROGS],
[m4_foreach([AC_PROG], [$1], [ad_CHECK_PROG(AC_PROG, $2)])])
