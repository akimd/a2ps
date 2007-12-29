/* xstrrpl.c -- replacement of substrings
   Copyright (c) 1988-1993 Miguel Santana
   Copyright (c) 1995-1999 Akim Demaille, Miguel Santana

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
#include <assert.h>
#include "xstrrpl.h"

extern char * stpcpy();

/* Perform subsitutions in string.  Result is malloc'd
   E.g., result = xstrrrpl ("1234", subst) gives result = "112333"
   where subst = { {"1", "11"}, {"3", "333"}, { "4", ""}}
   */
char *
xstrrpl (const char * string, const char * subst[][2])
{
  char * res, * cp;
  int i;

  size_t max = 0;
  for (i = 0 ; subst[i][0] ; i++)
    max = (max > strlen (subst[i][1])) ? max : strlen (subst[i][1]);

  /* Add one to make sure that it is never zero, which might cause malloc
     to return NULL.  */
  res = XMALLOC (char, strlen (string) * (max + 1) + 1);
  cp = res;

  /* Perform the substitutions */
  while (*string)
    {
      for (i = 0 ; subst[i][0] ; i++)
	if (!strncmp (subst[i][0], string, strlen (subst[i][0])))
	  {
	    cp = stpcpy (cp, subst[i][1]);
	    string += strlen (subst[i][0]);
	    goto __next_char;
	  }
      *cp++ = *string++;
    __next_char:
      /* nothing */;
    }

  *cp = '\0';
#if DEBUG
  assert (strlen (string) * (max + 1) < cp - res);
#endif

  res = XREALLOC (res, char, cp - res + 1);

  return res;
}

/* Destructive version */
void
strrpl (char ** string, const char * subst[][2])
{
  char * res;
  res = xstrrpl (*string, subst);
  free (*string);
  *string = res;
}

/* Perform subsitution in string.  String is untouched, result is malloc'd
   E.g., result = xstrrrpl ("1234", "1", "11", "3", "333", "4", "", NULL)
   gives result = "112333"
   */
#if __STDC__
char *
xvstrrpl (const char * string, ...)
#else
char *
xvstrrpl (va_alist)
    va_dcl
#endif
{
#if ! __STDC__
  const char *string;
#endif
  va_list ap;
  const char * subst[100][2];	/* Hope this is enough :) */
  int i = 0;

  /* Copy arguments into `args'. */
#if __STDC__
  va_start (ap, string);
#else
  va_start (ap);
  string = va_arg (ap, char *);
#endif
  for (i = 0 ; (subst[i / 2][i % 2] = va_arg (ap, char *)) ; i++)
    ;
  va_end (ap);
  if (i % 2)
    return NULL;
  return xstrrpl (string, subst);
}

/* Destructive version */
#if __STDC__
void
vstrrpl (char **string, ...)
#else
void
vstrrpl (va_alist)
  va_dcl
#endif
{
#if ! __STDC__
  char **string;
#endif
  va_list ap;
  const char * subst[100][2];	/* Hope this is enough :) */
  int i = 0;

  /* Copy arguments into `args'. */
#if __STDC__
  va_start (ap, string);
#else
  va_start (ap);
  string = va_arg (ap, char **);
#endif
  for (i = 0 ; (subst[i / 2][i % 2] = va_arg (ap, char *)) ; i++)
    ;
  va_end (ap);
  if (i % 2)
    free (*string);
  else
    strrpl (string, subst);
}
