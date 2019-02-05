AC_PREREQ([2.69])
AC_COPYRIGHT([Copyright 2017 Free Software Foundation, Inc.])

## ---------------- ##
## Initialization.  ##
## ---------------- ##

AC_INIT([GNU a2ps], [4.14], [bug-a2ps@gnu.org])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
m4_pattern_forbid([^jm_[A-Z]])

# Initialize automake
AM_INIT_AUTOMAKE([1.15 foreign dist-bzip2 no-dist-gzip subdir-objects silent-rules -Wall])

AC_CONFIG_HEADERS([config.h])

# Specify the liba2ps version number
LIBVERSION=1:0:0
AC_SUBST(LIBVERSION)
GNU_PACKAGE="GNU $PACKAGE"
AC_DEFINE_UNQUOTED(GNU_PACKAGE, "$GNU_PACKAGE",
		   [The concatenation of the strings `GNU ', and PACKAGE.])
AC_SUBST(GNU_PACKAGE)

# Available translations
ALL_LINGUAS="ca cs da de es fr it ja ko nl no pl pt ru sl sv tr"

## --------------------- ##
## Checks for programs.  ##
## --------------------- ##

AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
jm_PATH_PERL

# Check for the C compiler and the various oddities that may exist
AC_PROG_CC
AC_PROG_CC_C99
gl_EARLY

AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([GNU a2ps requires a C99 capable compiler!])
])

AC_USE_SYSTEM_EXTENSIONS

AC_PROG_YACC
AC_PROG_GPERF
AM_PROG_AR

# I want flex, and only flex
AC_PROG_LEX
AC_MSG_CHECKING([whether LEX is flex])
AS_CASE([`$LEX --version`],
  [flex*],
    [AC_MSG_RESULT([yes])],
  [
    AC_MSG_RESULT([no])
    LEX="$SHELL $missing_dir/missing flex"
    AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
    AC_SUBST([LEXLIB], [''])
])

# Libtool.  By default, don't do shared libs, liba2ps is not widely used.
LT_INIT([disable-shared static])

gl_INIT

## ------------------------------ ##
## checks for library functions.  ##
## ------------------------------ ##

ad_FUNC_SYSTEMPAPERNAME

# Internationalization.
AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external], [need-ngettext])

# Save the cache
AC_CACHE_SAVE

# In path separator character.
AC_DEFINE_UNQUOTED(DIRECTORY_SEPARATOR, '/',
		   [The char used to separate component of a path])
# Between paths separator character.
AC_DEFINE_UNQUOTED(PATH_SEPARATOR, ':',
		   [The char used to separate paths.])
# We define SYNTAX_TABLE so that regex uses syntax tables
AC_DEFINE(SYNTAX_TABLE, 1,
	  [Define to enable syntax table support in `regex'.])

## ---------------------------- ##
## Various extentions/options.  ##
## ---------------------------- ##

# Where to put the emacs files
AM_PATH_LISPDIR

# Shall the path to the tools be hardcoded?
ad_ENABLE_PATHS

# What should get installed?
AC_ARG_ENABLE(extensions,
AS_HELP_STRING(--disable-extensions,small foot print installation),
[case "${enableval}" in
  yes|no) ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --disable-extensions) ;;
esac],[enable_extensions=yes])
AM_CONDITIONAL(EXTENSIONS, test x$enable_extensions = xyes)

AC_MSG_CHECKING(for PostScript fonts path)
# Try to find some PostScript fonts.
# Find out if ghostscript is installed
ac_psfont_path=
for ac_dir in /usr/local/ghostscript/fonts \
  /usr/local/share/ghostscript/fonts;
do
  if test "cd $ac_dir 2>/dev/null && echo *.afm"; then
    ac_psfont_path="$ac_psfont_path:$ac_dir";
  fi
done
dnl Currently, we don't try TeX fonts, because sed expressions
dnl explode.  Anyway, we really need to support kpathsea to get there.
dnl # Try to find where are the TeX fonts.
dnl for ac_dir in /usr/local/TeX /usr/local/tex /usr/local/teTeX;
dnl do
dnl   if test -d "$ac_dir/texmf/fonts/type1"; then
dnl 	# There are fonts here.  Let's fetch all the directories.
dnl 	for ac_dir2 in $ac_dir/texmf/fonts/type1/* \
dnl 		       $ac_dir/texmf/fonts/type1/*/* \
dnl 		       $ac_dir/texmf/fonts/type1/*/*/* \
dnl 		       $ac_dir/texmf/fonts/afm/* \
dnl 		       $ac_dir/texmf/fonts/afm/*/* \
dnl 		       $ac_dir/texmf/fonts/afm/*/*/* ;
dnl 	do
dnl 	  if test -d "$ac_dir2" \
dnl 	     && test -n `echo $ac_dir2/*.afm $ac_dir2/*.gsf $ac_dir2/*.pf?`; then
dnl 	    ac_psfont_path="$ac_psfont_path:$ac_dir2";
dnl 	  fi
dnl 	done
dnl   fi
dnl done
dnl # Remove the leading `:'
if test -n "$ac_psfont_path"; then
  PSFONT_PATH=`echo "$ac_psfont_path" | sed -e 's/^://'`
fi
AC_SUBST(PSFONT_PATH)
AC_MSG_RESULT([$ac_psfont_path])

## ---------------------------------------------------------- ##
## Write a first sketch of a2ps.cfg.  The user should check.  ##
## ---------------------------------------------------------- ##

#
# Check for the file program with follows the links
#
ad_PROG_FILE_LINK
AC_SUBST(FILE_LINK)

# How to send things in the printer queue
ad_PROG_LPR

