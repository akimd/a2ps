/* quotearg.c - quote arguments for output
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

/* Written by Paul Eggert <eggert@twinsun.com> */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <quotearg.h>
#include <xalloc.h>
#include <wctype.h>

#include <ctype.h>
#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
# define ISASCII(c) 1
#else
# define ISASCII(c) isascii (c)
#endif
#define ISPRINT(c) (ISASCII (c) && isprint (c))

#if ENABLE_NLS
# include <libintl.h>
# define _(text) gettext (text)
#else
# define _(text) text
#endif

#if HAVE_LIMITS_H
# include <limits.h>
#endif
#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif
#ifndef UCHAR_MAX
# define UCHAR_MAX ((unsigned char) -1)
#endif

#if HAVE_STDLIB_H
# include <stdlib.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#endif

#if HAVE_MBRTOWC && HAVE_WCHAR_H
# include <wchar.h>
#else
# define iswprint(wc) 1
# define mbrtowc(pwc, s, n, ps) 1
# define mbsinit(ps) 1
# define mbstate_t int
#endif

#define INT_BITS (sizeof (int) * CHAR_BIT)

struct quoting_options
{
  /* Basic quoting style.  */
  enum quoting_style style;

  /* Quote the characters indicated by this bit vector even if the
     quoting style would not normally require them to be quoted.  */
  int quote_these_too[((UCHAR_MAX + 1) / INT_BITS
		       + ((UCHAR_MAX + 1) % INT_BITS != 0))];
};

/* Names of quoting styles.  */
char const *const quoting_style_args[] =
{
  "literal",
  "shell",
  "shell-always",
  "c",
  "escape",
  "locale",
  0
};

/* Correspondences to quoting style names.  */
enum quoting_style const quoting_style_vals[] =
{
  literal_quoting_style,
  shell_quoting_style,
  shell_always_quoting_style,
  c_quoting_style,
  escape_quoting_style,
  locale_quoting_style
};

/* The default quoting options.  */
static struct quoting_options default_quoting_options;

/* Allocate a new set of quoting options, with contents initially identical
   to O if O is not null, or to the default if O is null.
   It is the caller's responsibility to free the result.  */
struct quoting_options *
clone_quoting_options (struct quoting_options *o)
{
  struct quoting_options *p
    = (struct quoting_options *) xmalloc (sizeof (struct quoting_options));
  *p = *(o ? o : &default_quoting_options);
  return p;
}

/* Get the value of O's quoting style.  If O is null, use the default.  */
enum quoting_style
get_quoting_style (struct quoting_options *o)
{
  return (o ? o : &default_quoting_options)->style;
}

/* In O (or in the default if O is null),
   set the value of the quoting style to S.  */
void
set_quoting_style (struct quoting_options *o, enum quoting_style s)
{
  (o ? o : &default_quoting_options)->style = s;
}

/* In O (or in the default if O is null),
   set the value of the quoting options for character C to I.
   Return the old value.  Currently, the only values defined for I are
   0 (the default) and 1 (which means to quote the character even if
   it would not otherwise be quoted).  */
int
set_char_quoting (struct quoting_options *o, char c, int i)
{
  unsigned char uc = c;
  int *p = (o ? o : &default_quoting_options)->quote_these_too + uc / INT_BITS;
  int shift = uc % INT_BITS;
  int r = (*p >> shift) & 1;
  *p ^= ((i & 1) ^ r) << shift;
  return r;
}

/* Place into buffer BUFFER (of size BUFFERSIZE) a quoted version of
   argument ARG (of size ARGSIZE), using QUOTING_STYLE and the
   non-quoting-style part of O to control quoting.
   Terminate the output with a null character, and return the written
   size of the output, not counting the terminating null.
   If BUFFERSIZE is too small to store the output string, return the
   value that would have been returned had BUFFERSIZE been large enough.
   If ARGSIZE is -1, use the string length of the argument for ARGSIZE.

   This function acts like quotearg_buffer (BUFFER, BUFFERSIZE, ARG,
   ARGSIZE, O), except it uses QUOTING_STYLE instead of the quoting
   style specified by O, and O may not be null.  */

static size_t
quotearg_buffer_restyled (char *buffer, size_t buffersize,
			  char const *arg, size_t argsize,
			  enum quoting_style quoting_style,
			  struct quoting_options const *o)
{
  size_t i;
  size_t len = 0;
  char const *quote_string = 0;
  size_t quote_string_len = 0;
  int backslash_escapes = 0;

#define STORE(c) \
    do \
      { \
	if (len < buffersize) \
	  buffer[len] = (c); \
	len++; \
      } \
    while (0)

