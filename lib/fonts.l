%{ /* -*- c -*- */
/*
 * Lexer for afm files.
 *
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
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
 * $Id: fonts.l,v 1.1.1.1.2.1 2007/12/29 01:58:18 mhatta Exp $
 */

#include "a2ps.h"
#include "routines.h"
#include "message.h"
#include "hashtab.h"
#include "jobs.h"
#include "pathwalk.h"
#include "fonts.h"
#include "psstat.h"
#include "pair_ht.h"
#include "dsc.h"
#include "quotearg.h"

/*
  What this file is in charge of?

  Its main responsabilities are:

  - to be able to provide, given a font name and an encoding vector,
  to return an unsigned int array [256] of the width of each char.

  - to be able to dump the requirements (the needed IncludeResource: font)
  so that we have a correct PS-3 file.

  - to download a font file

  To acheive this goal, the main task is reading the AFM file of a font,
  to store what it has learnt into a hash table storing any pertinent
  info read in previous AFM files. (We may need this info several times,
  and we do not want to waste time).

  This file is decomposed into several parts:

  0. Handle the fonts.map files

  1. The lexer of afm files.
  The format is simple enough so that we do not need a parser.

  2. The cells (of type struct font_info) handling.

  3. The hash table maintenance.
  This hash table is, as usually, stored in the a2ps_job so that it
  does not pollute the space, and make liba2ps still "clean".

  4. Down load a font file

*/

/************************************************************************/
/*			fonts.map handling				*/
/************************************************************************/
struct pair_htable *
fonts_map_new (void)
{
  return pair_table_new ();
}

void
fonts_map_free (struct pair_htable * table)
{
  pair_table_free (table);
}

static inline void
fonts_map_add (struct pair_htable * fonts_map,
	       const char * alias,
	       const char * key)
{
  pair_add (fonts_map, alias, key);
}

/*
 * What is the short name used for the font file corresponding
 * to FONTNAME
 */
static const char *
fonts_map_resolve_alias (struct pair_htable * fonts_map,
			 const char * fontname)
{
  char * res;
  res = pair_get (fonts_map, fontname);
  if (res)
    return res;
  else
    /* The fonts has not been declared.  Try its full name */
    return fontname;
}

/*
 * Read a sheets map file
 */
static int
load_fonts_map (struct a2ps_job * job, const char * file)
{
  return pair_table_load (job->fonts_map, file);
}

/*
 * Read the fonts.map file
 */
int
load_main_fonts_map (struct a2ps_job * job)
{
  char * file;

  /* System's */
  file = xpw_find_file (job->common.path, "fonts.map", NULL);
  load_fonts_map (job, file);
  free (file);
  return 1;
}

/************************************************************************/
/* Defining the structures, and lexer of afm files			*/
/************************************************************************/
/* Definition of the structure that retains the wx information */
struct wx_entry
{
  char * glyph;
  int index;
  unsigned int wx;
};

struct font_info
{
  char * key;
  struct hash_table_s * wx;
};

/*
  The following variables are global because I see no other
  means to share information with the lexer.
*/

/* Last WX and index read */
static unsigned int last_wx_read;
static int last_index_read;

/* the current font info being filled */
static struct font_info * font_info_being_parsed = NULL;

static void wx_entry_add PARAMS ((struct hash_table_s * table,
				  const char * glyph,
				  int the_index, unsigned int wx));

int yylex PARAMS ((void));

%}

%option yylineno
%option prefix="afm"
%option outfile="lex.yy.c"

%x STATE_CHAR_METRICS

