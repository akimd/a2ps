/* xstrrpl.h - replacement of substrings
   Copyright 1988-2017 Free Software Foundation, Inc.

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

/* Author: Akim Demaille <demaille@inf.enst.fr> */

#ifndef XSTRRPL_H_
# define XSTRRPL_H_ 1

char * xstrrpl (const char * string, const char * subst[][2]);
void strrpl (char ** string, const char * subst[][2]);

# if defined (__STDC__) && __STDC__
extern char * xvstrrpl (const char * string, ...);
extern void vstrrpl (char ** string, ...);
# else
char * xvstrrpl ();
void vstrrpl ();
# endif

#endif /* !XSTRRPL_H_ */