  switch (quoting_style)
    {
    case c_quoting_style:
      STORE ('"');
      backslash_escapes = 1;
      quote_string = "\"";
      quote_string_len = 1;
      break;

    case escape_quoting_style:
      backslash_escapes = 1;
      break;

    case locale_quoting_style:
      for (quote_string = _("`"); *quote_string; quote_string++)
	STORE (*quote_string);
      backslash_escapes = 1;
      quote_string = _("'");
      quote_string_len = strlen (quote_string);
      break;

    case shell_always_quoting_style:
      STORE ('\'');
      quote_string = "'";
      quote_string_len = 1;
      break;

    default:
      break;
    }

  for (i = 0;  ! (argsize == (size_t) -1 ? arg[i] == '\0' : i == argsize);  i++)
    {
      unsigned char c;
      unsigned char esc;

      if (backslash_escapes
	  && quote_string_len
	  && i + quote_string_len <= argsize
	  && memcmp (arg + i, quote_string, quote_string_len) == 0)
	STORE ('\\');

      c = arg[i];
      switch (c)
	{
	case '?':
	  switch (quoting_style)
	    {
	    case shell_quoting_style:
	      goto use_shell_always_quoting_style;

	    case c_quoting_style:
	      if (i + 2 < argsize && arg[i + 1] == '?')
		switch (arg[i + 2])
		  {
		  case '!': case '\'':
		  case '(': case ')': case '-': case '/':
		  case '<': case '=': case '>':
		    /* Escape the second '?' in what would otherwise be
		       a trigraph.  */
		    i += 2;
		    c = arg[i + 2];
		    STORE ('?');
		    STORE ('\\');
		    STORE ('?');
		    break;
		  }
	      break;

	    default:
	      break;
	    }
	  break;

#if HAVE_C_BACKSLASH_A
	case '\a': esc = 'a'; goto c_escape;
#endif
	case '\b': esc = 'b'; goto c_escape;
	case '\f': esc = 'f'; goto c_escape;
	case '\n': esc = 'n'; goto c_escape;
	case '\r': esc = 'r'; goto c_escape;
	case '\t': esc = 't'; goto c_escape;
	case '\v': esc = 'v'; goto c_escape;
	case '\\': esc = c; goto c_escape;

	c_escape:
	  if (backslash_escapes)
	    {
	      c = esc;
	      goto store_escape;
	    }
	  if (quoting_style == shell_quoting_style)
	    goto use_shell_always_quoting_style;
	  break;

	case '#': case '~':
	  if (i != 0)
	    break;
	  /* Fall through.  */
	case ' ':
	case '!': /* special in bash */
	case '"': case '$': case '&':
	case '(': case ')': case '*': case ';':
	case '<': case '>': case '[':
	case '^': /* special in old /bin/sh, e.g. SunOS 4.1.4 */
	case '`': case '|':
	  /* A shell special character.  In theory, '$' and '`' could
	     be the first bytes of multibyte characters, which means
	     we should check them with mbrtowc, but in practice this
	     doesn't happen so it's not worth worrying about.  */
	  if (quoting_style == shell_quoting_style)
	    goto use_shell_always_quoting_style;
	  break;

	case '\'':
	  switch (quoting_style)
	    {
	    case shell_quoting_style:
	      goto use_shell_always_quoting_style;

	    case shell_always_quoting_style:
	      STORE ('\'');
	      STORE ('\\');
	      STORE ('\'');
	      break;

	    default:
	      break;
	    }
	  break;

	case '%': case '+': case ',': case '-': case '.': case '/':
	case '0': case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9': case ':': case '=':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
	case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
	case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z': case ']': case '_': case 'a': case 'b':
	case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
	case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
	case 'o': case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case '{': case '}':
	  /* These characters don't cause problems, no matter what the
	     quoting style is.  They cannot start multibyte sequences.  */
	  break;

	default:
	  /* If we have a multibyte sequence, copy it until we reach
	     its end, find an error, or come back to the initial shift
	     state.  For C-like styles, if the sequence has
	     unprintable characters, escape the whole sequence, since
	     we can't easily escape single characters within it.  */
	  {
	    /* Length of multibyte sequence found so far.  */
	    size_t m = 0;

	    int printable = 1;
	    mbstate_t mbstate;
	    memset (&mbstate, 0, sizeof mbstate);

	    if (argsize == (size_t) -1)
	      argsize = strlen (arg);
		
	    do
	      {
		wchar_t w;
		size_t bytes = mbrtowc (&w, &arg[i + m],
					argsize - (i + m), &mbstate);
		if (bytes == 0)
		  break;
		else if (bytes == (size_t) -1)
		  {
		    printable = 0;
		    break;
		  }
		else if (bytes == (size_t) -2)
		  {
		    printable = 0;
		    while (i + m < argsize && arg[i + m])
		      m++;
		    break;
		  }
		else
		  {
		    if (! iswprint (w))
		      printable = 0;
		    m += bytes;
		  }
	      }
	    while (! mbsinit (&mbstate));
		
	    if (m <= 1)
	      {
		/* Escape a unibyte character like a multibyte
		   sequence if using backslash escapes, and if the
		   character is not printable.  */
		m = backslash_escapes && ! ISPRINT (c);
		printable = 0;
	      }

	    if (m)
	      {
		/* Output a multibyte sequence, or an escaped
		   unprintable unibyte character.  */
		size_t imax = i + m - 1;

		for (;;)
		  {
		    if (backslash_escapes && ! printable)
		      {
			STORE ('\\');
			STORE ('0' + (c >> 6));
			STORE ('0' + ((c >> 3) & 7));
			c = '0' + (c & 7);
		      }
		    if (i == imax)
		      break;
		    STORE (c);
		    c = arg[++i];
		  }

		goto store_c;
	      }
	  }
	}

