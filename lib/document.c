/*
 * document.c
 *
 * handle report of various documenting formats.
 *
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 *
 */

/*
 * This file is part of a2ps.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * $Id: document.c,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */

#include "a2ps.h"
#include "routines.h"
#include "document.h"
#include "xstrrpl.h"

/* The rules for authors to Texinfo */
#define AUTHORS_TO_TEXINFO 	\
   "@", "@@",			\
   NULL

/* The rules to convert documentation to another format */
/* 1. Plain ASCII */
#define DOC_TO_PLAIN \
  "url(", "",		\
  ")url(", " (",	\
  ")url", ")",		\
  "samp(", "`",		\
  ")samp", "'",		\
  "emph(", "*",		\
  ")emph", "*",		\
  "code(", "",		\
  ")code", "",		\
  "@example\n",		"",	\
  "@end example\n", 	"",	\
  "@end example", 	"",	/* Just in case */	\
  "@itemize\n",		"",	\
  "@end itemize", 	"",	\
  "@item\n",		" - ",	\
  "@@",			"@",	\
  NULL

/* 2. Towards HTML */
#define DOC_TO_HTML \
  "url(", "<a href=\"",	\
  ")url(", "\">",	\
  ")url", "</a>",	\
  "emph(", "<emph>",	\
  ")emph", "</emph>'",	\
  "samp(", "`<code>",	\
  ")samp", "</code>'",	\
  "code(", "<code>",	\
  ")code", "</code>",	\
  "@example",		"<pre>",	\
  "@end example", 	"</pre>",	\
  "@itemize",		"<ul>",	\
  "@end itemize", 	"</ul>",	\
  "@item\n",		"<li>",	\
  "@@",			"@",	\
  NULL

/* 3. Towards Texinfo */
#define DOC_TO_TEXINFO \
  "emph(", "@emph{",	\
  ")emph", "}",		\
  "samp(", "@samp{",	\
  ")samp", "}",		\
  "code(", "@code{",	\
  ")code", "}",		\
  "url(", "@href{",	\
  ")url(", ",",		\
  ")url", "}",		\
  "@itemize",		"@itemize @minus",	\
  NULL


/************************************************************************/
/*      The authors list handling                                       */
/************************************************************************/
/*
 * In the following, we do not want to see Akim Demaille's name
 * in the style sheet context, since there would be too many.
 */
/*
 * Split the authors and print them on STREAM using AUTHOR_FMT
 * (which %1s is clean name, and %2s is the email), separated
 *  with BETWEEN).  They must be separated with ',', and
 * use this convention "First Last <email>".
 */
static inline void
authors_print (const uchar * authors, FILE * stream,
	       const char *before,
	       const char *author_fmt, const char *between,
	       const char *after)
{
  char *cp, *author, *email;
  bool first = true;

  if (!authors)
    return;

  /* Work on a copy */
  astrcpy (cp, authors);
  cp = strtok (cp, ",");

  while (cp)
    {
      author = cp;
      email = author + strcspn (author, "<");
      *(email - 1) = '\0';
      email++;
      *(email + strcspn (email, ">")) = '\0';
      *(email - 1) = '\0';
      if (!strequ (author, "Akim Demaille"))
	{
	  if (first)
	    {
	      fputs (before, stream);
	      first = false;
	    }
	  else
	    fputs (between, stream);
	  fprintf (stream, author_fmt, author, email);
	}
      cp = strtok (NULL, ",");
    }
  if (!first)
    fputs (after, stream);
}

/*
 * Plain : nothing to change
 */
void
authors_print_plain (const uchar * authors, FILE * stream,
		     const char *before)
{
  authors_print (authors, stream,
		 before, "%s <%s>", ", ", ".\n");
}

/*
 * HTML : nothing to change
 */
void
authors_print_html (const uchar * authors, FILE * stream,
		    const char *before)
{
  authors_print (authors, stream,
		 before,
		 "<a href=\"mailto:%2$s\">%1$s</a>", ", ",
		 ".\n");
}

/*
 * Plain : nothing to change
 */
void
authors_print_texinfo (const uchar * authors, FILE * stream,
		       const char *before)
{
  uchar *cp;
  /* We must quote the @ of the emails */
  cp = (uchar *) xvstrrpl ((const char *) authors,
			   AUTHORS_TO_TEXINFO);

  /* Don't print the email, that makes too wide output. */
  authors_print (cp, stream,
		 before, "%s", ", ", ".\n");
  free (cp);
}
/************************************************************************/
/*      The documentation handling                                      */
/************************************************************************/
/* 1. Plain ASCII */
void
documentation_print_plain (const uchar * documentation,
			   const char *format, FILE * stream)
{
  char *cp;

  if (!documentation)
    return;

  cp = xvstrrpl ((const char *) documentation, DOC_TO_PLAIN);
  fprintf (stream, format, cp);
  free (cp);
}

/* 2. Towards HTML */
void
documentation_print_html (const uchar * documentation,
			  const char *format, FILE * stream)
{
  char *cp;

  if (!documentation)
    return;

  cp = xvstrrpl ((const char *) documentation, DOC_TO_HTML);
  fprintf (stream, format, cp);
  free (cp);
}

/* 3. Towards Texinfo */
void
documentation_print_texinfo (const uchar * documentation,
			     const char *format, FILE * stream)
{
  char *cp;

  if (!documentation)
    return;

  cp = xvstrrpl ((const char *) documentation, DOC_TO_TEXINFO);
  fprintf (stream, format, cp);
  free (cp);
}
