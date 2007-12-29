#! /bin/sh
# -*- ksh -*-

# Check that the user options work correctly.
# Specifically, that option processing is not broken by user options.

# We use a dummy option/value which cannot change the test:
# for instance --stdin stdin-name

# The testing file
IN_NAME=tabulation.pre

. ./defs || exit 1

# -=useroption-test is --guess, so only 1 line is expected
lines=`$CHK -=useroption-test --stdin=stdin-name $TST_FILE 2>/dev/null | wc -l`
test $lines = 1 || exit 1

lines=`$CHK $TST_FILE --stdin=stdin-name -=useroption-test 2>/dev/null | wc -l`
test $lines = 1 || exit 1

lines=`$CHK -=useroption-test $TST_FILE --stdin=stdin-name 2>/dev/null | wc -l`
test $lines = 1 || exit 1

lines=`$CHK $TST_FILE -=useroption-test --stdin=stdin-name 2>/dev/null | wc -l`
test $lines = 1 || exit 1

# Success
exit 0
