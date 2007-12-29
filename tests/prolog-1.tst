#! /bin/sh
# -*- ksh -*-

# Check that the prologues behave well (no missing files)

: ${SRCDIR=.}

# Option to run a2ps with
OPT="-Esh"

# The testing file
IN_NAME=formfeed.txt

# Set up other vars
. $SRCDIR/defs || exit 1

for p in bold bw color gray gray2 matrix diff
do
  $verbose "Running prologue $p"
  $CHK --prologue=$p $OPT $TST_FILE -o/dev/null
done
