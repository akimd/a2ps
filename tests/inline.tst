#! /bin/sh
# -*- ksh -*-

# This program verifies that any inline function is static.
# This test is meant to avoid problems I had on IRIX.

: ${SRCDIR=.}

# Set up other vars
. $SRCDIR/defs || exit 1

# Get the guilty lines in *.c files
# Omit strfime.c and confg.c, coz' their inlines are safe.
# Exclude the word inlined.
res=`(cd $top_srcdir && fgrep 'inline' src/*.c lib/*.c) |
     egrep -v '__inline|static|inlined|lib/(confg|strftime)\.c' |
     sort -u`
if test -n "$res"; then
  echo "The following files use non static inline:"
  echo "$res"
  exit 1
fi

# No inline allowed in a header.
res=`(cd $top_srcdir && fgrep 'inline' src/*.h lib/*.h) | sort -u`
if test -n "$res"; then
  echo "The following headers use inline:"
  echo "$res"
  exit 1
fi

exit 0
