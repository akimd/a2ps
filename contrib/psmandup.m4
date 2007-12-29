include(shell.m4)dnl -*- shell-script-*-
#! /bin/sh -e
# -*- ksh -*-

# psmandup --- produce a version of a PS file to print in manual Duplex.

GPL([Copyright (c) 1998, 1999 Akim Demaille, Miguel Santana])

# Author: Akim Demaille <Akim.Demaille@freefriends.org>

# Get the name of the program
program=`echo $0 | sed 's#.*/##g'`

# Local vars
address=0	# Where to put the manual feed feature
back=:		# Print the back side pages.
debug=
file=
front=:		# Print the front side pages.
output=-	# Default is stdout
fixps=${FIXPS:-fixps}
message=
psselect=${PSSELECT:-psselect}
psset=${PSSET:-psset}
tmpdir=`mktemp -d -t psmandup.XXXXXX` || { echo "$program: Cannot create temporary dir!" >&2 ; exit 1; }

# These two must be kept in synch.  They are opposed.
verbose=echo
quiet=:

# The version/usage strings
version="psmandup 2.1 (@GNU_PACKAGE@ @VERSION@)
Written by Akim Demaille.

Copyright (c) 1998-1999 Akim Demaille, Miguel Santana
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

usage="\
Usage: $program FILE
Tries to produce a version of the PostScript FILE to print in manual
Duplex.

Options:
 -h, --help           display this help and exit
 -v, --version        display version information and exit
 -q, --quiet          don't print informational messages
 -o, --output=FILE    save result in FILE.  If FILE is \`-', send to stdout
 -n, --no-fix         don't call fixps to fix PS problems in FILE
 -f, --front          output only the front pages (recto) on the regular
                      tray
 -b, --back           output only the back pages (verso) on the manual
                      feed tray

Produced output is meant for PS level 2 printers which don't support
Duplex printing, but support Manual Feed.  Once the first set of pages
is printed (odd pages), manual feed is asked: introduce the odd pages
to print the even pages on the other side.

Because there is usually a short time out for manually fed jobs, you
should really be next to the printer.  If ever the time out expired,
use the option -b to send only the missing part.

Environment variables FIXPS, PSSELECT and PSSET, if defined, are used
to find the tools.

News, updates and documentation: http://www.inf.enst.fr/~demaille/a2ps/
Report bugs to <bug-a2ps@gnu.org>"

help="Try \`$program --help' for more information."

# Parse command line arguments.
option_without_arguments='vhsqnDbf'

GETOPT([    -v | --v*) echo "$version"; exit 0;;
    -h | --h*) echo "$usage"; exit 0;;
    -q | -s | --s* | --q*) verbose=:; quiet=echo;;
    # Delay debugging so that options parsing does not appear
    -D | --deb*) debug=-D ;;
    -o | --out*) shift ; output=$1 ;;
    -b | --bac*) front= ; back=: ;;
    -f | --fro*) front=: ; back= ;;
    -) # We are working with stdin ;;
       set dummy "$@" "$1"
       shift
       ;;
    -n | --no*) fixps= ;;])

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

# If printing from stdin, save into a tmp file
if test $file = '-'; then
  file=$tmpdir/stdin.ps
  cat > $file
fi

# Fix the file beforehand, so that we can really expect to find the
# page numbers.
if test -n "$fixps"; then
  $fixps $file -o $tmpdir/fixed.ps `$quiet -q`
  file=$tmpdir/fixed.ps
fi

# Get the number of pages of the document
changequote(, )dnl
pagenum=`sed -ne '/^%%Pages:/{
s/%%Pages: \\([0-9]*\\).*$/\\1/p
q
}' $file`
changequote([, ])dnl

# Make the string to get the front pages (even, increasing) if needed
if test -n "$front"; then
  evens="1"
  i=3
  # Build the string to give to psselect
  while test $i -le $pagenum;
  do
    evens="$evens,$i"
    i=`expr $i + 2`
  done
fi

# Make the string to get the back pages (odd, decreasing) if needed
if test -n "$back"; then
  # If the number of pages is odd, we need to insert a blank sheet
  case "$pagenum" in
changequote(, )dnl
    *[13579])
changequote([, ])dnl
      odds="_"
      i=`expr $pagenum - 1 || exit 0`
      ;;
    *) # Odd, and not 0, so at least >= 2
       odds=$pagenum
       i=`expr $pagenum - 2 || exit 0`
       ;;
  esac

  # Make the string to get the second half (odd, decreasing)
  while test $i != 0;
  do
    odds="$odds,$i"
    i=`expr $i - 2 || exit 0`
  done
fi

# If there are both odds and evens to print, the separator is `,'
test -n "$odds" && test -n "$evens" && separator=,

# Reorder the pages
$psselect -q $evens$separator$odds $file > $tmpdir/ordered.ps

# If needed, insert the manual feed request
if test -n "$back"; then
  # The option of psset to ask the manual feed.  If not set,
  # psset just does nothing.
  pssetmanfeed=-m
  # Compute the address, depending on front pages are printed or not.
  if test -n "$front"; then
    address=`expr '(' $pagenum + 1 ')' / 2 + 1`
    message="\
Once the first half of the file printed, insert the sheets stack into
the manual feed tray to print the second half.  Be aware the time out
if usually short.  If it expired, use option -b to proceed."
  else
    address=0
    message="Insert the front pages stack into the manual feed tray."
  fi
fi

# Insert the manual feed request if needed
$psset -n $pssetmanfeed -a $address -o$output $tmpdir/ordered.ps $debug

test -n "$message" && $verbose "$message" 1>&2
exit 0
