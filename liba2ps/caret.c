/* caret.c - how non printable chars are represented
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

#include <config.h>

#include "a2ps.h"
#include "jobs.h"
#include "routines.h"

/*
 * Return a string describing the current unprintable format
 */
const char *
unprintable_format_to_string (enum unprintable_format format)
{
  switch (format) {
  case space:
    return _("space (i.e., ` ')");
  case octal:
    return _("octal (i.e., `\\001' etc.)");
  case hexa:
    return _("hexadecimal (i.e., `\\x0a' etc.)");
  case caret:
    return _("caret (i.e., `^C', `M-^C' etc.)");
  case Emacs:
    return _("emacs (i.e., `C-c', `M-C-c' etc.)");
  case question_mark:
    return _("question-mark (i.e., `?')");
  }
  error (1, 0, 
	 "switch of unprintable_format_to_string");
  return NULL; /* For lint */
}

/*
 * Escape a char, considering it is unprintable
 * (Note: it is an error if the char is printable.
 *        result is unpredictable)
 * Return the num of chars used
 */
int
escape_unprintable (a2ps_job * job, int c,
		    unsigned char * res)
{
  int len = 0;

  switch (job->unprintable_format) {
  case octal:
    sprintf ((char *) res, "\\\\%03o", c);
    return 4;

  case hexa:
    sprintf ((char *) res, "\\\\x%02x", c);
    return 4;

  case question_mark:
    USTRCCAT (res, '?');
    return 1;

  case space:
    USTRCCAT (res, ' ');
    return 1;
    
  case caret:
    if (0177 < c) {
      ustrcat (res, "M-");
      len += 2; 
      c &= 0177;
    }
    
    if (c < ' ') {
      USTRCCAT(res, '^');
      USTRCCAT(res, c);
      len += 2; 
    } else if (c == 0177) {
      ustrcat(res, "^?");
      len += 2;
    } else {
      USTRCCAT(res, c);
      len++;
    }
    return len;

  case Emacs:
    if (0177 < c) {
      ustrcat(res, "M-");
      len += 2; 
      c &= 0177;
    }
    
    if (c < ' ') {
      ustrcat (res, "C-");
      USTRCCAT(res, c);
      len += 3; 
    } else if (c == 0177) {
      ustrcat(res, "C-?");
      len += 3;
    } else {
      USTRCCAT(res, c);
      len++;
    }
    return len;
  }
  return 0; /* For -Wall */
}
