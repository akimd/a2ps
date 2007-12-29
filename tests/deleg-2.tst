#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps does not delegate with --delegate=no

: ${SRCDIR=.}

# The options to run with
OPT="-Etest_delegation --delegate=no"

# The testing file
IN_NAME=report.pre

# Outputs
OUT_NAME=del.ps

# Set up other vars
. $SRCDIR/defs || exit 1

# a2ps should fail, saying it does not know the sheet test_delegation.ssh
$CHK $OPT $TST_FILE -P void 2>&1 | fgrep 'cannot find style sheet' > /dev/null 2>&1

# Return status of fgrep
exit $?
