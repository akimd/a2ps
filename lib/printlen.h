/* printlen.h - return number of chars used by a printf like call
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _PRINTLEN_H_
#define _PRINTLEN_H_

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Support of prototyping when possible */
#ifndef PARAMS
#  if PROTOTYPES
#    define PARAMS(protos) protos
#  else /* no PROTOTYPES */
#    define PARAMS(protos) ()
#  endif /* no PROTOTYPES */
#endif

/*
 * Return length of the string sprintf would produce. Always > 0
 */
/*
 * Varaible arguments
 */
# if defined (__STDC__) && __STDC__
extern int printflen (const char *format, ...)
     __attribute__ ((__format__ (__printf__, 1, 2)));
# else
int printflen ();
# endif

/*
 * List of arguments.
 */
int vprintflen PARAMS ((const char *format, va_list args));


#endif /* ! defined(_PRINTLEN_H_) */
