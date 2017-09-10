/* title.h - fprintf that underlines
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

/*
 * Note:
 * very strong inspiration was taken in error.[ch] by
 * David MacKenzie <djm@gnu.ai.mit.edu>
 */

#ifndef _TITLE_H_
#define _TITLE_H_

# if defined (__STDC__) && __STDC__

/* Print the title TITLE, which is a printf-style
   format string with optional args if msg_level is bigger than
   title_verbosity.*/

extern void title (FILE * stream, char c, int center_p,
		   const char *format, ...)
     __attribute__ ((__format__ (__printf__, 4, 5)));

# else
void title ();
# endif

#endif /* not TITLE_H_ */
