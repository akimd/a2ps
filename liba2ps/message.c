/* message.c - declaration for verbosity sensitive feedback function
   Copyright 1998-2017 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/* Jim Meyering writes:

   "... Some ctype macros are valid only for character codes that
   isascii says are ASCII (SGI's IRIX-4.0.5 is one such system --when
   using /bin/cc or gcc but without giving an ansi option).  So, all
   ctype uses should be through macros like ISPRINT...  If
   STDC_HEADERS is defined, then autoconf has verified that the ctype
   macros don't need to be guarded with references to isascii. ...
   Defining isascii to 1 should let any compiler worth its salt
   eliminate the && through constant folding."  */

#define ISASCII(c) isascii((int) c)
#define ISDIGIT(c) (ISASCII (c) && isdigit   ((int) c))


/* We want strtok. */
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

  //ARGMATCH_VERIFY (_msg_verbosity_args, _msg_verbosity_types);

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
	  res |= XARGMATCH (option, token,
				_msg_verbosity_args, _msg_verbosity_types);
	}
      while ((token = strtok (NULL, verbosity_sep)));
    }

  return res;
}
