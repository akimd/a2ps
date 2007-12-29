#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps correctly reads stdin, and
# writes on stdout.  In particular, we check both with -Eplain
# specified, and without (i.e., -E alone).

: ${SRCDIR=.}

# failure?
fail=0

# The options to run with
OPT="-o- -"

# The testing file
IN_NAME=report.pre

# Set up other vars
. $SRCDIR/defs || exit 1

# 3. Read stdin, produce on stdout, set automatic ssh selection on
nlines=`cat $TST_FILE | $CHK $OPT -E | wc -l | sed 's/[ \t]//g'`
test $? = 0 || fail=1
test $nlines != 0 || fail=1

exit $fail
