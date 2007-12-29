/* title.h -- fprintf that underlines
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

/* Get prototypes for the functions defined here.  */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#if HAVE_VPRINTF || HAVE_DOPRNT || _LIBC
# if __STDC__
#  include <stdarg.h>
#  define VA_START(args, lastarg) va_start(args, lastarg)
# else
#  include <varargs.h>
#  define VA_START(args, lastarg) va_start(args)
# endif
#else
# define va_alist a1, a2, a3, a4, a5, a6, a7, a8
# define va_dcl char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
#endif

# ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__
#   define __attribute__(Spec) /* empty */
#  endif
/* The __-protected variants of `format' and `printf' attributes
   are accepted by gcc versions 2.6.4 (effectively 2.7) and later.  */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#   define __format__ format
#   define __printf__ printf
#  endif
# endif

#include "title.h"
#include "printlen.h"

/* Print the message FORMAT, which is a printf-style
   format string*/

void
#if defined(VA_START) && __STDC__
title (FILE * stream, char c, int center_p, const char *format, ...)
#else
title (stream, c, center_p, format, va_alist)
     FILE * stream;
     char c;
     int center_p;
     char *format;
     va_dcl
#endif
{
  int len;
  int padding;
#ifdef VA_START
  va_list args;
#endif

#ifdef VA_START
  VA_START (args, format);

  len = vprintflen (format, args);

  va_end (args);

  if (format [strlen (format) - 1] == '\n')
    len --;
  if (center_p)
    for (padding = 0 ; padding < 79 - len ; padding += 2)
      putc (' ', stream);

  VA_START (args, format);

# if HAVE_VPRINTF || _LIBC
  vfprintf (stream, format, args);
# else
  _doprnt (format, args, stream);
# endif
  va_end (args);
#else
  fprintf (stream, format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
  /* We suppose that \n can only be met at the end of format, not
   * of one of its arguments */
  if (format [strlen (format) - 1] != '\n')
    putc ('\n', stream);

  /* Draw the line */
  if (center_p)
    for (padding = 0 ; padding < 79 - len ; padding += 2)
      putc (' ', stream);
  for (/* nothing */ ; len ; len --)
    putc (c, stream);
  putc ('\n', stream);

  fflush (stream);
}
