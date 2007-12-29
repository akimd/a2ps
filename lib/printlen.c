/* printlen.c - return number of chars used by a printf like call
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
  Note: not all the cases are implemented, so improve it
  before use it!
*/

#include "system.h"
#include "printlen.h"


unsigned long strtoul ();

static int
int_printflen (const char *format, va_list ap)
{
  const char *cp;
  int total_width = 0;
  int width = 0;

  for (cp = format ; *cp ; cp++)
    {
      if (*cp != '%')
	total_width ++;
      else
	{
	  /* A `% ' is occuring */
	  cp++;
	  width = 0;
	  while (strchr ("-+ #0", *cp))
	    ++cp;
	  if (*cp == '*')
	    {
	      ++cp;
	      width = abs (va_arg (ap, int));
	    }
	  if (*cp == '.')
	    {
	      ++cp;
	      if (*cp == '*')
		{
		  ++cp;
		  width = abs (va_arg (ap, int));
		}
	    }
	  while (strchr ("hlL", *cp))
	    ++cp;
	  /* Currently not enough cases are covered */
	  switch (*cp)
	    {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      (void) va_arg (ap, int);
	      break;
	    case 'f':
	    case 'e':
	    case 'E':
	    case 'g':
	    case 'G':
	      (void) va_arg (ap, double);
	      break;
	    case 's':
	      total_width += strlen (va_arg (ap, char *));
	      break;
	    case 'p':
	    case 'n':
	      (void) va_arg (ap, char *);
	      break;
	    }
	}
    }
  return total_width;
}

int
vprintflen (const char *format,  va_list args)
{
  va_list ap;
  int ret;
  
  va_copy (ap, args);
  
  ret = int_printflen (format, ap);
  
  va_end(ap);
  
  return ret;
}

int
#if defined(VA_START) && __STDC__
printflen (const char *format, ...)
#else
printflen (format, va_alist)
   char * format;
   va_dcl
#endif
{
  va_list args;
  int res;

  VA_START (args, format);

  res = vprintflen (format, args);
  va_end (args);

  return res;
}
