#! /bin/sh
# -*- ksh -*-

# This program tries most common options of the program

# This is were you installed the tested version of the program
# (and the directories test, test-ps and diff)

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

pairs="includeres:perl
psmandup:sh
run-help:zsh
strange.mail:mail
ehandler:ps"

for pair in $pairs
do
  # What are the filename and its type
  file=`echo $pair | sed "s/:.*//"g`
  lang=`echo $pair | sed "s/[^:]*://"g`
  
  # Check that a2ps agrees
  res=`$CHK --guess $TST_DIR/$file | sed -e 's/[^(]*(//g' | sed -e 's/)[^)]*//g'`
  if test "$res" != $lang; then
    has_failed=yes;
    cant_recognize="$cant_recognize $lang"
  fi
done

# Understandable report of the problems
if test x$has_failed = xyes; then
  echo "Your /etc/magic file seems a bit junky."
  echo "It is unable to recognize the type of$cant_recognize files"
  echo "You might run into bad guesses with automatic pretty-printing"
  echo "-- Please note that this is not an error from a2ps."
  echo "-- Don't be worried by this failure, nor should you report it"
fi

exit 0