integer [0-9]+
symbol	[^/#, \t\r\f\n\"()+;]*

%%

StartCharMetrics.*\n		{ BEGIN STATE_CHAR_METRICS; }

.*\n		{  /* Ignore */; }


<STATE_CHAR_METRICS>{		/* string of characters */
  EndCharMetrics 			{ /* return the string */
    /* There is nothing more we want to know, but return to
    * the INITIAL state for the next call... */
    BEGIN INITIAL;
    /* Cut parsing right now (speed up) */
    yyterminate ();
  }

  C\ {integer}		{
    char *cursor = yytext + 3;

    last_index_read = yytext[2] - '0';
    while (*cursor)
      last_index_read = 10 * last_index_read + *cursor++ - '0';
  }

  C\ -{integer}		{
    char *cursor = yytext + 4;

    last_index_read = yytext[3] - '0';
    while (*cursor)
      last_index_read = 10 * last_index_read + *cursor++ - '0';
    last_index_read = - last_index_read;
  }

  WX\ {integer}		{
    char *cursor = yytext + 4;

    last_wx_read = yytext[3] - '0';
    while (*cursor)
      last_wx_read = 10 * last_wx_read + *cursor++ - '0';
  }

  N\ {symbol}		{
    char *cursor = yytext + 2;
    wx_entry_add (font_info_being_parsed->wx, cursor,
		  last_index_read, last_wx_read);
  }

  . 	{ /* Ignore */; }
  \n 	{ /* Ignore */; }
}

%%
int
yywrap (void)
{
  return 1;
}

/************************************************************************
 * hash tables associating a glyph name to the corresponding wx		*
 ************************************************************************/
/*
 * Basic routines
 */
static unsigned long
wx_entry_hash_1 (struct wx_entry *wx_entry)
{
  return_STRING_HASH_1 (wx_entry->glyph);
}

static unsigned long
wx_entry_hash_2 (struct wx_entry *wx_entry)
{
  return_STRING_HASH_2 (wx_entry->glyph);
}

static int
wx_entry_hash_cmp (struct wx_entry *x, struct wx_entry *y)
{
  return_STRING_COMPARE (x->glyph, y->glyph);
}

#if 0
/*
 * For sorting them in alpha order
 */
static int
wx_entry_hash_qcmp (struct wx_entry **x, struct wx_entry **y)
{
  return_STRING_COMPARE ((*x)->glyph, (*y)->glyph);
}
#endif

/*
 * For sorting them in the order of their index
 * (or `code', i.e. 32 for space)
 */
static int
wx_entry_hash_index_qcmp (struct wx_entry **x, struct wx_entry **y)
{
  return ((*y)->index - (*x)->index);
}

/*
 * Create the structure that stores the list of wx_entrys
 */
static struct hash_table_s *
wx_entry_table_new (void)
{
  struct hash_table_s * res;

  res = XMALLOC (struct hash_table_s, 1);
  hash_init (res, 8,
	     (hash_func_t) wx_entry_hash_1,
	     (hash_func_t) wx_entry_hash_2,
	     (hash_cmp_func_t) wx_entry_hash_cmp);
  return res;
}

/*
 * Free a wx_entry entry
 */
static void
hash_free_wx_entry (struct wx_entry * wx_entry)
{
  free (wx_entry->glyph);
  free (wx_entry);
}

/*
 * Free the whole structure
 */
static void
wx_entry_table_free (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) hash_free_wx_entry);
  free (table);
}

/*
 *  Add a wx_entry, with your own allocation for them.
 * It GLYPH is yet used, override its wx with WX
 */
static void
wx_entry_add (struct hash_table_s * table,
	      const char * glyph, int the_index, unsigned int wx)
{
  struct wx_entry * item, token;

  token.glyph = (char *) glyph;
  item = (struct wx_entry *) hash_find_item (table, &token);

  if (item) {
    /* This is abnormal.  There is a problem in the afm file
     * (several definition of the same glyph).
     * Since there is a bug in Ogonkify's AFM files, just shut up. */
    return;
  } else {
    item = XMALLOC (struct wx_entry, 1);
    item->glyph = xstrdup(glyph);
    item->wx = wx;
    item->index = the_index;
    hash_insert (table, item);
  }
}

/*
 * Get the wx associated to GLYPH in TABLE
 * Return NULL upon error (this means that it is not
 * valid to enter NULL as a wx)
 */
static inline unsigned int
wx_entry_get_wx (struct hash_table_s * table,
		 const char * glyph)
{
  struct wx_entry * item, token;

  token.glyph = (char *) glyph;
  item = (struct wx_entry *) hash_find_item (table, &token);

  if (item)
    return item->wx;

  return 0;
}

