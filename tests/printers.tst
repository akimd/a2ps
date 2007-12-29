#! /bin/sh
# -*- ksh -*-

# The DefaultPrinter is not defined in a2ps-test.cfg, but that's
# no reason to dump!

. ./defs || exit 1

(echo | $CHK -d 2>&1) | grep "^a2ps:" >&5 || exit 1

exit 0
