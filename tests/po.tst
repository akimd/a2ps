#! /bin/sh

: ${SRCDIR=.}

# Set up other vars
. $SRCDIR/defs || exit 1

for file in `(cd $top_srcdir && fgrep -l '_(' lib/*.c src/*.c)`
do
  if fgrep "$file" $top_srcdir/po/POTFILES.in >/dev/null 2>&1; then :; else
    echo "Did not see $file in POTFILES.in."
    failure=1
  fi
done

exit $failure
