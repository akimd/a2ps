/* Copyright (C) 1993, 1995, 1996 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This is almost copied from strncpy.c, written by Torbjorn Granlund.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>


/* Copy no more than N characters of SRC to DEST, returning the address of
   the last character written into DEST.  */
char *
stpncpy (dest, src, n)
     char *dest;
     const char *src;
     size_t n;
{
  char c;
  char *s = dest;

  --dest;

  if (n >= 4)
    {
      size_t n4 = n >> 2;

      for (;;)
	{
	  c = *src++;
	  *++dest = c;
	  if (c == '\0')
	    break;
	  c = *src++;
	  *++dest = c;
	  if (c == '\0')
	    break;
	  c = *src++;
	  *++dest = c;
	  if (c == '\0')
	    break;
	  c = *src++;
	  *++dest = c;
	  if (c == '\0')
	    break;
	  if (--n4 == 0)
	    goto last_chars;
	}
      n = n - (dest - s) - 1;
      if (n == 0)
	return dest + 1;
      goto zero_fill;
    }

 last_chars:
  n &= 3;
  if (n == 0)
    return dest + 1;

  do
    {
      c = *src++;
      *++dest = c;
      if (--n == 0)
	return dest + 1;
    }
  while (c != '\0');

 zero_fill:
  while (n-- > 0)
    dest[n] = '\0';

  return dest;
}
