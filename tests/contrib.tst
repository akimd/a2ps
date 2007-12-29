#! /bin/sh
# -*- ksh -*-

# Dummy test that verifies that there are no stupid syntax errors
# in the contributions.

. ./defs || exit 1

for prog in card fixps pdiff psmandup psset
do
  for opt in --version --help
  do
    # There should be nothing on stderr, nor failing
    $verbose "Running $prog $opt"
    err=`${top_builddir}/contrib/$prog $opt 2>&1 1>&5` || exit 1
    test "x$err" != x && exit 1
  done
done

exit 0
