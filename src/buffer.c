/*
 * buffer.c - read line by line with conversion of EOL types
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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
 * $Id: buffer.c,v 1.1.1.1.2.1 2007/12/29 01:58:34 mhatta Exp $
 */

/* FIXME: There is a case where the behavior is probably wrong: mixed
   string/stream buffers.  It may happen that the last char of the
   string part be a '\n' (or '\r') and buffer wants to see the next
   char (to see if is a '\r' (or '\n')), but the next char is in the
   stream, not the string.

   Currently it does not fetch that next char. */

#include "a2ps.h"
#include "routines.h"
#include "argmatch.h"
#include "buffer.h"
#include <assert.h>

/****************************************************************/
/*		 	Handling of the various eol styles	*/
/****************************************************************/
const char *
eol_to_string (enum eol_e eol)
{
  switch (eol)
    {
    case eol_r:
      return "\\r";

    case eol_n:
      return "\\n";

    case eol_rn:
      return "\\r\\n";

    case eol_nr:
      return "\\n\\r";

    case eol_auto:
      /* TRANS: the type of the end-of-line rules, is `any type', i.e.
         lines ended by \n, or \r, or \r\n, or \n\r are OK */
      return _ ("any type");

    default:
      abort ();
    }
  return NULL;			/* -Wall */
}

/*
 * What about the bools
 */
static const char *const eol_args[] =
{
  "r", "mac",
  "n", "unix",
  "nr",
  "rn", "pc",
  "auto", "any", "4x4",
  0
};

static const enum eol_e eol_types[] =
{
  eol_r, eol_r,
  eol_n, eol_n,
  eol_nr,
  eol_rn, eol_rn,
  eol_auto, eol_auto, eol_auto
};


enum eol_e
option_string_to_eol (const char *option,
		      const char *arg)
{
  ARGMATCH_ASSERT (eol_args, eol_types);
  return XARGCASEMATCH (option, arg, eol_args, eol_types);
}

/****************************************************************/
/*		 	buffer_t Service routines 		*/
/****************************************************************/
static inline void
buffer_internal_set (buffer_t * buffer,
		     FILE * stream,
		     const uchar * buf, size_t bufsize,
		     bool pipe_p, enum eol_e eol)
{
  buffer->buf = buf;
  buffer->bufsize = bufsize;
  buffer->bufoffset = 0;
  buffer->stream = stream;
  buffer->pipe_p = pipe_p;

  buffer->content = NULL;
  buffer->eol = eol;
  buffer->lower_case = false;	/* By default, no lower case version. */
  buffer->value = NULL;
  buffer->line = 0;
  buffer->allocsize = 0;
  buffer->len = 0;
  buffer->curr = 0;
  obstack_init (&buffer->obstack);
}

void
buffer_stream_set (buffer_t * buffer, FILE * stream, enum eol_e eol)
{
  buffer_internal_set (buffer, stream, NULL, 0, false, eol);
}

void
buffer_pipe_set (buffer_t * buffer, FILE * stream, enum eol_e eol)
{
  buffer_internal_set (buffer, stream, NULL, 0, true, eol);
}

void
buffer_string_set (buffer_t * buffer, const uchar * string, enum eol_e eol)
{
  buffer_internal_set (buffer, NULL, string, strlen (string), false, eol);
}

void
buffer_buffer_set (buffer_t * buffer, const uchar * buf, size_t bufsize,
		   enum eol_e eol)
{
  buffer_internal_set (buffer, NULL, buf, bufsize, false, eol);
}

/* getc and ungetc on the stream of Buf. */

#define sgetc(Buf) (getc ((Buf)->stream))
#define sungetc(Char, Buf) (ungetc (Char, (Buf)->stream))

/* getc and ungetc on the buffer of Buf.
   It is much more tricky, especially if Buf is mixed buffer/stream.
   Even when the buffer was completely read, the oofset must be increased. */

#define bgetc(Buf)			\
  (((Buf)->bufoffset < (Buf)->bufsize) 	\
   ? ((Buf)->buf[(Buf)->bufoffset++])	\
   : ((Buf)->bufoffset++, EOF))

#define bungetc(Char, Buf) ((Buf)->bufoffset--)

void
buffer_self_print (buffer_t * buffer, FILE * stream)
{
  if (buffer->buf)
    fprintf (stream, "A string buffer.  Bufoffset %u\n",
	     buffer->bufoffset);
  if (buffer->stream)
    fprintf (stream, "A stream buffer (%s).\n",
	     buffer->pipe_p ? "pipe" : "file");
  fprintf (stream, "Len = %d, Lower case = %d, Line = %d\n",
	   buffer->len, buffer->lower_case, buffer->line);
  if (buffer->len)
    fprintf (stream, "Content = `%s'\n", buffer->content);
}