      if (! (backslash_escapes
	     && o->quote_these_too[c / INT_BITS] & (1 << (c % INT_BITS))))
	goto store_c;
      
    store_escape:
      STORE ('\\');

    store_c:
      STORE (c);
    }

  if (quote_string)
    for (; *quote_string; quote_string++)
      STORE (*quote_string);

  if (len < buffersize)
    buffer[len] = '\0';
  return len;

 use_shell_always_quoting_style:
  return quotearg_buffer_restyled (buffer, buffersize, arg, argsize,
				   shell_always_quoting_style, o);
}

/* Place into buffer BUFFER (of size BUFFERSIZE) a quoted version of
   argument ARG (of size ARGSIZE), using O to control quoting.
   If O is null, use the default.
   Terminate the output with a null character, and return the written
   size of the output, not counting the terminating null.
   If BUFFERSIZE is too small to store the output string, return the
   value that would have been returned had BUFFERSIZE been large enough.
   If ARGSIZE is -1, use the string length of the argument for ARGSIZE.  */
size_t
quotearg_buffer (char *buffer, size_t buffersize,
		 char const *arg, size_t argsize,
		 struct quoting_options const *o)
{
  struct quoting_options const *p = o ? o : &default_quoting_options;
  return quotearg_buffer_restyled (buffer, buffersize, arg, argsize,
				   p->style, p);
}

/* Use storage slot N to return a quoted version of the string ARG.
   OPTIONS specifies the quoting options.
   The returned value points to static storage that can be
   reused by the next call to this function with the same value of N.
   N must be nonnegative.  N is deliberately declared with type `int'
   to allow for future extensions (using negative values).  */
static char *
quotearg_n_options (int n, char const *arg,
		    struct quoting_options const *options)
{
  static unsigned int nslots;
  static struct slotvec
    {
      size_t size;
      char *val;
    } *slotvec;

  if (nslots <= n)
    {
      int n1 = n + 1;
      size_t s = n1 * sizeof (struct slotvec);
      if (! (0 < n1 && n1 == s / sizeof (struct slotvec)))
	abort ();
      slotvec = (struct slotvec *) xrealloc (slotvec, s);
      memset (slotvec + nslots, 0, (n1 - nslots) * sizeof (struct slotvec));
      nslots = n;
    }

  {
    size_t size = slotvec[n].size;
    char *val = slotvec[n].val;
    size_t qsize = quotearg_buffer (val, size, arg, (size_t) -1, options);

    if (size <= qsize)
      {
	slotvec[n].size = size = qsize + 1;
	slotvec[n].val = val = xrealloc (val, size);
	quotearg_buffer (val, size, arg, (size_t) -1, options);
      }

    return val;
  }
}

char *
quotearg_n (unsigned int n, char const *arg)
{
  return quotearg_n_options (n, arg, &default_quoting_options);
}

char *
quotearg (char const *arg)
{
  return quotearg_n (0, arg);
}

char *
quotearg_n_style (unsigned int n, enum quoting_style s, char const *arg)
{
  struct quoting_options o;
  o.style = s;
  memset (o.quote_these_too, 0, sizeof o.quote_these_too);
  return quotearg_n_options (n, arg, &o);
}

char *
quotearg_style (enum quoting_style s, char const *arg)
{
  return quotearg_n_style (0, s, arg);
}

char *
quotearg_char (char const *arg, char ch)
{
  struct quoting_options options;
  options = default_quoting_options;
  set_char_quoting (&options, ch, 1);
  return quotearg_n_options (0, arg, &options);
}

char *
quotearg_colon (char const *arg)
{
  return quotearg_char (arg, ':');
}