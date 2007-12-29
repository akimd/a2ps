#! /bin/sh
# -*- ksh -*-

# We test the encoding support by a2ps.
# This is done through a file which dynamically calls several
# encodings.

: ${SRCDIR=.}

# The options to run with
OPT="-Cm2"

# Where is the test file
IN_NAME=encoding.pre

# Output name
OUT_NAME=encoding.ps

# Set up other vars
. $SRCDIR/defs || exit 1

# Run the test
$CHK $OPT $TST_FILE -P $CHK_FILE || exit 1
if test -f $REF_FILE; then :; else
  $REF $OPT $TST_FILE -P $REF_FILE || exit 1
fi

# Return the verdict
cmp $REF_FILE $CHK_FILE 2> /dev/null
exit $?
