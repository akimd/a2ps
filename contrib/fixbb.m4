include(shell.m4)
#! /bin/sh -e
# -*- ksh -*-

# Get the name of the program
program=`echo $0 | sed 's#.*/##g'`

# Look for a running ghostscript
gs=${GHOSTSCRIPT:-${GS:-gs}}
tmpdir=`mktemp -d -t fixbb.XXXXXX` || { echo "$program: Cannot create temporary dir!" >&2 ; exit 1; }

# fixbb -- Fix bounding box info in postscript files.
# Written 2/92 by ggh@cs.brown.edu,
# using an idea of Doug Crabill (dgc@cs.purdue.edu).
# Revised 9/30/92: Use GNU utilities, make backup copy of input.
# Revised 3/18/93: Set PATH explicitly.
# Revised 4/23/93: Hack FrameMaker header so that it doesn't set paper size.
# 04/21/99: Changed by hf@nike.phys.chemie.uni-muenchen.de to run with gs5.x
#           and some sort of eps-files.
#------------------------------------------------------------------------------

help="Try \`$program --help' for more information."

# Parse command line arguments.
option_without_arguments='vhsqDf'

GETOPT([[    -v | --v*) echo "$version"; exit 0;;
    -h | --h*) echo "$usage"; exit 0;;
    -q | -s | --s* | --q*) verbose=:;;
    # Delay debugging so that options parsing does not appear
    -D | --debug) debug=: ;;
    -o | --output) shift ; output=$1 ;;
    -f | --force)
     # Refuse if gs does not seem to work
     if test "x$gs" = x; then
       echo "$program: error: ghostscript does not work." >&2
       exit 1
     else
       run_gs=1
     fi
     ;;
    -) # We are working with stdin ;;
      set dummy "$@" "$1"; shift;;]])

# Check the number of arguments.
case $# in
  0)  file=-;;
  1)  file=$1;;
  *)  echo "$program: too many arguments" 1>&2
      echo "$help"  1>&2
      exit 1;;
esac

if test -n "$debug"; then
  # Set -x now if debugging
  set -x
else
  # Temp dir.  Get ready not to leave junk (if not debugging)
  trap "/bin/rm -rf $tmpdir" 0 1 2 3 13 15
fi

#------------------------------------------------------------------------------
# Render image using GhostScript.
#------------------------------------------------------------------------------

# Does ghostscript run?
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
if ($gs -v) >/dev/null 2>&1; then :; else
  echo "Cannot not run $gs" >&2
  exit 1
fi

echo "Computing bounding box: ."

# If gs support the bbox device, then cool!
if ($gs -h | grep bbox) >/dev/null 2>&1; then
  bbox_awk='
/^%%BoundingBox:/ {
  if (bbox_llx < $1)
     bbox_llx = $1 ;
  if (bbox_lly < $2)
     bbox_lly = $2;
  if (bbox_urx > $3)
     bbox_urx = $3;
  if (bbox_ury > $4)
     bbox_urx = $4;
}

/^%%HiResBoundingBox:/ {
  if (hrbbox_llx < $1)
     hrbbox_llx = $1 ;
  if (hrbbox_lly < $2)
     hrbbox_lly = $2;
  if (hrbbox_urx > $3)
     hrbbox_urx = $3;
  if (hrbbox_ury > $4)
     hrbbox_urx = $4;
}

END {
 print "%%BoundingBox:" bbox_llx bbox_lly bbox_urx bbox_ury;
 print "%%HiResBoundingBox:" hrbbox_llx hrbbox_lly hrbbox_urx hrbbox_ury;
}
'
  # I don't know why, but the result is sent onto stderr.
  bbox=`$gs -dNOPAUSE -sDEVICE=bbox $file 2>&1 >/dev/null | awk "$bbox_awk"`
  echo $bbox
  exit 0
else
  echo "Cannot not run $gs -sDEVICE=bbox" >&2
  exit 1
fi

BBOX=`$GS -dNOPAUSE -dQUIET -dBATCH -sDEVICE=epswrite -sOutputFile=- $1 | \
grep "^%%BoundingBox: *[0-9]" | cut -d" " -f2- `

echo -n "."

if [ $? -ne 0 ]; then echo; echo "fixbb: Could not find bbox" >&2; exit 1; fi

echo "$BBOX"

echo -n "Editing $1: ."

#------------------------------------------------------------------------------
# Backup original file and replace with updated version.
#------------------------------------------------------------------------------

cp $1 $1.orig

if [ $? -ne 0 ]; then echo; echo "fixbb: Could not save original file" >&2; exit 1; fi

#------------------------------------------------------------------------------
# Update %%BoundingBox comment in source file, nuke /papersize in Frame output.
#------------------------------------------------------------------------------

awk '\
  /^%%BoundingBox/ {if (!done) print "%%BoundingBox: '"$BBOX"'"; done=1; next}\
  /FrameDict/   {frame=1}\
  /%%EndProlog/ {if (frame) print "FrameDict begin /papersize {false} def end"}\
  /.*/          {print}\
  END           {if (!done) print "%%BoundingBox: '"$BBOX"'"}' \
 $1.orig > $1

if [ $? -ne 0 ]; then
    echo; echo "fixbb: Edit of $1 failed" >&2;
    cp $1.orig $1;
    if [ $? -ne 0 ]; then
        echo;
        echo "fixbb: Could not write back original file saved in $1.orig" >&2;
        exit 1;
    fi
    exit 1;
fi

echo ".done"