/*
 * It frees the content, not the pointer
 */
void
buffer_release (buffer_t * buffer)
{
  /* VALUE is malloc'd only if BUFFER->LOWER_CASE */
  if (buffer->lower_case)
    free (buffer->value);
  /* I don't know how this one should be used */
  /*  obstack_free (&buffer->obstack, NULL); */
}

void
buffer_set_lower_case (buffer_t * buffer, bool sensitive)
{
  buffer->lower_case = sensitive;
}

/*
 * Get a line from BUFFER->STREAM.
 * Returns true  if a full line has been read,
 *         false if EOF was met before
 */

static inline bool
buffer_stream_get_line (buffer_t * buffer)
{
  register int c, d;

  while ((c = sgetc (buffer)) != EOF)
    switch (c)
      {
      case '\n':
	switch (buffer->eol)
	  {
	  case eol_r:
	  case eol_rn:
	    /* \n plain char */
	    goto stream_plain_char;

	  case eol_auto:
	    /* If the next char is a \r, eat it */
	    if ((d = sgetc (buffer)) != '\r')
	      sungetc (d, buffer);
	    break;

	  case eol_n:
	    /* This is a good eol */
	    break;

	  case eol_nr:
	    if ((d = sgetc (buffer)) != '\r')
	      {
		/* This is \n, but eol is \n\r: make it a plain char */
		sungetc (d, buffer);
		goto stream_plain_char;
	      }
	    /* This is eol=\n\r, just return \n */
	    break;
	  }
	/* End it.  No need to NUL-terminate */
	obstack_1grow (&buffer->obstack, c);
	return true;

      case '\r':
	switch (buffer->eol)
	  {
	  case eol_n:
	  case eol_nr:
	    /* \r plain char */
	    goto stream_plain_char;

	  case eol_r:
	    /* This is a good eol, but the lib uses \n */
	    c = '\n';
	    break;

	  case eol_auto:
	    /* If the next char is a \n, eat it */
	    if ((d = sgetc (buffer)) != '\n')
	      sungetc (d, buffer);
	    c = '\n';
	    break;

	  case eol_rn:
	    if ((d = sgetc (buffer)) != '\n')
	      {
		/* This is \r, but eol is \r\n: make it a plain char */
		sungetc (d, buffer);
		goto stream_plain_char;
	      }
	    /* This is eol = \r\n: just return \n */
	    c = '\n';
	    break;
	  }
	/* End it.  No need to NUL-terminate */
	obstack_1grow (&buffer->obstack, c);
	return true;

      default:
      stream_plain_char:
	obstack_1grow (&buffer->obstack, c);
	break;
      }

  /* If we are here, it's because there is nothing more to read, and
     the last char was not an eol: report the line is not complete. */
  return false;
}

/*
 * Get a line from BUFFER->BUF
 *
 * Note that we could have made it destuctive.
 * But would have caused problem if some day we want to use
 * mmap.
 *
 * Returns true  if a full line has been read,
 *         false if EOF (i.e. offset >= bufsize) was met before
 */

static inline bool
buffer_string_get_line (buffer_t * buffer)
{
  register int c, d;

  while ((c = bgetc (buffer)) != EOF)
    switch (c)
      {
      case '\n':
	switch (buffer->eol)
	  {
	  case eol_r:
	  case eol_rn:
	    /* \n plain char */
	    goto string_plain_char;

	  case eol_auto:
	    /* If the next char is a \r, eat it */
	    if ((d = bgetc (buffer)) != '\r')
	      bungetc (d, buffer);
	    break;

	  case eol_n:
	    /* This is a good eol */
	    break;

	  case eol_nr:
	    if ((d = bgetc (buffer)) != '\r')
	      {
		/* This is \n, but eol is \n\r: make it a plain char */
		bungetc (d, buffer);
		goto string_plain_char;
	      }
	    /* This is eol=\n\r, just return \n */
	    break;
	  }
	/* End it.  No need to NUL-terminate */
	obstack_1grow (&buffer->obstack, c);
	return true;

      case '\r':
	switch (buffer->eol)
	  {
	  case eol_n:
	  case eol_nr:
	    /* \r plain char */
	    goto string_plain_char;

	  case eol_r:
	    /* This is a good eol, but the lib uses \n */
	    c = '\n';
	    break;

	  case eol_auto:
	    /* If the next char is a \n, eat it */
	    if ((d = bgetc (buffer)) != '\n')
	      bungetc (d, buffer);
	    c = '\n';
	    break;

	  case eol_rn:
	    if ((d = bgetc (buffer)) != '\n')
	      {
		/* This is \r, but eol is \r\n: make it a plain char */
		bungetc (d, buffer);
		goto string_plain_char;
	      }
	    /* This is eol = \r\n: just return \n */
	    c = '\n';
	    break;
	  }
	/* End it.  No need to NUL-terminate */
	obstack_1grow (&buffer->obstack, c);
	return true;

      default:
      string_plain_char:
	obstack_1grow (&buffer->obstack, c);
	break;
      }

  /* If we are here, it's because there is nothing more to read, and
     the last char was not an eol: report the line is not complete. */
  return false;
}


