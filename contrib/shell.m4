dnl This file inplements common sh idioms.
divert(-1)
changequote([, ])
undefine([shift])

dnl GETOPT(CASE-STATEMENT)
dnl Implement a portable getopt
define([GETOPT],
[# Push a token among the arguments that will be used to notice when
# we ended options/arguments parsing.
arg_sep="$$--$$"
set dummy ${1+"[$]@"} "$arg_sep"
shift
while test "x[$]1" != "x$arg_sep"; do

  # Handle --option=value by splitting apart and putting back on argv.
  case "[$]1" in
    --*=*)
      opt=`echo "[$]1" | sed -e 's/=.*//'`
      val=`echo "[$]1" | sed -e 's/@BKL@^=@BKR@*=//'`
      shift
      set dummy "$opt" "$val" ${1+"[$]@"}
      shift
      ;;

changequote(, )dnl
    -[$option_without_arguments]?*)
changequote([, ])dnl
      [#] Prefix [$]1 with x to avoid running `echo -na' for instance.
      opt=`echo "x[$]1" | sed -e 's/x-\(.\).*/-\1/'`
      rest=`echo "x[$]1" | sed -e 's/x-.\(.*\)/-\1/'`
      shift
      set dummy "$opt" "$rest" ${1+"[$]@"}
      shift
      ;;

    # This case needs to be protected so that the case `-??*' does
    # not split long options without arguments
    --*)
      ;;

    # This is an option with argument.  Split apart and put back on argv.
    -??*)
      opt=`echo "x[$]1" | sed -e 's/x-\(.\).*/-\1/'`
      arg=`echo "x[$]1" | sed -e 's/x-.\(.*\)/\1/'`
      shift
      set dummy "$opt" "$arg" ${1+"[$]@"}
      shift
      ;;
  esac

  [#] Now, handle the options.  [$]1 is the option *only*.  If it has an
  [#] argument, it is now necessarily in [$]2 etc.  Remember to shift
  [#] when fetching an argument.
  case "[$]1" in
[$1]

    --) # What remains are not options.
      shift
      while test "x[$]1" != "x$arg_sep"; do
        set dummy ${1+"[$]@"} "[$]1"
        shift
	shift
      done
      break;;

    -*)
      echo "$program: Unknown or ambiguous option \`[$]1'." >&2
      echo "$program: Try \`--help' for more information." >&2
      exit 1;;
    *) set dummy ${1+"[$]@"} "[$]1"
       shift
       ;;
   esac
   shift
done
# Pop the token
shift
])

dnl A short GPL
define([GPL],
[ifelse([$1],,,[# $1
])
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, you can either send email to this
# program's maintainer or write to: The Free Software Foundation,
# Inc.; 59 Temple Place, Suite 330; Boston, MA 02111-1307, USA.])

divert(0)dnl
