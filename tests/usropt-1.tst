#! /bin/sh
# -*- ksh -*-

# Check that the user options work correctly.

# The testing file
IN_NAME=tabulation.pre

. ./defs || exit 1

# -=useroption-test is --guess, so only 1 line is expected

# The file is after the user option
lines=`$CHK -=useroption-test $TST_FILE | wc -l`
test $lines = 1 || exit 1

# The file is before the user option
lines=`$CHK $TST_FILE -=useroption-test | wc -l`
test $lines = 1 || exit 1

# Success
exit 0
