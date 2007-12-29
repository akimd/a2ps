#! /bin/sh
# -*- ksh -*-

: ${SRCDIR=.}
. $SRCDIR/defs || exit 1

# This program compares what produces two distinct versions of the program

# Same but with -g
GREF_DIR=$srcdir/gps-ref
GCHK_DIR=$builddir/gps-chk

# Here are stored the diff of outputs by both programs
DIFF=$builddir/ps-diff
GDIFF=$builddir/gps-diff

# Just used for the banner
whites="                                            "

# Remove trash
/bin/rm -f $TST_DIR/*~ $PS_DIFF $GPS_DIFF 2> /dev/null

# Make sure the directories exist
for dir in $GREF_DIR $GCHK_DIR $DIFF $GDIFF
do
  test -d "$dir" || mkdir $dir
  chmod u+wrx $dir
done

echo "-------------------------------------"
echo "   Comparing generated PostScript"
echo "-------------------------------------"

# We explicitely give the style sheet to use, because:
# - it avoids the problems with broken file(1)
# - it is not the purpose of this test to test automatic style selection
# - some languages share the same suffix

for pair in $TEST_FILES
do
  filename=`echo $pair | sed "s/:.*//"g`
  lang=`echo $pair | sed "s/[^:]*://"g`
  bannerfile=`echo "$filename$whites"  | sed -e 's/^\(.\{18\}\).*/\1/'`
  bannerlang=`echo "($lang)...$whites" | sed -e 's/^\(.\{16\}\).*/\1/'`
  echo $echo_n "$bannerfile$bannerlang$echo_c"
  if [ ! -f "$TST_DIR/$filename" ]; then
    echo $ac_t "is not a test file"
    continue
  fi

  # If the file has a `.', then sed the suffix to ps
  # Otherwise, add `.ps'
  if echo "$filename" | grep \\. >/dev/null; then
    psfilename=`echo $filename | sed -e 's/\..*/.ps/g'`
  else
    psfilename=$filename.ps
  fi

  # The name of the input file.
  file=$TST_DIR/$filename

  # What produces this tested program?
  $CHK -E$lang -P awkout $file > $CHK_DIR/$psfilename 2>&5
  $CHK -Cgs2 -E$lang -P awkout $file > $GCHK_DIR/$psfilename 2>&5

  # Build the references if missing, and protect them.
  if [ ! -r "$REF_DIR/$psfilename" ]; then
    $REF -E$lang -P awkout $file > $REF_DIR/$psfilename 2>&5 || :
  fi
  if [ ! -r "$GREF_DIR/$psfilename" ]; then
    $REF -Cgs2 -E$lang -P awkout $file > $GREF_DIR/$psfilename 2>&5 || :
  fi

  # Compute the diffs.  In a subshell and with `:' to avoid set -e
  # catches the exit status of diff.
  if (cmp $REF_DIR/$psfilename $CHK_DIR/$psfilename) >/dev/null 2>&1; then
    # Identical.
    rm -f $DIFF/$filename
    nbdiff=0
  else
    # Different.
    (diff $REF_DIR/$psfilename $CHK_DIR/$psfilename > $DIFF/$filename || :)
    nbdiff=`wc -l <$DIFF/$filename`
  fi
  if (cmp $GREF_DIR/$psfilename $GCHK_DIR/$psfilename) >/dev/null 2>&1; then
    # Identical.
    rm -f $GDIFF/$filename
    nbdiffsym=0
  else
     # Different.
   (diff $GREF_DIR/$psfilename $GCHK_DIR/$psfilename > $GDIFF/$filename || :)
    nbdiffsym=`wc -l <$GDIFF/$filename`
  fi

  case "$nbdiff$nbdiffsym" in
    00) echo "Ok";;
    *)  echo "Bad: -ng -> $nbdiff, -g -> $nbdiffsym";;
  esac
done

# Sumary of the diff's.  Protect with `||:' the commands that may
# exit != 0 (because of set -e).
cd $DIFF
# I don't use `echo *`, since I don't know how the various shells behave
# when there are no files.
if test -n "`ls`"; then
  (wc -l `ls` | sort -u | grep -v "^[ \t]*0"||:) > \
    $builddir/sum-ps-diff
  echo "  There are differences with reference PostScript files:"
  cat $builddir/sum-ps-diff
  failure=1
fi

cd $GDIFF
if test -n "`ls`"; then
  (wc -l `ls` | sort -u | grep -v "^[ \t]*0"||:) > \
    $builddir/sum-gps-diff
  echo "  There are differences with reference PostScript files (with -g):"
  cat $builddir/sum-gps-diff
  failure=1
fi

exit $failure
