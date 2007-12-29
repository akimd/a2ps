/*
 * read.c --- routines of input with no style sheets
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-2000 Akim Demaille, Miguel Santana
 *
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

#include "main.h"
#include "buffer.h"


/*-------------------------------------------------------.
| Returns a single char (EOF for end-of-file).           |
|                                                        |
| Does the nroff's replacement for underline, bold etc.  |
`-------------------------------------------------------*/

static int
plain_getc (buffer_t * buffer, enum face_e *face)
{
  uchar c;

  if (buffer_is_empty (buffer))
    {
      buffer_get (buffer);

      /* We don't trust liba2ps for the line numbers, because if a2ps
	 skips some lines (e.g., --strip-level, or INVISIBLE), liba2ps
	 will number upon output lines, not imput lines, which is what
	 is expected.  */
      (CURRENT_FILE (job))->lines = buffer->line;

      if (buffer->len == 0)
	return EOF;
    }

  *face = Plain;
  c = buffer->content[(buffer->curr)++];

  /* Check if it is a special nroff'ed sequence */
  if (buffer->content[buffer->curr] == '\b')
    {
      /* We might be dealing with misceleanous nroff'ed pages. */
      const uchar *input = buffer->content + buffer->curr + 1;

      /* This might be a bolding sequence.  The bad news is that some
	 strange systems build the bold sequences with only one
	 rewriting, not the 3 usuals.

	 Super strong `_', seen in Sun's mpeg_rc doc.  */
      if (c	== input[0] &&
	  '\b'	== input[1] &&
	  c	== input[2] &&
	  '\b'	== input[3] &&
	  c	== input[4] &&
	  '\b'	== input[5] &&
	  c	== input[6])
	{
	  *face = Label_strong;
	  buffer->curr += 8;
	}
      else if (c     == input[0] &&
	       '\b'  == input[1] &&
	       c     == input[2] &&
	       '\b'  == input[3] &&
	       c     == input[4])
	{
	  *face = Keyword_strong;
	  buffer->curr += 6;
	}
      else if  (c    == input[0] &&
		'\b' == input[1] &&
		c    == input[2])
	{
	  *face = Keyword_strong;
	  buffer->curr += 4;
	}
      else if  (c == input[0])
	{
	  *face = Keyword_strong;
	  buffer->curr += 2;
	}
      /* If C is `_', then set font to italic and move to next char.
	 */
      else if (c == '_')
	{
	  char c2 = input[0];
	  /* Winner of the cup: mpeg_rc, from Sun, where it tries both
	     to underline, and to boldize.  */
	  if	('\b' == input[1] &&
		 c2   == input[2] &&
		 '\b' == input[3] &&
		 c2   == input[4] &&
		 '\b' == input[5] &&
		 c2   == input[6])
	    {
	      *face = Label_strong;
	      c = c2;
	      buffer->curr += 8;
	    }
	  else
	    {
	      *face = Keyword;
	      c = input[0];
	      buffer->curr += 2;
	    }
	}
      /* Seen in gcc.1: o;\b;+, seen in cccp.1: +;\b;o to have an
	 itemizing symbol.  */
      else if (('o' == c &&
		'+' == input[0])
	       || ('+' == c &&
		   'o' == input[0]))
	{
	  *face = Symbol;
	  buffer->curr += 2;
	  c = 0305; /* \oplus in LaTeX */
	}
      /* Seen in groff.1 : c;\b;O, for copyright */
      else if ('c' == c &&
	       'O' == input[0])
	{
	  *face = Symbol;
	  buffer->curr += 2;
	  c = 0343; /* \copyright. */
	}
      /* Seen in gtroff.1 : +;\b;_, for plus or minus */
      else if ('+' == c &&
	       '_' == input[0])
	{
	  *face = Symbol;
	  buffer->curr += 2;
	  c = 0261;
	}
      /* Seen in geqn.1 : ~;\b>;\b;_ for greater or equal */
      else if ('~'  == c &&
	       '>'  == input[0] &&
	       '\b' == input[1] &&
	       '_'  == input[2])
	{
	  *face = Symbol;
	  buffer->curr += 4;
	  c = 0263;
	}
      /* Less than or equal to. */
      else if ('~'  == c &&
	       '<'  == input[0] &&
	       '\b' == input[1] &&
	       '_'  == input[2])
	{
	  *face = Symbol;
	  buffer->curr += 4;
	  c = 0243;
	}
      /* Underlined: x;\b;_ . Note that we have a conflict here in the
	 case x == '+' (see above).  This choice seems the best.  */
      else if ('_' == input[0])
	{
	  *face = Keyword;
	  buffer->curr += 2;
	}
      /* (Should be last). In some case, headers or footers too big,
	 nroff backslashes so that both chars. are superimposed.  We
	 decided to keep only the first one.  */
      else if (((CURRENT_FILE (job)->lines + 3) % 66 == 0)
	       || ((CURRENT_FILE (job)->lines - 4) % 66 == 0))
	{
	  buffer->curr += 2;
	}
      /* else: treate the backslash as a special characters */
    }
  return c;
}

/*
 * Print a file to postscript (no style)
 */
void
plain_print_postscript (a2ps_job * Job, buffer_t * buffer)
{
  enum face_e face = Plain;
  int c;

  while ((c = plain_getc (buffer, &face)) != EOF)
    ps_print_char (Job, c, face);
}
