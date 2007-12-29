/* dstring.c - The dynamic string handling routines used by cpio.
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

/* Author: Akim Demaille <demaille@inf.enst.fr> */

#include "system.h"
#include "dstring.h"
#include "printlen.h"

#define DS_MARGIN	1024

int ds_exit_error = EXIT_FAILURE;	/* exit value when encounters	*
					 * an error 			*/


/* Initialiaze dynamic string STRING with space for SIZE characters.  */

struct dstring *
ds_new (size_t size,
	enum ds_growth growth, size_t increment)
{
  struct dstring * res;

  if (size ==  0)
    error (ds_exit_error, 0, "invalid size for dynamic string: %d",
	   size);

  if (increment == 0 && growth != ds_steady)
    error (ds_exit_error, 0, "invalid increment for dynamic string: %d",
	   increment);

  res = XMALLOC (struct dstring, 1);
  res->len = 0;
  res->size = size;
  res->original_size = size;
  res->growth = growth;
  res->increment = increment;
  res->content = XMALLOC (char, size);
  res->content[0] = '\0';

  return res;
}

/* Delete dynamic string.  */

void
ds_erase (struct dstring *string)
{
  free (string->content);
  free (string);
}

/*
 * Report the load of the string
 */
void
ds_print_stats (struct dstring * str, FILE * stream)
{
  const char * cp = NULL;

  fprintf (stream, _("Dynamic string:\n"));
  fprintf (stream, _("\tload: %d/%d (%2.1f%%)\n"),
	   str->len, str->size, 100.0 * str->len / str->size);
  switch (str->growth) {
  case ds_steady:
    cp = "[const]";
    break;
  case ds_linear:
    cp = "+=";
    break;
  case ds_geometrical:
    cp = "*=";
    break;
  default:
    error (ds_exit_error, 0, "invalid growth type for dstring");
  }
  fprintf (stream, _("\toriginal size: %d, growth: %s %d\n"),
	   str->original_size, cp, str->increment);
}

/*
 * Expand dynamic string STRING, if necessary, to hold SIZE characters.
 */
void
ds_resize (struct dstring *string, size_t size)
{
  if (string->len + 1 < size)
    {
      string->size = size;
      string->content = XREALLOC (string->content, char, size);
    }
}

/*
 * Automatic growth
 */
void
ds_grow (struct dstring *string)
{
  switch (string->growth) {
  case ds_steady:
    return;

  case ds_linear:
    string->size += string->increment;
    break;

  case ds_geometrical:
    string->size *= string->increment;
    break;
  }
  string->content = XREALLOC (string->content, char, string->size);
}

/****************************************************************/
/*		Testing						*/
/****************************************************************/
/*
 * Guess what :)
 */
int
ds_is_full (struct dstring *str)
{
  return (str->len + 1 >= str->size);
}

/****************************************************************/
/*		Usual string manipulations			*/
/****************************************************************/
/*
 * Concatenate strings to a dstring
 */
void
ds_strcat (struct dstring *s, char *t)
{
  size_t len = s->len;

  s->len += strlen (t);
  if (ds_is_full (s))
    ds_grow (s);

  strcpy (s->content + len, t);
}

void
ds_strncat (struct dstring *s, char *t, int n)
{
  size_t len = s->len;

  s->len += n;
  if (ds_is_full (s))
    ds_grow (s);

  strncpy (s->content + len, t, n);
  s->content[s->len] = '\0';
}

/*
 * Concatenate chars to a dstring
 */
void
ds_strccat (struct dstring *s, char c)
{
  if (s->len + 2 >= s->size)
    ds_grow (s);

  s->content [s->len++] = c;
  s->content [s->len] = '\0';
}

/****************************************************************/
/*		Safe sprintf variations				*/
/****************************************************************/
/*
 * sprintf into the dstring, resizing as necessary
 */
void
ds_vsprintf (struct dstring * ds, const char *format, va_list args)
{
  int len;

  len = vprintflen (format, args);
  ds_resize (ds, len);

  vsprintf (ds->content, format, args);
  ds->len = strlen (ds->content);
}

