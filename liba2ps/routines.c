/* routines.c - general use routines
   Copyright 1995-2017 Free Software Foundation, Inc.

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
#include "routines.h"
#include "message.h"
#include "quotearg.h"


/*
 * Convert a list of string of valid chars to an yes/no array
 */
void
string_to_array (unsigned char arr[256], const unsigned char * string)
{
  int c;

  for (c = 0 ; c < 256 ; c++)
    arr [c] = false;
  for ( /* nothing */ ; *string ; string ++)
    arr [*string] = true;
}

/*
 * Concatenation of a char. No malloc is done.
 */
void
ustrccat (unsigned char * string, unsigned char c)
{
  int len = strlen((char *)string);
  *(string+len) = c;
  *(string+len+1) = '\0';
}

/*
 * return true iff there are no upper case chars
 */
int
is_strlower (const unsigned char * string)
{
  for (/* skip */; *string != '\0'; string++)
    if (isupper(*string))
      return false;
  return true;
}

/* Copy the LEN first characters of SRC into DST in lower case.
   DST[LEN] is set to \0.  */

static inline unsigned char *
_strncpylc (unsigned char *dst, const unsigned char *src, size_t len)
{
  size_t i;
  for (i = 0 ; i < len ; i++)
    dst[i] = tolower (src[i]);
  dst[len] = '\0';
  return dst;
}

unsigned char *
strnlower (unsigned char *string, size_t len)
{
  return _strncpylc (string, string, len);
}

unsigned char *
strlower (unsigned char *string)
{
  return _strncpylc (string, string, strlen (string));
}

unsigned char *
strcpylc (unsigned char *dst, const unsigned char *src)
{
  return _strncpylc (dst, src, strlen (src));
}

/*
 * Count the number of occurrence of C in S
 */
int
strcnt (unsigned char *s, unsigned char c)
{
  int res;
  for (res = 0 ; *s ; s++)
    if (*s == c)
      res++;
  return res;
}

/*
 * Extract a substring for START, of LENGTH, making sure to
 * set the trailing '\0' (return pos of \0)
 */
char *
strsub (char * dest, const char * string, int start, int length)
{
  char * end = stpncpy (dest, string + start, length);
  *end = '\0';
  return end;
}

/*
 * fopen, but exits on failure
 */
static inline FILE *
_xfopen (const char * filename, const char * rights, const char * format)
{
  FILE * res;

  message (msg_file,
	   (stderr, "%s-fopen (%s)\n", rights, quotearg (filename)));
  res = fopen (filename, rights);
  if (!res)
    error (1, errno, format, quotearg (filename));
  return res;
}

FILE *
xfopen (const char * filename, const char * rights, const char * format)
{
  return _xfopen (filename, rights, format);
}

FILE *
xrfopen (const char * filename)
{
  return _xfopen (filename, "r", _("cannot open file `%s'"));
}

FILE *
xwfopen (const char * filename)
{
  return _xfopen (filename, "w", _("cannot create file `%s'"));
}



/*
 * Like popen, but exist upon failure
 */
static inline FILE *
_xpopen (const char * filename, const char * rights, const char * format)
{
  FILE * res;

  message (msg_file,
	   (stderr, "%s-popen (%s)\n", rights, filename));
  res = popen (filename, rights);
  if (!res)
    error (1, errno, format, quotearg (filename));
  return res;
}

FILE *
xpopen (const char * filename, const char * rights, const char * format)
{
  return _xpopen (filename, rights, format);
}

FILE *
xrpopen (const char * filename)
{
  return _xpopen (filename, "r", _("cannot open a pipe on `%s'"));
}

FILE *
xwpopen (const char * filename)
{
  return _xpopen (filename, "w", _("cannot open a pipe on `%s'"));
}

/*
 * Copy the content of IN into OUT
 */
static inline void
_streams_copy (FILE * in, FILE * out)
{
  size_t read_length;
  char buf [BUFSIZ];

  while ((read_length = fread (buf, sizeof (char), sizeof (buf), in)))
    fwrite (buf, sizeof (char), read_length, out);
}

void
streams_copy (FILE * in, FILE * out)
{
  _streams_copy (in, out);
}

/*
 * Dump the content of the file FILENAME onto STREAM.
 * Used when honoring a subcontract.
 */
void
stream_dump (FILE * stream, const char * filename)
{
  FILE * fp;

  message (msg_tool | msg_file, (stderr, "Dumping file `%s'\n", filename));

  fp = xrfopen (filename);
  _streams_copy (fp, stream);
  fclose (fp);
}

/*
 * Unlink the file FILENAME.
 */
void
unlink2 (PARAM_UNUSED void * dummy, const char * filename)
{
  message (msg_tool | msg_file, (stderr, "Unlinking file `%s'\n", filename));

  /* Don't complain if you can't unlink.  Who cares of a tmp file? */
  unlink (filename);
}
