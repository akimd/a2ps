#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps handles correctly --strip

# This is were you installed the tested version of the program
# (and the directories test, test-ps and diff)

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

# Where is the test file
TEST_FILE=$TST_DIR/prosamp.pre

# Initialize the success
failure=0

for strip_level in 0 1 2 3
do
  # Checked and reference outputs
  REF_FILE=$REF_DIR/strip-${strip_level}.ps
  CHK_FILE=$CHK_DIR/strip-${strip_level}.ps

  # The options to run a2ps with
  OPT="-Epre --strip-level=${strip_level} -P awkout"

  # Generation of the reports
  # It must not crash
  $CHK $OPT $TEST_FILE > $CHK_FILE
  test $? = 0 || exit 1

  # build the ref if not here
  test -f $REF_FILE || $REF $OPT $TEST_FILE > $REF_FILE

  # it must not print the ps header: just receive `OK'
  cmp -s $REF_FILE $CHK_FILE 2> /dev/null || failure=1
done

exit $failure
