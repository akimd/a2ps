#! /bin/sh
# -*- ksh -*-

# Test if the basic options work, and produce on stdout only.

. ./defs || exit 1

tmp=$tmpdir/opt-1

for p in --version --help --list=features --list=printers --list=options \
         --list=media --list=style-sheets --list=delegations \
	 --list=macro-meta-sequences --list=encodings --list=user-options \
         --list=prologues --list=ppd;
do
  # There should be nothing on stderr */
  $verbose "Running a2ps $p"
  # I used to run this test:
  #err=`$CHK $p 2>&1 >/dev/null`
  #test "x$err" = x
  # but it seems that many shells are not able to redirect fd 2 like this.
  $CHK $p >/dev/null 2>$tmp
  test "x`cat $tmp`" = x
done

exit 0
