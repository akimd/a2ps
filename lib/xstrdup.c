/* xstrdup.c -- copy a string with out of memory checking
   Copyright (C) 1990, 1996, 1998 Free Software Foundation, Inc.

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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef PARAMS
# if defined PROTOTYPES || (defined __STDC__ && __STDC__)
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

#if STDC_HEADERS || HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <sys/types.h>

char *xmalloc PARAMS ((size_t n));

/* Return a newly allocated copy of STRING.  */

char *
xstrdup (const char *string)
{
  return strcpy (xmalloc (strlen (string) + 1), string);
}

/* Return a newly allocated copy of STRING copying at most N
   characters.  The string is always terminated, and thus is at most
   N + 1 chars long.  */

char *
xstrndup (const char *string, size_t n)
{
  size_t len = strlen (string);
  char *res;
  if (n < len)
    len = n;

  res = xmalloc (len + 1);
  strncpy (res, string, len);
  res[len] = '\0';
  return res;
}