#if 0
/*
 * Typically for --list-features
 */
static void
wx_entry_list_short (struct hash_table_s * table, FILE * stream)
{
  int i, tab;
  struct wx_entry ** entries;
  entries = (struct wx_entry **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) wx_entry_hash_qcmp);

  tab = 0;
  for (i = 0 ; entries[i] ; i++) {
    if (!(tab % 7))
      fputs ("\n  ", stream);
    fprintf (stream, "%-10s", entries[i]->glyph);
    tab++;
  }
  putc ('\n', stream);
  free (entries);
}

/*
 * Typically for --list-<something>
 */
static void
wx_entry_list_long (struct hash_table_s * table, FILE * stream)
{
  int i;
  struct wx_entry ** entries;
  entries = (struct wx_entry **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) wx_entry_hash_qcmp);

  for (i = 0 ; entries[i] ; i++)
    fprintf (stream, "  %-10s (%d) = %d\n",
	     entries[i]->glyph, entries[i]->index, entries[i]->wx);

  putc ('\n', stream);
  free (entries);
}
#endif

/*
 * Getting the glyphs in the order of the index
 */
static inline struct wx_entry **
wx_entry_get_in_index_order (struct hash_table_s * table)
{
  return (struct wx_entry **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) wx_entry_hash_index_qcmp);
}

/************************************************************************
 * A font (not encoded)							*
 ************************************************************************/
/*
 * Basic routines
 */
static unsigned long
font_info_hash_1 (struct font_info *font_info)
{
  return_STRING_HASH_1 (font_info->key);
}

static unsigned long
font_info_hash_2 (struct font_info *font_info)
{
  return_STRING_HASH_2 (font_info->key);
}

static int
font_info_hash_cmp (struct font_info *x, struct font_info *y)
{
  return_STRING_COMPARE (x->key, y->key);
}

#if 0
/*
 * For sorting them in alpha order
 */
static int
font_info_hash_qcmp (struct font_info **x, struct font_info **y)
{
  return_STRING_COMPARE ((*x)->key, (*y)->key);
}
#endif

/*
 * Create the structure that stores the list of font_infos
 */
struct hash_table_s *
font_info_table_new (void)
{
  NEW (struct hash_table_s, res);

  hash_init (res, 8,
	     (hash_func_t) font_info_hash_1,
	     (hash_func_t) font_info_hash_2,
	     (hash_cmp_func_t) font_info_hash_cmp);
  return res;
}

/*
 * Free a font_info entry
 */
static void
hash_free_font_info (struct font_info * font_info)
{
  free (font_info->key);
  wx_entry_table_free (font_info->wx);
  free (font_info);
}

/*
 * Free the whole structure
 */
void
font_info_table_free (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) hash_free_font_info);
  free (table);
}

/*
 * Create and return a new font_info cell, with key = KEY
 */
static inline struct font_info *
font_info_new (const char * key)
{
  NEW (struct font_info, res);

  res->key = xstrdup (key);
  res->wx = wx_entry_table_new ();

  return res;
}

/*
 * It need JOB for the lib path, to look for the afm file.
 */
static void
font_info_finalize (struct a2ps_job * job, struct font_info * font)
{
  char * file;
  const char * filename;
  FILE * stream;

  yylineno = 0;

  /* Find its AFM file: first resolve its font file name */
  filename = fonts_map_resolve_alias (job->fonts_map, font->key);
  file = xpw_find_file (job->common.path, filename, ".afm");

  /* Go for it */
  message (msg_font | msg_file | msg_parse,
	   (stderr, "Parsing file `%s'\n", file));
  stream = xrfopen (file);

  /* Prepare the global var that encloses the hash table.
   * WARNING: this is not reentrant... */
  font_info_being_parsed = font;
  yyrestart (stream);
  yylex ();

  fclose (stream);
  free (file);
}

/*
 * Create a font_info for font KEY and store it in the hash table
 * And register in the DSC requirements
 */