/* buffer_get
 *   if the language is case insensitive,
 *   build a lower case version of the buffer
 */
void
buffer_get (buffer_t * buffer)
{
  bool line_ended_p = false;	/* the line read finishes by eol */

  /* If there is something to read from the buffered string, fetch it */
  if (buffer->buf && buffer->bufoffset < buffer->bufsize)
    line_ended_p = buffer_string_get_line (buffer);

  /* If the line was not finished, continue the reading but in the
    stream.  This includes the case where there is no buf */
  if (buffer->stream && !line_ended_p)
    line_ended_p = buffer_stream_get_line (buffer);

  /* A full line has been read.  Close the obstack, get the content.
     We NUL terminate because it helps the parsing functions such as
     match_keyword, which looks one char after the current char.  With
     this sentinel, which is probably not in the alphabet, we save a
     test on the length of the buffer. */
  buffer->len = obstack_object_size (&buffer->obstack);
  obstack_1grow (&buffer->obstack, '\0');
  buffer->content = (uchar *) obstack_finish (&buffer->obstack);

  /* One more line read */
  buffer->line++;

  /* If the eol char is preceded by a \f, then just forget the eol, so
     that there won't be a blank line at the top of the next page */
  if ((buffer->len >= 2)
      && buffer->content[buffer->len - 2] == '\f')
    {
      buffer->content[--(buffer->len)] = '\0';
    }

  if (buffer->lower_case)
    {
      size_t i;
      if (buffer->allocsize <= buffer->len)
	buffer->allocsize = buffer->len + 1;

      buffer->value =
	XREALLOC (buffer->value, uchar, buffer->allocsize);

      for (i = 0; i <= buffer->len; i++)
	buffer->value[i] = tolower (buffer->content[i]);
    }
  else
    {
      buffer->value = buffer->content;
    }

  buffer->curr = 0;
}

/* buffer_sample_get
 *   extract a piece of a BUFFER's stream into FILENAME
 *
 *   The piece that has been extracted is taken as buf in BUFFER, so that
 *   we still can use it.
 */
void
buffer_sample_get (buffer_t * buffer, const char *filename)
{
  /* I think that most file(1) just use the 512 first bytes */
#define SAMPLE_SIZE 512
  FILE *out = xwfopen (filename);
  size_t cur = 0;
  int c;
  char *sample_buffer = XMALLOC (char, SAMPLE_SIZE);

  for (; (cur < SAMPLE_SIZE) && ((c = sgetc (buffer)) != EOF); cur++)
    {
      sample_buffer[cur] = c;
      putc (c, out);
    }

  /* Put the sample into the buffer for later use */
  buffer->buf = sample_buffer;
  buffer->bufsize = cur;

  fclose (out);
}


/* buffer_save
 *   save the content of BUFFER to the file FILENAME
 *   if BUFFER has a buf, dump it
 *   then if it has a stream, dump its content
 *
 *   Note that the buffer is no longer usable: we don't rewind it,
 *   because it can be stdin.
 */
void
buffer_save (buffer_t * buffer, const char *filename)
{
  FILE *out = xwfopen (filename);

  if (buffer->buf)
    {
      size_t cur;
      for (cur = 0; cur < buffer->bufsize; cur++)
	putc (buffer->buf[cur], out);
    }

  if (buffer->stream)
    streams_copy (buffer->stream, out);

  fclose (out);
}

/*
 * Check if sample + dump are OK.  Only for debug purpose
 */
#if BUFFER_TEST
void
buffer_test (const char *filename)
{
  FILE *in = xrfopen (filename);
  buffer_t buffer;

  buffer_stream_set (&buffer, in, eol_n);

  buffer_sample_get (&buffer, "/tmp/sample");
  buffer_save (&buffer, "/tmp/dump");
  buffer_release (&buffer);
}
#endif
