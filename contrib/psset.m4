include(shell.m4)dnl -*- shell-script -*-
#! /bin/sh -e
# -*- ksh -*-

# psset --- Put page device definition somewhere in a PS document.

GPL([Copyright (c) 1999 Akim Demaille, Miguel Santana])

# Author: Akim Demaille <Akim.Demaille@freefriends.org>

# Get the name of the program
program=`echo $0 | sed 's#.*/##g'`

# Local vars
at=0	# Insert call in the document's Setup section.
debug=
file=
fixps=${FIXPS:-fixps}
output=-
pagedevices=	# `;' separated list of `key:value'
quiet=:         # i.e., verbose
tmpdir=`mktemp -d -t psset.XXXXXX` || { echo "$program: Cannot create temporary dir!" >&2 ; exit 1; }
sedscript=$tmpdir/psset.sed

# The version/usage strings
version="$program 1.3 (@GNU_PACKAGE@ @VERSION@)
Written by Akim Demaille.

Copyright (c) 1998-1999 Akim Demaille, Miguel Santana
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

usage="\
Usage: $program FILE

Produce a version of the PostScript FILE with a protected call to the
PostScript operator \`setpagedevice'.  Typical use is making FILE
print duplex, or on the manual tray etc.

Options:
 -h, --help           display this help and exit
 -v, --version        display version information and exit
 -q, --quiet          don't print informational messages
 -o, --output=FILE    save result in FILE.  If FILE is \`-', send to stdout
 -n, --no-fix         don't run \`fixps' to fix PS problems in FILE
changequote(, )dnl
 -S, --setpagedevice=KEY[:VALUE]  pass a page device definition to output.
changequote([, ])dnl
                      Multiple values accumulate
 -a, --at=PAGE        insert the page device definitions at PAGE.  Default
                      is PAGE=0, standing for the Document's Setup section

Shortcuts:
 -m, --manualfeed     alias for -SManualFeed:true
 -s, --simplex        alias for -SDuplex:false
 -d, --duplex         alias for -SDuplex:true -STumble:false
                      corresponds to Duplex, binding along the long edge
 -t, --tumble         alias for -SDuplex:true -STumble:true
                      corresponds to Duplex, binding along the short edge

Produced output is meant for PS level 2 printers.

News, updates and documentation: visit http://www.inf.enst.fr/~demaille/a2ps/
Report bugs to <bug-a2ps@gnu.org>"

help="Try \`$program --help' for more information."

# Parse command line arguments.
option_without_arguments='vhqnDmdts'

GETOPT([    -v | --v*) echo "$version"; exit 0;;
    -h | --h*) echo "$usage"; exit 0;;
    -q | --sil* | --q*) quiet=echo;;
    # Delay debugging so that options parsing does not appear
    -D | --debug) debug=: ;;
    -o | --output) shift ; output=$1 ;;
    -a | --at)     shift ; at=$1 ;;
    -S | --set*)   shift
       case $1 in
         *:*) pagedevices="${pagedevices}$1;" ;;
         *)   echo "$program: invalid argument for -S: $1" 1>&2
              exit 1 ;;
       esac
       ;;
    -m | --man* ) pagedevices="${pagedevices}ManualFeed:true;" ;;
    -d | --dup* ) pagedevices="${pagedevices}Duplex:true;Tumble:false;" ;;
    -t | --tum* ) pagedevices="${pagedevices}Duplex:true;Tumble:true;" ;;
    -s | --sim* ) pagedevices="${pagedevices}Duplex:false;" ;;
    -) # We are working with stdin ;;
      set dummy "$@" "$1"
      shift ;;
    -n|--no-fix) fixps="$fixps -n" ;;])

# Check the number of arguments.
case $# in
  0)  file=;;
  1)  # We want $file to be empty to mean stdin
      if test "x$file" = "x-"; then
        file=
      else
        file=$1
      fi
      ;;
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

# Well, if there is nothing to do, just do nothing.
if test -z "$pagedevices"; then
  exec $fixps `$quiet -q` $file -o "$output"
fi

# Prepare the PostScript snippet we plan to insert if needed
pspagedevice=
saved_IFS=$IFS
IFS=";"
for pagedevice in $pagedevices
do
  IFS=$saved_IFS
  key=`echo $pagedevice | sed -e 's/:.*//g'`
  value=`echo $pagedevice | sed -e 's/.*://g'`
  # I use `cvx exec' in order to let the stopped environment catch the
  # errors.  This way, the user can do -SDuplex:fuubar and have a
  # PS file that behaves well.
  pspagedevice="$pspagedevice
%%BeginFeature: *$key $value
  (<<) cvx exec /$key ($value) cvx exec (>>) cvx exec
  systemdict /setpagedevice get exec
%%EndFeature"
done

pspagedevice="% Pagedevice definitions:
countdictstack
% Push our own mark, since there can be several PS marks pushed depending
% where the failure really occured.
/psset_mark
{$pspagedevice
} stopped
% My cleartomark
{ /psset_mark eq { exit } if } loop
countdictstack exch sub dup 0 gt
{
  { end } repeat
}{
  pop
} ifelse"

# Prepare the sed command we want to run.
# End of lines must be protected by `\' but the last one, and leading
# spaces with a `\' too...
pspagedevicelen=`echo "$pspagedevice" | wc -l`
pspagedevice=`echo "$pspagedevice" | \
 sed -e "$pspagedevicelen!s/\$/\\\\\\/;s/^ /\\\\\\ /"`

case $at in
  0) # Insert last in the Setup, so that we win over other requests.
     address="^%%EndSetup$"
     insert="i"
     ;;
  *) # Insert right after the beginning of the page.  Trying to find a
     # better place than right after %%Page: is an overkill.
     address="^%%Page: .*$at$"
     insert="a"
     ;;
esac

# The script is complicated by the fact that we don't want to issue
# twice the setpagedevice.  And it is correct for an included document
# to have a Setup section.
cat >$sedscript <<EOF
:start
/$address/{
  $insert\\
$pspagedevice
  b end
}
n
b start
:end
n
b end
EOF

# OK, I'm tired of preparing plenty and doing nothing. Let's go now!
# Insert the page device definition right after the Prolog.
test "$output" != '-' && exec >$output
$fixps `$quiet -q` $file | sed -f $sedscript

# We don't remove the tmp directory: trap does it.
exit 0
