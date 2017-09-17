/* title.c - fprintf that underlines
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

/* Get prototypes for the functions defined here.  */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include <stdarg.h>
#define VA_START(args, lastarg) va_start(args, lastarg)

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
title (FILE * stream, char c, int center_p, const char *format, ...)
{
  int len;
  int padding;
  va_list args;

  VA_START (args, format);

  len = vprintflen (format, args);

  va_end (args);

  if (format [strlen (format) - 1] == '\n')
    len --;
  if (center_p)
    for (padding = 0 ; padding < 79 - len ; padding += 2)
      putc (' ', stream);

  VA_START (args, format);

  vfprintf (stream, format, args);
  va_end (args);

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
