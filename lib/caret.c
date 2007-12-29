/*
 * caret.c
 *
 * How non printable chars are represented
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 */

/*
 * This file is part of a2ps.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * $Id: caret.c,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */
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
		    uchar * res)
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
