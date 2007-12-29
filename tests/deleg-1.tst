#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps handles correctly the delegations

: ${SRCDIR=.}

# The options to run with
OPT="-ZEtest_delegation"

# The testing file
IN_NAME=report.pre

# Outputs
OUT_NAME=del.ps

# Set up other vars
. $SRCDIR/defs || exit 1

# Run the test
eval $CHK $OPT $TST_FILE -P $CHK_FILE
if test -f $REF_FILE; then :; else
  eval $REF $OPT $TST_FILE -P $REF_FILE
fi

# Return the verdict
cmp $REF_FILE $CHK_FILE 2> /dev/null
exit $?