void
font_info_add (struct a2ps_job * job, const char * key)
{
  struct font_info * font;

  /* Create, fill and store the font_info structure */
  font = font_info_new (key);
  font_info_finalize (job, font);
  hash_insert (job->font_infos, font);

  /* Register the font for the DSC */
  add_required_font (job, key);
}

/*
 * Retreive.
 */
static inline struct font_info *
font_info_get (struct a2ps_job * job, const char * key)
{
  struct font_info token, * res;

  token.key = (char *) key;
  res = (struct font_info *) hash_find_item (job->font_infos, &token);
  if (!res) {/* Require loading of the font's info */
    font_info_add (job, key);
    res = (struct font_info *) hash_find_item (job->font_infos, &token);
  }
  return res;
}

/*
 * For instance Symbol is not to reencode => don't put it
 * in the encoding specific dictionary
 */
int
font_is_to_reencode (PARAM_UNUSED struct a2ps_job * job, const char * key)
{
  return !strequ (key, "Symbol");
}

/*
 * Return a WX array that reflects the font KEY according to
 * the encoding VECTOR
 */
void
font_info_get_wx_for_vector (struct a2ps_job * job,
			     const char * key,
			     char * vector [256],
			     unsigned int * wx)
{
  struct font_info * font_info = font_info_get (job, key);
  struct wx_entry ** sorted_by_index;
  int c, the_index;

  if (font_is_to_reencode (job, key))
    {
      /* This is a regular font, such as Courier, which needs to
       * be recoded according to the current char set */
      for (c = 0 ; c < 256 ; c++)
	wx [c] = wx_entry_get_wx (font_info->wx, vector [c]);
    }
  else
    {
      /* This is a font such as Symbol, which has its own char set */
      for (c = 0 ; c < 256 ; c++)
	wx [c] = 0;
      sorted_by_index = wx_entry_get_in_index_order (font_info->wx);
      for (c = 0 ; sorted_by_index [c] ; c++)
	{
	  the_index = sorted_by_index [c]->index;
	  if (0 <= the_index && the_index < 256)
	    wx [the_index] = sorted_by_index [c]->wx;
	}
    }
}

/*
 * A font exists iff a KEY.afm file exists
 */
int
font_exists (struct a2ps_job * job, const char * key)
{
  const char * filename;

  filename = fonts_map_resolve_alias (job->fonts_map, key);
  return pw_file_exists_p (job->common.path, filename, ".afm");
}

/*
 * Dump the setup of all the fonts not to be reencoded
 */
void
font_info_table_dump_special_font_setup (FILE * stream,
					 struct a2ps_job * job)
{
  struct font_info ** fonts;
  int i;

  fonts = (struct font_info **)
    hash_dump (job->font_infos, NULL, NULL);

  for (i = 0 ; fonts [i] ; i++)
    if (!font_is_to_reencode (job, fonts [i]->key))
      fprintf (stream, "/f%s /%s findfont def\n",
	       fonts [i]->key, fonts [i]->key);
}

/************************************************************************
 * Downloading of PostScript font					*
 ************************************************************************/
/*
   This is according to
   << Supporting Downloadable PostScript Language Fonts >>
   by Adobe
 */

/*
 * Read and dump a segment
 * Returns:
 * - 0 something was read
 * - 1 nothing had to be read
 * - 2 an error occured
 */
