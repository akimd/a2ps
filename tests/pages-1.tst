#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps handles correctly the page ranges

: ${SRCDIR=.}

# The options to run with
# There are 7 pages to print formfeed.txt.  Use it twice to test
# Page selection of several files.
# Do not use compact mode to check that the pages are anyway correctly
# generated
# Get the first 2, the last 2, and 1 in the middle (one of each file)
# And one which is far too big
OPT="-C2mq --compact=no -a-2,4,7-8,13-,99"

# The testing file
IN_NAME=formfeed.txt

# Outputs
OUT_NAME=pages-1.ps

# Set up other vars
. $SRCDIR/defs || exit 1

# Run the test
$CHK $OPT $TST_FILE $TST_FILE -P awkout > $CHK_FILE || exit 1
if test -f $REF_FILE; then :; else
  $REF $OPT $TST_FILE $TST_FILE -P awkout > $REF_FILE || exit 1
fi

# Return the verdict
cmp $REF_FILE $CHK_FILE 2> /dev/null

exit 0
