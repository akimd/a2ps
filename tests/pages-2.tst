#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps handles correctly the page ranges
# Check that invalid values are rejected.

: ${SRCDIR=.}

# The testing file
IN_NAME=formfeed.txt

# Set up other vars
. $SRCDIR/defs || exit 1

# Now check that some invalid values are rejected
$CHK $FILE -o- --pages=1-2d > /dev/null 2>&1 && exit 1
$CHK $FILE -o- --pages=tocd > /dev/null 2>&1 && exit 1
$CHK $FILE -o- --pages=1,d > /dev/null 2>&1 && exit 1

# Check the behavior when -a is reset
$CHK $FILE -o- --pages=1-2 --pages < /dev/null > /dev/null 2>&1 || exit 1
exit 0
