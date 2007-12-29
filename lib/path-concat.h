/* path-concat.c -- concatenate two arbitrary pathnames
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Written by Jim Meyering.  */

#if ! defined PATH_CONCAT_H_
# define PATH_CONCAT_H_

# ifndef PARAMS
#  if defined PROTOTYPES || (defined __STDC__ && __STDC__)
#   define PARAMS(Args) Args
#  else
#   define PARAMS(Args) ()
#  endif
# endif

/* May return NULL if fails to malloc. */
char *
path_concat PARAMS ((const char *dir, const char *base,
		     char **base_in_result));

/* Cannot. */
char *
xpath_concat PARAMS ((const char *dir, const char *base,
		     char **base_in_result));

#endif
