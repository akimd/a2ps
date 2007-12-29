#! /bin/sh
# -*- ksh -*-

# This program verifies that the use of PARAMS in consistent

: ${SRCDIR=.}

# Set up other vars
. $SRCDIR/defs || exit 1

fail=0

cd $top_srcdir
srcfiles=`echo lib/*.[chily] src/*.[chily]`

# Get all the files that define PARAMS or __P
res=`\
grep -l '#[\t ]*define[\t ]*PARAMS' $srcfiles;\
grep -l '#[\t ]*define[\t ]*__P' $srcfiles`

# These files _must_ check for PROTOTYPES
for i in $res;
do
  case $i in
    # Don't check obstack.h which has a macro starting with __P
    *obstack.h)
      ;;

    *)
      if grep -s PROTOTYPES $i; then :; else
	echo "$i defines PARAMS without checking PROTOTYPES";
	fail=1
      fi
      ;;
  esac
done

exit $fail
