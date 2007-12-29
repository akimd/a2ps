#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps handles correctly --file-align
# with non delegated files.

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

# Where is the test file
TEST_FILE=$TST_DIR/3-pages.txt

# Initialize the success
failure=0

for file_alignment in virtual rank page sheet 3
do
  # Checked and reference outputs
  REF_FILE=$REF_DIR/al1-${file_alignment}.ps
  CHK_FILE=$CHK_DIR/al1-${file_alignment}.ps

  # The options to run a2ps with
  OPT="-Eplain -4 --file-align=${file_alignment} -Pawkout -s2"

  # Generation of the reports
  # It must not crash
  $verbose "testing: --file-align=${file_alignment}"
  $CHK $OPT $TEST_FILE $TEST_FILE > $CHK_FILE 2>&5

  # build the ref if not here
  if test -f $REF_FILE; then :; else
    $REF $OPT $TEST_FILE $TEST_FILE > $REF_FILE 2>&5
  fi

  # Compare
  cmp -s $REF_FILE $CHK_FILE 2> /dev/null
done

exit 0
