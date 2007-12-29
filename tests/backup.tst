#! /bin/sh
# -*- ksh -*-

# This program checks that a2ps correctly backups only when needed
# This test tries both -o foo.ps, and -P bak, bak being a printer
# that saves in a file (supposed to be backup'd).
: ${SRCDIR=.}

# Set up other vars
. $SRCDIR/defs || exit 1

# In this test, there are places where it is normal to fail.
set +e

# The output test file
out=$tmpdir/backup.ps

# The options to run with.  Printer `bak' is:
# Printer: bak > #{bak.file}
OPT="-Eplain --version-control=simple --define=bak.file:$out"

# It should first save a backup and then make the new file
/bin/rm -f $out $out~
: > $out
$verbose "New file, -o, backup=simple"
echo 1 | $CHK $OPT -o $out 2>&5 || exit 1
test -f $out~ || exit 1

/bin/rm -f $out $out~
: > $out
$verbose "New file, -P, backup=simple"
echo 2 | $CHK $OPT -P bak 2>&5 || exit 1
test -f $out~ || exit 1

# It should not backup /dev/null (what is not possible, hence raises an error)
echo 3 | $CHK $OPT -o /dev/null 2>&5 || exit 1

echo 4 | $CHK $OPT -P bak --define=bak.file=/dev/null 2>&5 || exit 1

# It should not succeed in backuping up a file which permissions
# would have made output impossible.  Hence, it should fail, not
# create the backup file, nor remove the original
/bin/rm -f $out $out~
: > $out
chmod 077 $out
echo 5 | $CHK $OPT -o $out 2>&5 && exit 1
test -f $out || exit 1
test -f $out~ && exit 1

/bin/rm -f $out $out~
: > $out
chmod 077 $out
echo 6 | $CHK $OPT -P bak 2>&5 && exit 1
test -f $out || exit 1
test -f $out~ && exit 1

# It should not backup when not required :), but succeed
# The options to run with.  Printer `bak' is:
# Printer: bak > #{bak.file}
OPT="-Eplain --version-control=none --define=bak.file:$out"

/bin/rm -f $out $out~
: > $out
echo 7 | $CHK $OPT -o $out 2>&5 || exit 1
test -f $out || exit 1
test -f $out~ && exit 1

/bin/rm -f $out $out~
: > $out
echo 8 | $CHK $OPT -P bak 2>&5 || exit 1
test -f $out || exit 1
test -f $out~ && exit 1

# No need to clean up, done by trap
exit 0
