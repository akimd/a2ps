/*  Copyright (C) 1995 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _GETSHLINE_H_
# define _GETSHLINE_H_ 1

/* Support of prototyping when possible */
# ifndef PARAMS
#   if PROTOTYPES
#     define PARAMS(protos) protos
#   else /* no PROTOTYPES */
#     define PARAMS(protos) ()
#   endif /* no PROTOTYPES */
# endif

/* These routines are devoted to reading meaning full lines.  By
 * meaningful is ment, not empty, and not behind a COMMENTOR symbol.
 * These lines are ended by a DELIMITER, but may be continued if
 * DELIMITER is preceded by a DELIMITER_QUOTE.
 */

int
getshline PARAMS ((char **_lineptr, size_t *_n, FILE *_stream));

int
getshdelim PARAMS ((char **_lineptr, size_t *_n,
		    int _delimiter, int _delimiter_quote, int _commentor,
		    FILE *_stream));


/* These routines are the pending of the above routines, keeping
 * track of the meaning full line interval read.  When calling them
 * *LASTLINE should be the number of the last line read (hence 0
 * for the first line), upon return, *FIRSTLINE is the line number
 * where begun the line returned, and *LASTLINE, the line number
 * where it ended
 */

/* A basic example is available at the bottom of getshline.c */
int
getshline_numbered PARAMS ((int *_firstline, int *_lastline,
			    char **_lineptr, size_t *_n,
			    FILE *_stream));

int
getshdelim_numbered PARAMS ((int *_firstline, int *_lastline,
			     char **_lineptr, size_t *_n,
			     int _delimiter, int _delimiter_quote,
			     int _commentor, FILE *_stream));

#endif /* not defined(_GETSHLINE_H_) */
