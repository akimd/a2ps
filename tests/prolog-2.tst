#! /bin/sh
# -*- ksh -*-

# Compare two prologues for -1 and -4 (used in align.tst)
: ${SRCDIR=.}

# Option to run a2ps with
OPT="-Eplain -Pawkin"

# The testing file
IN_NAME=formfeed.txt

# Set up other vars
. $SRCDIR/defs || exit 1

for nup in 1 4
do
  # Checked and reference outputs
  ref_file=$REF_DIR/prolog-${nup}.ps
  chk_file=$CHK_DIR/prolog-${nup}.ps

  $verbose "Running a2ps -$nup $OPT $TST_FILE"
  $CHK -$nup $OPT $TST_FILE >$chk_file 2>&5
  if test ! -f $ref_file; then
    $verbose "Running ref-a2ps -$nup $OPT $TST_FILE"
    $REF -$nup $OPT $TST_FILE >$ref_file 2>&5
  fi
  cmp -s $ref_file $chk_file
done

exit $failure
