/* tterm.c -- Minimalist information on the terminal
   Copyright (C) 1998 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#if defined STDC_HEADERS || defined _LIBC || defined HAVE_STDLIB_H
# include <stdlib.h>
#else
char *getenv ();
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#if HAVE_LIMITS_H
/* limits.h must come before pathmax.h because limits.h on some systems
   undefs PATH_MAX, whereas pathmax.h sets PATH_MAX.  */
# include <limits.h>
#endif
#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

/* The extra casts work around common compiler bugs,
   e.g. Cray C 5.0.3.0 when t == time_t.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))
#define TYPE_MINIMUM(t) \
  ((t) (TYPE_SIGNED (t) ? ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1) : (t) 0))
#define TYPE_MAXIMUM(t) ((t) (~ (t) 0 - TYPE_MINIMUM (t)))

#ifndef INT_MAX
# define INT_MAX TYPE_MAXIMUM (int)
#endif

#if HAVE_TERMIOS_H
# include <termios.h>
#endif

#ifdef HAVE_SYS_IOCTL
# include <sys/ioctl.h>
#endif

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#endif

#include "xstrtol.h"
#include "quotearg.h"
#include "error.h"
#include "tterm.h"

struct tterm
{
  /* The number of chars per hardware tab stop.  Setting this to zero
     inhibits the use of TAB characters for separating columns.  -T */
  size_t tabsize;

  /* The line length to use for breaking lines in many-per-line format.
     Can be set with -w.  */
  size_t width;
};

/* Default setting */
static struct tterm tterm_default =
{
  8,	/* tabsize */
  80	/* width */
};

/* Set the line width of TTERM to WIDTH.  Returns the previous value. */

size_t
tterm_width_set (struct tterm * tterm, size_t width)
{
  struct tterm *t = tterm ? tterm : &tterm_default;
  size_t old = t->width;
  t->width = width;
  return old;
}


/* Return the line width of TTERM. */

size_t
tterm_width (struct tterm * tterm)
{
  struct tterm *t = tterm ? tterm : &tterm_default;
  return t->width;
}

/* Set the tab size of TTERM to SIZE.  Returns the previous value. */

size_t
tterm_tabsize_set (struct tterm * tterm, size_t size)
{
  struct tterm *t = tterm ? tterm : &tterm_default;
  size_t old = t->tabsize;
  t->tabsize = size;
  return old;
}


/* Return the tab size of TTERM. */

size_t
tterm_tabsize (struct tterm * tterm)
{
  struct tterm *t = tterm ? tterm : &tterm_default;
  return t->tabsize;
}

/* Initialize the values taking the environment into account (for line
   width, and tabsize). */

void
tterm_initialize (struct tterm *tterm, FILE *stream)
{
  const char *cp;
  struct tterm *t = tterm ? tterm : &tterm_default;
  long int tmp_long;

  /* Length of the line.  */
  if ((cp = getenv ("COLUMNS")) && *cp)
    {
      if (xstrtol (cp, NULL, 0, &tmp_long, NULL) == LONGINT_OK
	  && 0 < tmp_long && tmp_long <= INT_MAX)
	{
	  t->width = (int) tmp_long;
	}
      else
	{
	  error (0, 0,
		 "ignoring invalid width in environment variable COLUMNS: %s",
		 quotearg (cp));
	}
    }

#ifdef TIOCGWINSZ
  {
    struct winsize ws;

    if (ioctl (fileno (stream), TIOCGWINSZ, &ws) != -1 && ws.ws_col != 0)
      t->width = ws.ws_col;
  }
#endif

  /* Tabulation size.  */
  /* Using the TABSIZE environment variable is not POSIX-approved.
     Ignore it when POSIXLY_CORRECT is set.  */
  if (!getenv ("POSIXLY_CORRECT") && (cp = getenv ("TABSIZE")) && *cp)
    {
      if (xstrtol (cp, NULL, 0, &tmp_long, NULL) == LONGINT_OK
	  && 0 <= tmp_long && tmp_long <= INT_MAX)
	{
	  t->tabsize = (int) tmp_long;
	}
      else
	{
	  error (0, 0,
	   "ignoring invalid tab size in environment variable TABSIZE: %s",
		 quotearg (cp));
	}
    }
}

/* Sample test of TinyTerm. */

#ifdef TEST

#include <stdio,h>

const char * program_name = "tterm";

int
main (int argc, char **argv)
{
  tterm_init ();

  printf ("Width = %d, Tabsize = %d\n",
	  tterm_width (NULL), tterm_tabsize (NULL));

  return 0;
}
#endif
