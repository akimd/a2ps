#! /bin/sh
# -*- ksh -*-

# guess2.tst
# Check that a2ps is able to use file(1) on stdin.
# Do _not_ report an error if a2ps failed to recognize PostScript files
# when file itself fails.  But display a big warning though.

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

# Get the type verdict from a2ps
get_type='sed -e s/[^(]*(//g;s/)[^)]*//g'

# Check that guesses are OK for stdin filename
# No argument, no content
res=`echo | $CHK --guess --stdin=foo.ps | $get_type` || exit 1
test "$res" = ps || exit 1

# Argument is -, no content
res=`echo | $CHK --guess --stdin=foo.ps - | $get_type` || exit 1
test "$res" = ps || exit 1

# No arg, no specified title, no content
res=`echo | $CHK --guess | $get_type` || exit 1
test "$res" = plain || exit 1

# No arg, no specified title, a ps content
# This test relies upon a good file(1) (i.e. a good etc/magic).
res=`cat $TST_DIR/ehandler | $CHK --guess | $get_type` || exit 1
if test "$res" = ps; then :; else
  # We have failed to recognize a ps file.  Is it because of file(1)?
  set -- `$CHK --guess -vtools $TST_DIR/ehandler 2>&1 | grep '^file(1)'`
  case "$3$4" in
    PostScript|postscript)
      # a2ps is truly responsible
      exit 1
      ;;

    *)
      # a2ps is not responsible
      exec 1>&2
      echo "*****************************************************************"
      echo "* Your file(1) is sadly broken.  It does not recognize PostScript"
      echo "* files.  You may have unexpected results such as pretty printing"
      echo "* of the _source_ of your PostScript documents."
      echo "* To avoid this problem ask your system administrator to update"
      echo "* etc/magic, so that running"
      echo "*    file $TST_DIR/ehandler"
      echo "* correctly answers \`PostScript document'"
      echo "*****************************************************************"
      # Consider the test is succesful
      exit 0
      ;;
    esac
fi

exit 0
