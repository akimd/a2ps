/* msg.h -- declaration for verbosity sensitive feedback function
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


#if HAVE_CONFIG_H
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

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
/* Jim Meyering writes:

   "... Some ctype macros are valid only for character codes that
   isascii says are ASCII (SGI's IRIX-4.0.5 is one such system --when
   using /bin/cc or gcc but without giving an ansi option).  So, all
   ctype uses should be through macros like ISPRINT...  If
   STDC_HEADERS is defined, then autoconf has verified that the ctype
   macros don't need to be guarded with references to isascii. ...
   Defining isascii to 1 should let any compiler worth its salt
   eliminate the && through constant folding."  */

#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
#define ISASCII(c) 1
#else
#define ISASCII(c) isascii((int) c)
#endif
#define ISDIGIT(c) (ISASCII (c) && isdigit   ((int) c))

#if defined STDC_HEADERS || defined _LIBC || defined HAVE_STDLIB_H
# include <stdlib.h>
#endif

/* We want strtok. */

#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# include <strings.h>
char *memchr ();
#endif

#include "message.h"
#include "argmatch.h"
#include "getnum.h"

/* The bigger, the more verbose.  Should be set by application */

unsigned int msg_verbosity = 0;


/* Decoce the ARG as a value for verbosity level, return the value.
   Use OPTION as the context name when raise an error.  */

#define verbosity_sep ",:;+"


/* Include the definition of the arguments/values to set the verbosity
   level. */

#include "msg.c"

int
msg_verbosity_argmatch (const char *option, char *arg)
{
  int res = 0;

  ARGMATCH_ASSERT (_msg_verbosity_args, _msg_verbosity_types);

  if (ISDIGIT (*arg))
    {
      /* The verbosity is set through an integer value */
      res = get_integer_in_range (option, arg, 0, 0, range_min);
    }
  else
    {
      /* Keywords are used to define verbosity level */
      char *token = strtok (arg, verbosity_sep);
      do
	{
	  res |= XARGCASEMATCH (option, token,
				_msg_verbosity_args, _msg_verbosity_types);
	}
      while ((token = strtok (NULL, verbosity_sep)));
    }

  return res;
}
