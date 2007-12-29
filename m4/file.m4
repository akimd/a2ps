# -*- Autoconf -*-
# Find where is `file' and what is the option for it to follow the
# links. If no way, use a plain `file'.
#
# Copyright (C) 1995-2000, 2002 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
# 02111-1307  USA

# Written by Akim Demaille <akim@freefriends.org>
# serial 5

dnl This is a test to know what is the correct way to get
dnl file following the links
dnl I hope it is robust, but I'm a novice :) Akim Demaille
AC_DEFUN([ad_PROG_FILE_LINK],
  [AC_PATH_PROG(file_prog, file)dnl
   AC_MSG_CHECKING(for the option for file to follow the links)
   AC_CACHE_VAL(ac_cv_prog_file_link_option,[
     touch conftestfile
     ln -s conftestfile conftestfile1
     for file_opt in '' ' -L'
     do
       res=`eval $file_prog $file_opt conftestfile1 2>&-` || res=symbolic
       case $res in
	  *symbolic*) ;;
	  *) ac_cv_prog_file_link_option=$file_opt ; break    ;;
       esac
     done
     rm -f conftestfile conftestfile1
     if test "X$ac_cv_prog_file_link_option" = X; then
        ac_cv_prog_file_link_option="no";
     fi])dnl
   # If no flag were found, default to `file' with no options
   case "X$ac_cv_prog_file_link_option" in
     X)   AC_MSG_RESULT(none required) ;
          FILE_LINK=${file_prog} ;;
     Xno) AC_MSG_RESULT(no way);
          FILE_LINK=${file_prog} ;;
     *)   AC_MSG_RESULT($ac_cv_prog_file_link_option);
          FILE_LINK=${file_prog}${ac_cv_prog_file_link_option} ;;
   esac])
