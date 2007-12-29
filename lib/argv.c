/* Create and destroy argument vectors (argv's)
   Copyright (C) 1992 Free Software Foundation, Inc.
   Written by Fred Fish @ Cygnus Support

   Modified by Akim Demaille so that it can compile out of liberty

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


/*  Create and destroy argument vectors.  An argument vector is simply an
    array of string pointers, terminated by a NULL pointer. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
#  define HAVE_ALLOCA 1
# else
#  if defined HAVE_ALLOCA_H || defined _LIBC
#   include <alloca.h>
#  else
#   ifdef _AIX
 #pragma alloca
#   else
#    ifndef alloca
char *alloca ();
#    endif
#   endif
#  endif
# endif
#endif

#if defined STDC_HEADERS || defined _LIBC
# include <stdlib.h>
#else
# ifdef HAVE_MALLOC_H
#  include <malloc.h>
# else
extern void free ();
extern void * calloc ();
extern void * malloc ();
extern void * realloc ();
# endif
#endif

#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# include <strings.h>
char *memchr ();
#endif


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

#define ISSPACE(c) (ISASCII (c) && isspace   ((int) c))

#include "argv.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8	/* Number of args + NULL in initial argv */


/* dupargv -- duplicate an argument vector */

char **
dupargv (argv)
     char **argv;
{
  int argc;
  char **copy;
  
  if (argv == NULL)
    return NULL;
  
  /* the vector */
  for (argc = 0; argv[argc] != NULL; argc++);
  copy = (char **) malloc ((argc + 1) * sizeof (char *));
  if (copy == NULL)
    return NULL;
  
  /* the strings */
  for (argc = 0; argv[argc] != NULL; argc++)
    {
      int len = strlen (argv[argc]);
      copy[argc] = malloc (sizeof (char *) * (len + 1));
      if (copy[argc] == NULL)
	{
	  freeargv (copy);
	  return NULL;
	}
      strcpy (copy[argc], argv[argc]);
    }
  copy[argc] = NULL;
  return copy;
}

/* freeargv -- free an argument vector */

void freeargv (vector)
char **vector;
{
  register char **scan;

  if (vector != NULL)
    {
      for (scan = vector; *scan != NULL; scan++)
	{
	  free (*scan);
	}
      free (vector);
    }
}

void freeargv_from (vector, from)
char **vector;
int from;
{
  register char **scan;

  if (vector != NULL)
    {
      for (scan = vector ; (from > 0) && (*scan != NULL) ; from--)
 	scan ++;
      
      for (/* nothing */ ; *scan != NULL; scan++)
	free (*scan);
      free (vector);
    }
}

/* buildargv -- build an argument vector from a string */

char **buildargv_argc (input, argc)
  const char *input;
  int * argc;
{
  char *arg;
  char *copybuf;
  int squote = 0;
  int dquote = 0;
  int bsquote = 0;
  int maxargc = 0;
  char **argv = NULL;
  char **nargv;

  if (input != NULL)
    {
      copybuf = alloca (strlen (input) + 1);
      /* Is a do{}while to always execute the loop once.  Always return an
	 argv, even for null strings.  See NOTES above, test case below. */
      do
	{
	  /* Pick off argv[*argc] */
	  while (ISSPACE (*input))
            input++;
	  if ((maxargc == 0) || (*argc >= (maxargc - 1)))
	    {
	      /* argv needs initialization, or expansion */
	      if (argv == NULL)
		{
		  maxargc = *argc > INITIAL_MAXARGC ? *argc : INITIAL_MAXARGC;
		  nargv = (char **) malloc (maxargc * sizeof (char *));
		}
	      else
		{
		  maxargc *= 2;
		  nargv = (char **) realloc (argv, maxargc * sizeof (char *));
		}
	      if (nargv == NULL)
		{
		  if (argv != NULL)
		    {
		      freeargv (argv);
		      argv = NULL;
		    }
		  break;
		}
	      argv = nargv;
	      argv[*argc] = NULL;
	    }
	  /* Begin scanning arg */
	  arg = copybuf;
	  while (*input != EOS)
	    {
	      if (ISSPACE (*input) && !squote && !dquote && !bsquote)
		break;
	      else
		{
		  if (bsquote)
		    {
		      bsquote = 0;
		      *arg++ = *input;
		    }
		  else if (*input == '\\')
		    {
		      bsquote = 1;
		    }
		  else if (squote)
		    {
		      if (*input == '\'')
			squote = 0;
		      else
			*arg++ = *input;
		    }
		  else if (dquote)
		    {
		      if (*input == '"')
			dquote = 0;
		      else
			*arg++ = *input;
		    }
		  else
		    {
		      if (*input == '\'')
			{
			  squote = 1;
			}
		      else if (*input == '"')
			{
			  dquote = 1;
			}
		      else
			{
			  *arg++ = *input;
			}
		    }
		  input++;
		}
	    }
	  *arg = EOS;
	  argv[*argc] = strdup (copybuf);
	  if (argv[*argc] == NULL)
	    {
	      freeargv (argv);
	      argv = NULL;
	      break;
	    }
	  (*argc)++;
	  argv[*argc] = NULL;

	  while (ISSPACE (*input))
	    input++;
	}
      while (*input != EOS);
    }
  return (argv);
}

char **
buildargv (const char * string)
{
  int argc = 0;
  return buildargv_argc (string, &argc);
}

#ifdef MAIN

/* Simple little test driver. */

static char *tests[] =
{
  "a simple command line",
  "arg 'foo' is single quoted",
  "arg \"bar\" is double quoted",
  "arg \"foo bar\" has embedded whitespace",
  "arg 'Jack said \\'hi\\'' has single quotes",
  "arg 'Jack said \\\"hi\\\"' has double quotes",
  "a b c d e f g h i j k l m n o p q r s t u v w x y z 1 2 3 4 5 6 7 8 9",
  
  /* This should be expanded into only one argument.  */
  "trailing-whitespace ",

  "",
  NULL
};

main ()
{
  char **argv;
  char **test;
  char **targs;

  for (test = tests; *test != NULL; test++)
    {
      printf ("buildargv(\"%s\")\n", *test);
      if ((argv = buildargv (*test)) == NULL)
	printf ("failed!\n\n");
      else
	{
	  for (targs = argv; *targs != NULL; targs++)
	    printf ("\t\"%s\"\n", *targs);
	  printf ("\n");
	}
      freeargv (argv);
    }

}

#endif	/* MAIN */
