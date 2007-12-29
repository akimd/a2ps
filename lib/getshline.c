/* getshline.c -- read a meaningfull line from a file

Copyright (C) 1993, 1996 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* Written by Jan Brittenson, bson@gnu.ai.mit.edu.  */
/* Written by Akim Demaille, demaille@inf.enst.fr
 * from getline.c by Jan Brittenson, bson@gnu.ai.mit.edu.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <assert.h>

#include "xalloc.h"

#include "getshline.h"

/* Always add at least this many bytes when extending the buffer.  */
#define MIN_CHUNK 64

/* Read up to (and including) a TERMINATOR from STREAM into *LINEPTR
   + OFFSET (and null-terminate it). *LINEPTR is a pointer returned from
   malloc (or NULL), pointing to *N characters of space.  It is realloc'd
   as necessary.  Return the number of characters read (not including the
   null terminator), or -1 on error or EOF.  */

static int
getshstr (int * firstline, int * lastline,
	  char ** lineptr, size_t * n,
	  FILE * stream,
	  char terminator, int terminator_quote, int commentor,
	  size_t offset)
{
  int nchars_avail;		/* Allocated but unused chars in *LINEPTR.  */
  char *read_pos;		/* Where we're reading into *LINEPTR. */
  int ret;

  if (!lineptr || !n || !stream)
    return -1;

  if (!*lineptr)
    {
      *n = MIN_CHUNK;
      *lineptr = XMALLOC (char, *n);
      if (!*lineptr)
	return -1;
    }

  /* We are reading a new line */
  *firstline = ++(*lastline);
  nchars_avail = *n - offset;
  read_pos = *lineptr + offset;

  for (;;)
    {
      register int c = getc (stream);

      /* We always want at least one char left in the buffer, since we
	 always (unless we get an error while reading the first char)
	 NUL-terminate the line buffer.  */
      assert(*n + *lineptr == read_pos + nchars_avail);
      if (nchars_avail < 2)
	{
	  if (*n > MIN_CHUNK)
	    *n *= 2;
	  else
	    *n += MIN_CHUNK;

	  nchars_avail = *n + *lineptr - read_pos;
	  *lineptr = XREALLOC (*lineptr, char, *n);
	  if (!*lineptr)
	    return -1;
	  read_pos = *n - nchars_avail + *lineptr;
	  assert(*n + *lineptr == read_pos + nchars_avail);
	}

      if (c == EOF || ferror (stream))
	{
	  /* Return partial line, if any.  */
	  if (read_pos == *lineptr)
	    return -1;
	  else
	    break;
	}

      *read_pos++ = c;
      nchars_avail--;

      if (c == terminator)
	{
	  /* The comment lines can not be continued by an
	   * escaped eol, so test this before quotation of eol */
	  if (**lineptr == terminator || **lineptr == commentor)
	    {
	      /* We just read a blank line.  Then we are reading
	       * a new first line */
	      *firstline = *lastline + 1;
	      return (getshstr (firstline, lastline, lineptr, n, stream,
				terminator, terminator_quote, commentor,
				offset));
	    }
	  if ((read_pos - 2 <= *lineptr)
	      || (*(read_pos - 2) != terminator_quote))
	    /* Return the line.  */
	    break;
	  /* The line is continued because the eol was quoted,
	   * hence lastline has to be incremented */
	  read_pos -= 2;
	  nchars_avail += 2;
	  (*lastline)++;
	}
    }

  /* Done - NUL terminate and return the number of chars read.  */
  *read_pos = '\0';

  ret = read_pos - (*lineptr + offset);
  return ret;
}

int
getshline_numbered (int * firstline, int * lastline,
		    char ** lineptr, size_t * n, FILE * stream)
{
  return getshstr (firstline, lastline, lineptr, n, stream,
		   '\n', '\\', '#', 0);
}

int
getshline (char ** lineptr, size_t * n, FILE * stream)
{
  int firstline, lastline;
  return getshstr (&firstline, &lastline, lineptr, n, stream,
		   '\n', '\\', '#', 0);
}


int
getshdelim_numbered (int * firstline, int * lastline,
		     char ** lineptr, size_t * n,
		     int delimiter, int delimiter_quote, int commentor,
		     FILE * stream)
{
  return getshstr (firstline, lastline, lineptr, n, stream,
		   delimiter, delimiter_quote, commentor, 0);
}

int
getshdelim (char ** lineptr, size_t * n,
	    int delimiter, int delimiter_quote, int commentor,
	    FILE * stream)
{
  int firstline, lastline;
  return getshstr (&firstline, &lastline, lineptr, n, stream,
		   delimiter, delimiter_quote, commentor, 0);
}

#ifdef TEST

const char * program_name = "getsh";

int
main(int argc, char *argv[])
{
  int firstline = 0, lastline = 0;
  char * line_content = NULL;
  int line_size = 0;
  FILE * file;
  int res;

  if (argc < 2)
    exit (1);
  file = fopen (argv[1], "r");
  if (!file)
    exit (1);

  while ((res = getshline_numbered (&firstline, &lastline,
				    &line_content, &line_size, file)) != -1)
      printf ("%3d-%3d:%s", firstline, lastline, line_content);


  free (line_content);
  return 0;
}
#endif
