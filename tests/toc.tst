#! /bin/sh
# -*- ksh -*-

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

## In this test, a2ps is called on the regular test files.
## This is because it happened that it crashed _only_ when
## printing several files.  Don't take all the content of
## test_files, because encoding.pre generates a super big
## prologue which is of no use here but wasting space.

# But it was dumb to lose so much time.  Now it makes sure that --toc
# in conjunction with --pages=toc is OK.  Actually, we use
# --pages=1,toc just to make sure :)

# The options to run with
OPT="-Eplain -Cmq2 --compact=no -a1,toc --toc=#{toc_test} -P awkout"

# Where are the test files
FILES=`echo $TEST_FILES | sed "s#\\([^ :]*\\):[^ ]*#$TST_DIR/\\1#g"`

# Checked and reference outputs
REF_FILE=$REF_DIR/toc.ps
CHK_FILE=$CHK_DIR/toc.ps

# Run the test
$CHK $OPT $FILES > $CHK_FILE 2>&5
if test -f $REF_FILE; then :; else
  $REF $OPT $FILES > $REF_FILE 2>&5
fi

# Return the verdict
cmp $REF_FILE $CHK_FILE 2>/dev/null

exit 0