/*
 * Like sprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
#if defined(VA_START) && __STDC__
ds_sprintf (struct dstring * ds, const char *format, ...)
#else
ds_sprintf (ds, format, va_alist)
  struct dstring * ds;
  char * format;
  va_dcl
#endif
{
#ifdef VA_START
  va_list args;

  VA_START (args, format);
  ds_vsprintf (ds,
	       format, args);
  va_end (args);
#else
  ds_vsprintf (ds,
	       format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

/*
 * Like cat_vsprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
ds_cat_vsprintf (struct dstring * ds, const char *format, va_list args)
{
  int len;
  len = ds->len + vprintflen (format, args);

  ds_resize (ds, len);

  vsprintf (ds->content + ds->len, format, args);
  ds->len += strlen (ds->content + ds->len);
}

/*
 * Like cat_sprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
#if defined(VA_START) && __STDC__
ds_cat_sprintf (struct dstring * ds, const char *format, ...)
#else
ds_cat_sprintf (ds, format, va_alist)
  struct dstring * ds;
  char * format;
  va_dcl
#endif
{
#ifdef VA_START
  va_list args;

  VA_START (args, format);

  ds_cat_vsprintf (ds,
		   format, args);
  va_end (args);
#else
  ds_cat_vsprintf (ds,
		   format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

/****************************************************************/
/*		Unsafe sprintf variations			*/
/****************************************************************/
/*
 * Like ds_vsprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
ds_unsafe_vsprintf (struct dstring * ds, const char *format, va_list args)
{
  vsprintf (ds->content, format, args);
  ds->len = strlen (ds->content);
}

/*
 * Like sprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
#if defined(VA_START) && __STDC__
ds_unsafe_sprintf (struct dstring * ds, const char *format, ...)
#else
ds_unsafe_sprintf (ds, format, va_alist)
  struct dstring * ds;
  char * format;
  va_dcl
#endif
{
#ifdef VA_START
  va_list args;

  VA_START (args, format);
  ds_unsafe_vsprintf (ds,
		      format, args);
  va_end (args);
#else
  ds_unsafe_vsprintf (ds,
		      format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

/*
 * Like ds_cat_vsprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
ds_unsafe_cat_vsprintf (struct dstring * ds, const char *format, va_list args)
{
  if (ds->size < ds->len + DS_MARGIN)
    ds_grow (ds);

  vsprintf (ds->content + ds->len, format, args);
  ds->len += strlen (ds->content + ds->len);
}

/*
 * Like cat_sprintf, but not very carrefull
 * (sprinting far too big string may SEGV)
 */
void
#if defined(VA_START) && __STDC__
ds_unsafe_cat_sprintf (struct dstring * ds, const char *format, ...)
#else
ds_unsafe_cat_sprintf (ds, format, va_alist)
  struct dstring * ds;
  char * format;
  va_dcl
#endif
{
#ifdef VA_START
  va_list args;

  VA_START (args, format);
  ds_unsafe_cat_vsprintf (ds,
			  format, args);
  va_end (args);
#else
  ds_unsafe_cat_vsprintf (ds,
			  format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

/****************************************************************/
/*		Dealing with files				*/
/****************************************************************/
/* Dynamic string S gets a string terminated by the EOS character
   (which is removed) from file F.  S will increase
   in size during the function if the string from F is longer than
   the current size of S.
   Return NULL if end of file is detected.  Otherwise,
   Return a pointer to the null-terminated string in S.  */

char *
ds_getdelim (struct dstring *s, char eos, FILE *f)
{
  int insize;			/* Amount needed for line.  */
  int strsize;			/* Amount allocated for S.  */
  int next_ch;

  /* Initialize.  */
  insize = 0;
  strsize = s->len;

  /* Read the input string.  */
  next_ch = getc (f);
  while (next_ch != eos && next_ch != EOF)
    {
      if (insize >= strsize - 1)
	{
	  ds_grow (s);
	  strsize = s->len;
	}
      s->content[insize++] = next_ch;
      next_ch = getc (f);
    }
  s->content[insize++] = '\0';

  if (insize == 1 && next_ch == EOF)
    return NULL;
  else
    return s->content;
}

char *
ds_getline (struct dstring *s, FILE *f)
{
  return ds_getdelim (s, '\n', f);
}