#
# default medium to use (defaulted to A4)
#
AC_ARG_WITH(medium,
AS_HELP_STRING([--with-medium=medium],
               [specify the default medium (A4, Letter, Legal, etc.) [default=A4]]),
MEDIUM=$with_medium,
if test x"$ac_cv_lib_paper_systempapername" = xyes; then
  MEDIUM=libpaper
else
  MEDIUM=A4
fi)
AC_SUBST(MEDIUM)

#
# default encoding to use (defaulted to latin1)
#
AC_ARG_WITH(encoding,
AS_HELP_STRING([--with-encoding=enc],
               [specify the default encoding (ascii, latin1, latin2... latin6, hp etc.) [default=latin1]]),
ENCODING=$with_encoding,
ENCODING=latin1)
AC_SUBST(ENCODING)

########################
# Nice special printers
########################
# Look for GV or Ghostview
ad_CHECK_PROGS([gv, ghostview])
if test "$COM_gv$COM_ghostview" = "##"; then
  COM_DISPLAY="#"
fi
AC_SUBST(COM_DISPLAY)

#################################
# Helping people with delegations
#################################
# "Compressed" delegation
ad_CHECK_PROG(gzip,
[a2ps works better with gzip.
Consider fetching gzip at any ftp site proposing GNU programs])
ad_CHECK_PROGS([bzip, bzip2])

# DVI delegation
ad_CHECK_PROG(dvips)

# ImageMagick for most image formats
ad_CHECK_PROG(convert)

# HTML delegation
ad_CHECK_PROGS([netscape, html2ps])
if test "x$COM_netscape" = x; then
  AC_CACHE_CHECK([whether netscape support remote-command], [my_cv_remote_com_netscape],
  [$netscape --help >conftest.help 2>&1
  if fgrep -e -remote  conftest.help >/dev/null &&
     fgrep -e -noraise conftest.help >/dev/null; then
     my_cv_remote_com_netscape=yes
  else
     my_cv_remote_com_netscape=no
  fi
  rm -f conftest.help])
  test "$my_cv_remote_com_netscape" = no && COM_netscape='#'
fi

# PDF delegation
ad_CHECK_PROGS([acroread, pdf2ps])
COM_acroread4='#'
AC_SUBST(COM_acroread4)
# If we have acroread, is it a version which supports -shrink etc.?
if test "x$COM_acroread" = x; then
  AC_CACHE_CHECK(whether acroread is acroread 4, a2_cv_com_acroread4,
  [$acroread -helpall >conftest.help
  if fgrep -e -size   conftest.help >/dev/null &&
     fgrep -e -shrink conftest.help >/dev/null; then
     a2_cv_com_acroread4=yes
  else
     a2_cv_com_acroread4=no
  fi
  rm -f conftest.help])
  test "$a2_cv_com_acroread4" = yes && COM_acroread4=''
fi

# Outputting PDF
ad_CHECK_PROGS([distill, ps2pdf])
test "$COM_distill$COM_ps2pdf" = "##" && COM_PS2PDF="#"
AC_SUBST(COM_PS2PDF)

# PostScript delegation.  Check for psutils 1.17
a2_PSUTILS(1, 17)

# ROFF delegation
ad_CHECK_PROG(grog,
[a2ps works better with groff.
Consider fetching groff at any ftp site proposing GNU programs])

# Texinfo delegation
ad_CHECK_PROGS([makeinfo, tex, latex])
COM_TEXI="$COM_tex$COM_makeinfo$COM_dvips"
COM_LATEX="$COM_latex$COM_dvips"
AC_SUBST(COM_TEXI)
AC_SUBST(COM_LATEX)

## ---------- ##
## Epilogue.  ##
## ---------- ##

AC_CONFIG_SUBDIRS(ogonkify)

# Shell scripts
AC_CONFIG_FILES([contrib/card], [chmod +x contrib/card])
AC_CONFIG_FILES([contrib/fixps], [chmod +x contrib/fixps])
AC_CONFIG_FILES([contrib/pdiff], [chmod +x contrib/pdiff])
AC_CONFIG_FILES([contrib/psmandup], [chmod +x contrib/psmandup])
AC_CONFIG_FILES([contrib/psset], [chmod +x contrib/psset])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  doc/Makefile
  man/Makefile
  tests/Makefile tests/defs
  lib/Makefile
  liba2ps/Makefile
  build-aux/Makefile
  etc/Makefile etc/a2ps_cfg etc/a2ps-site.cfg
  po/Makefile.in
  sheets/Makefile
  encoding/Makefile
  ps/Makefile
  afm/Makefile
  contrib/Makefile
  contrib/sample/Makefile
  contrib/emacs/Makefile
  fonts/Makefile
  ppd/Makefile
])

if test x"${srcdir}" != x.; then
  AC_CONFIG_LINKS([man/GNUmakefile:man/GNUmakefile])
  AC_CONFIG_LINKS([man/Makefile.maint:man/Makefile.maint])
fi

AC_OUTPUT

if test $MEDIUM = libpaper; then
  libpaper_msg="
*   (\`libpaper' means a2ps asks the medium to libpaper.  See man paperconf.)"
fi

AC_MSG_RESULT([
*****************************************************************
* a2ps thinks it should be configured the following way:
* - you print on `$MEDIUM' paper$libpaper_msg
* - your charset is `$ENCODING'
* - to send a JOB to the default printer, run `$LPR job'
* - to send a JOB to the printer PRINTER, run `$LPR $LPR_QUEUE_OPTION PRINTER job'
* - to ask `file(1)' to follow the links, run `$FILE_LINK'
*   (`file(1)' is used to guess the type of the files to print.)
*
* If a2ps is wrong somewhere, fix the `System Dependent parameters'
* in `etc/a2ps-site.cfg'.  See the README file.
*****************************************************************
])
