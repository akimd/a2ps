#! /bin/sh
# -*- ksh -*-

# This program tests the validity of troncating option

: ${SRCDIR=.}

# The options to run with
OPT="-cC1mq"

# The testing file
IN_NAME=tabulation.pre

# Outputs
OUT_NAME=cut.ps

# Set up other vars
. $SRCDIR/defs || exit 1

# Run the test
$CHK $OPT $TST_FILE -P awkout > $CHK_FILE
if test -f $REF_FILE; then :; else
  $REF $OPT $TST_FILE -P awkout > $REF_FILE
fi

# Return the verdict
cmp $REF_FILE $CHK_FILE 2> /dev/null
exit $?
