/* title.h -- declaration for verbosity sensitive feedback function
   Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
   Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana

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