static inline int
binary_font_dump_segment (FILE * in, FILE * out)
{
  enum segment_type
  {
    ascii = 1, binary = 2, end_of_file = 3
  };
  enum segment_type segment_type;
  int magic;
  size_t length, read_length, length_to_read;
  uchar buf [BUFSIZ];

  magic = getc (in);

  if (magic != 128)	/* This is most weird! */
    return 1;

  /* The type of the segment */
  segment_type = (enum segment_type) getc (in);

  /* Four bytes for the size of the segment */
  length  = getc (in);
  length += getc (in) * 256;
  length += getc (in) * 256 * 256;
  length += getc (in) * 256 * 256 * 256;

  switch (segment_type)
    {
    case ascii:
      while (length)
	{
	  size_t i;
	  /* Don't read more than the length of this segment */
	  length_to_read = length < sizeof (buf) ? length : sizeof (buf);
	  read_length = fread (buf, sizeof (char), length_to_read, in);
	  if (!read_length)
	    /* Error */
	    return 2;
	  length -= read_length;
	  for (i= 0 ; i < read_length ; i++)
	    switch (buf [i])
	      {
	      case '\r':	/* eerk: Mac eol. */
		putc ('\n', out);
		break;
	      default:
		putc (buf [i], out);
	      }
	}
      break;

    case binary:
      while (length)
	{
	  size_t i;
	  /* Don't read more than the length of this segment */
	  length_to_read = length < sizeof (buf) ? length : sizeof (buf);
	  read_length = fread (buf, sizeof (char), length_to_read, in);
	  if (!read_length)
	    /* Error */
	    return 2;
	  length -= read_length;
	  for (i = 0 ; i < read_length ; i++)
	    {
	      fprintf (out, "%02X", buf [i]);
	      /* Don't make too long lines */
	      if (!((i + 1) % 32))
		putc ('\n', out);
	    }
	}
      break;

    case end_of_file:
      /* Nothing more to do */
      return 1;

    default:
      abort ();
    }
  /* Finish with an eol (because it does not look good without */
  putc ('\n', out);
  return 0;
}

/*
 * Dump onto OUT a binary segment from IN
 * Returns true if success, false if error occured
 */
static inline int
binary_font_dump (FILE * in, FILE * out)
{
  int status;

  while (!(status = binary_font_dump_segment (in, out)))
    /* Again */;

  if (status == 2)
    return false;
  else
    return true;
}

/*
 * Put onto OUT the description of the font file IN
 * Returns true if success, false if error occured
 */
static int
internal_font_file_dump (FILE * in, FILE * out)
{
  int c;

  c = getc (in);
  ungetc (c, in);
  if (c == 128)
    {
      /* This a font file a` la IBM-PC */
      return binary_font_dump (in, out);
    }
  else
    {
      streams_copy (in, out);
      return true;
    }
}

/*
 * Dump the font definition files for the fonts
 */
void
dump_fonts (FILE * stream, struct a2ps_job * job)
{
  FILE * fp;
  char * filename;
  const char * fontname;
  char ** fonts;
  int i;

  /* Get all the fonts used that need to be downloaded
  * (Care: there can be 0) */
  fonts = required_fonts_get (job);
  if (!fonts)
    return;

  for (i = 0 ; fonts [i] ; i++)
    {
      /* This font is not known: include its definition file */
      fontname = fonts_map_resolve_alias (job->fonts_map, fonts [i]);

      /* First, find where is the correct file.
       * pfb first, then pfa, finaly gsf. */
      filename = pw_find_file (job->common.path, fontname, ".pfb");
      if (!filename)
	filename = pw_find_file (job->common.path, fontname, ".pfa");
      if (!filename)
	filename = pw_find_file (job->common.path, fontname, ".gsf");
      if (!filename)
	{
	  /* Repeat the same message to avoid too much work to
             translators */
	  char * file;
	  astrcat2 (file, fontname, ".pfb");
	  error (0, 0, _("cannot find file `%s'"), quotearg (file));
	  strcat2 (file, fontname, ".pfa");
	  error (0, 0, _("cannot find file `%s'"), quotearg (file));
	  strcat2 (file, fontname, ".gsf");
	  error (1, 0, _("cannot find file `%s'"), quotearg (file));
	}

      /* Open it */
      fp = xrfopen (filename);

      /* Dump it */
      message (msg_font | msg_file, (stderr, "Dumping file `%s'\n", filename));
      fprintf (stream, "%%%%BeginResource: font %s\n", fonts [i]);
      if (!internal_font_file_dump (fp, stream))
	error (1, 0, "Bad PostScript font file `%s'\n", filename);
      fprintf (stream, "%%%%EndResource\n");

      /* release */
      fclose (fp);
      free (filename);
    }
  free (fonts);
}
