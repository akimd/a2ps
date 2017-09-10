/* xfnmatch.h - fnmatch declarations wrapper
   Copyright 1996-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef _xfnmatch_h_
#define _xfnmatch_h_

#include <fnmatch.h>

#ifndef FNM_FILE_NAME
# define FNM_FILE_NAME FNM_PATHNAME
#endif

#if HAVE_LINK
# define MAYBE_FNM_CASEFOLD 0
#else
# define MAYBE_FNM_CASEFOLD FNM_CASEFOLD
#endif

#endif /* not _xfnmatch_h_ */
