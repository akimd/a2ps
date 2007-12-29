/*
 * encodings.c
 *
 * definition of the char encodings used
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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
#include "encoding.h"
#include "routines.h"
#include "jobs.h"
#include "message.h"
#include "pathwalk.h"
#include "pair_ht.h"
#include "fonts.h"
#include "xobstack.h"
#include "dsc.h"
#include "getshline.h"
#include "document.h"
#include "quotearg.h"
#include "strverscmp.h"

/* Suffix of the Encoding Description Files */
#define EDF_SUFFIX ".edf"


/* FIXME: THe comment is no longer exact
  Dealing with the encodings is a long and painful work.

  There are many things to be done, in as short time as possible.
  The big problem is to associate an array of the width of the
  characters for a given font in a given encoding (two keys).

  a2ps first loads the prologue definitions.  In that file, there are
  lines such as:

  %Face: Plain Courier
  /p {
  false UL
  false BX
  fc setfont
  Show
  } bind def

  By this line, it is requested to encode the font Courier, which
  will be used through the name fc.  One of the problems is that for
  instance Courier does not suppport Latin2 encoding and other.
  Then we define some substitution, related to each encoding.  As
  an example, in latin2.edf, there is the subsitution Courier->
  Courier-Ogonkify.

  So we must be carreful to separate various instanciation of the fonts
  (fc) from one encoding to another.  This is done through ps dictionaries.
  fc is define in latin1dict, latin2dict, with different encodings.  It
  works very well with the ps interpreters: there are no problems
  to encounter later, when using these dictionaries.

  Still, building them is painful if one wants to stay open.

  One may ask: << Why not
   %Face: Plain Courier
  /p {
  false UL
  false BX
  FontPlain setfont		%% Here is the difference
  Show
  >>
  The answer is that in that case if two faces use the same base
  font, them we use twice the memory we need.

  One may reply: << Why not
  %Face: Plain Courier
  /p {
  false UL
  false BX
  FontCourier setfont		%% Here is the difference
  Show
  >>
  The problem is then when somebody wants to use a separate Courier
  instance, with another size, in a header.  Then there should be two
  different FontCourier. Too bad.

  It gets even worse when one knows that all this cannot be resolved
  with the subsitution, since these are known _after_ the prologue
  is loaded.  And I don't want any more cross-references between
  prologues and encodings.

  So the decision, not great, but the easier, will be the second one,
  the one which uses too much memory:

  %Face: Plain Courier
  /p {
  false UL
  false BX
  FontPlain setfont		%% Here is the difference
  Show
  >>

  because it is the most open solution.  Especially if the prologue
  makes use of some other faces (in the headers for instance).
*/

/************************************************************************/
/*			encoding.map handling				*/
/************************************************************************/
/*
  These entries allow to define several aliases for a single encoding.
  This is extremely useful for instance when a2ps-the-prog parse a mail
  and finds tags that define the charset to use.  It could read 8859-1,
  or ISO-8859-1 or iso-8859-1 though the user would like latin1.

  The scheme, of course, should be much more general than just skipping
  ISO|iso-8859, since it is even _wrong_ for, say, latin5 (8859-9)

  From an implementation point of view, it is nothing but a double
  strings hash table (cf. pair_ht.[ch]).
 */
struct pair_htable *
encodings_map_new (void)
{
  return pair_table_new ();
}

void
encodings_map_free (struct pair_htable * table)
{
  pair_table_free (table);
}

/*
 * What is the KEY corresponding to ALIAS?
 */
static char *
encodings_map_resolve_alias (struct pair_htable * encodings_map,
			     const char * alias)
{
  return pair_get (encodings_map, alias);
}

/*
 * Read a sheets map file
 */
static int
load_encodings_map (struct a2ps_job * job, const char *file)
{
  return pair_table_load (job->encodings_map, file);
}

/*
 * Read the encodings.map file
 */
int
load_main_encodings_map (struct a2ps_job * job)
{
  char * file;

  /* System's */
  file = xpw_find_file (job->common.path, "encoding.map", NULL);
  load_encodings_map (job, file);
  if (msg_test (msg_enc))
    {
      fprintf (stderr, "Read encoding.map:\n");
      pair_table_list_long (job->encodings_map, stderr);
    }
  free (file);
  return 1;
}

/************************************************************************/
/*		     	wx handling					*/
/************************************************************************/
static inline unsigned int *
wx_new (void)
{
  int i;
  unsigned int *res = XMALLOC (unsigned int, 256);

  for (i = 0 ; i < 256 ; i++)
    res [i] = 0;

  return res;
}

static void
wx_self_print (unsigned int * wx, FILE * stream)
{
  int j;

  if (wx)
    for (j = 0 ; j < 256 ; j += 8)
      fprintf (stream,
	       "%3d: %04u %04u %04u %04u %04u %04u %04u %04u\n", j,
	       wx [j + 0], wx [j + 1], wx [j + 2], wx [j + 3],
	       wx [j + 4], wx [j + 5], wx [j + 6], wx [j + 7]);
  else
    fputs ("<No WX defined>\n", stream);
}


/************************************************************************/
/*		     fonts entries handling				*/
/************************************************************************/
/*
  Font entries are hashed, but they will be used only at
  initialization: the first thing to do will be to associate
  the font's to the face which uses this font.

  But it is not yet known.  First we read the information on the
  encoding required (which allows us to discover that for instance
  Courier is _not_ to use with Latin2 (use Courier-Ogonki), and
  later, reading the PS prologue, we will learn what face uses
  what font.  At this moment we will associate the font's wx to the
  face.
*/

/*
 * For a font (actually encoding for a given char set)
 * it wx.
 */
struct font_entry {
  char * key;		/* eg. Courier-Ogonki 				*/
  int used;		/* If used, need to be dump in the PS		*/
  int reencode;		/* E.g. Symbol is not to reencode		*/
  unsigned int * wx;	/* Not an array, because some fonts may
			 * share their wx (Courier familly for instance	*/
};

static unsigned long
font_entry_hash_1 (struct font_entry * entry)
{
  return_STRING_HASH_1 (entry->key);
}

static unsigned long
font_entry_hash_2 (struct font_entry * entry)
{
  return_STRING_HASH_2 (entry->key);
}

static int
font_entry_hash_cmp (struct font_entry * x, struct font_entry * y)
{
  return_STRING_COMPARE (x->key, y->key);
}

static int
font_entry_hash_qcmp (struct font_entry ** x, struct font_entry ** y)
{
  return_STRING_COMPARE ((*x)->key, (*y)->key);
}

/*
 * We make a copy of the name, but not of the array
 */
static inline struct font_entry *
font_entry_new (const char * name, unsigned int * wx)
{
  NEW (struct font_entry, res);
  res->key = xstrdup (name);
  res->used = false;
  res->wx = wx;
  return res;
}

/*
 * Free the memory used
 */
static inline void
font_entry_free (struct font_entry * item)
{
  free (item->key);
  free (item);
}

static void
font_entry_self_print (struct font_entry * entry, FILE * stream)
{
  fprintf (stream, "***** %s ***** (%s, %s) ",
	   entry->key,
	   entry->used ? "used" : "not used",
	   entry->reencode ? "to share between encodings" : "reencode");
  wx_self_print (entry->wx, stream);
}

/************************************************************************/
/*		     fonts entries hash table handling			*/
/************************************************************************/
static struct hash_table_s *
font_table_new (void)
{
  struct hash_table_s * res;

  res = XMALLOC (hash_table, 1);
  hash_init (res, 32,
	     (hash_func_t) font_entry_hash_1,
	     (hash_func_t) font_entry_hash_2,
	     (hash_cmp_func_t) font_entry_hash_cmp);
  return res;
}

static void
font_table_free (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) font_entry_free);
  free (table);
}

static void
font_table_self_print (struct hash_table_s * table, FILE * stream)
{
  int i;
  struct font_entry ** entries;
  entries = (struct font_entry **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) font_entry_hash_qcmp);

  for (i = 0 ; entries[i] ; i++)
    font_entry_self_print (entries [i], stream);
  putc ('\n', stream);
  free (entries);
}

/*
 * We make a copy of the name, but not of the array
 */
static inline void
font_entry_add (struct hash_table_s * table,
		const char * name, unsigned int * wx)
{
  hash_insert (table, font_entry_new (name, wx));
}

static inline struct font_entry *
font_entry_get (struct hash_table_s * table, const char * name)
{
  struct font_entry token, * item;
  token.key = (char *) name;
  item = (struct font_entry *) hash_find_item (table, &token);
  return item;
}

static unsigned int *
font_entry_get_wx (struct hash_table_s * table,
		   const char * name)
{
  return font_entry_get (table, name)->wx;
}

static int
font_entry_exists (struct hash_table_s * table, const char * name)
{
  return font_entry_get (table, name) != NULL;
}

static int
font_entry_is_used (struct hash_table_s * table, const char * name)
{
  return font_entry_get (table, name)->used;
}

static void
font_entry_set_used (struct hash_table_s * table, const char * name)
{
  font_entry_get (table, name)->used = true;
}

/************************************************************************/
/*		     encodings entries handling				*/
/************************************************************************/
/*
 * Association of suffixes rules, and corresponding style sheet
 * (The hashing is upon `alias')
 */
struct encoding {
  char * key;			/* e.g. latin1			*/
  char * name;			/* e.g. ISO Latin 1		*/
  uchar * documentation;		/* Useful pieces of text	*/

  char * default_font;		/* When a font can't be used
				   define the font to use	*/
  struct pair_htable * substitutes;	/* e.g. in latin2, don't use
					 * Courier, but Courier-Ogonki	*/

  char * vector[256];	/* Define the char set			*/
  struct darray * font_names_used;
				/* E.g. Courier has been used, but
				 * Actually Courier_Ogonki is the real
				 * font used (cf infra)			*/
  struct hash_table_s * fonts;	/* Contains cells that are
				 * 1. name of font, 2. int wx[256] 	*/
  unsigned int * faces_wx[NB_FACES];
};

/*
 * Create an encoding, reading its associated file.wx
 */
static struct encoding *
encoding_new (const char * key)
{
  int i;

  NEW (struct encoding, res);

  res->key = xstrdup (key);
  res->name = NULL;
  res->default_font = NULL;
  res->documentation = NULL;

  /* Vector will be set by setup */
  res->substitutes = pair_table_new ();
  res->font_names_used = da_new ("List of font names", 10,
				 da_linear, 10,
				 (da_print_func_t) da_str_print,
				 (da_cmp_func_t) da_str_cmp);
  res->fonts = font_table_new ();
  for (i = 0 ; i < NB_FACES ; i++)
    res->faces_wx [i] = NULL;
  for (i = 0 ; base_faces [i] != -1 ; i++)
    res->faces_wx [base_faces [i]] = wx_new ();
  return res;
}

static void
encoding_free (struct encoding * encoding)
{
  int i;

  XFREE (encoding->key);
  XFREE (encoding->name);
  XFREE (encoding->default_font);
  XFREE (encoding->documentation);

  for (i = 0 ; i < 256 ; i++)
    free (encoding->vector [i]);

  pair_table_free (encoding->substitutes);
  da_free (encoding->font_names_used, (da_map_func_t) free);
  font_table_free (encoding->fonts);

  for (i = 0 ; base_faces [i] != -1 ; i++)
    free (encoding->faces_wx [base_faces [i]]);
  free (encoding);
}

/*
 * Add a subsitution in the current encoding
 */
static void
encoding_add_font_substitute (struct encoding * encoding,
			      const char * orig, const char * subs)
{
  pair_add (encoding->substitutes, orig, subs);
}

/*
 * When FONT_NAME is used with ENCODING, return the
 * real font name to use (e.g., in latin2, Courier-Ogonki
 * should be returned when asked for Courier).
 *
 * Actually, now FONT_NAME can be a `,' separated list
 * of fonts desired, in decreasing order of preference
 */
const char *
encoding_resolve_font_substitute (struct a2ps_job * job,
				  struct encoding * encoding,
				  const char * font_list)
{
  const char * res = NULL;
  char * font_list_copy;
  char * font_name;

  astrcpy (font_list_copy, font_list);
  font_name = strtok (font_list_copy, ",<>;");

  do
    {
      /* Find if there is a substitute for that font */
      res = pair_get (encoding->substitutes, font_name);
      if (!res)
	/* No. Check if this font is supported */
	if (font_exists (job, font_name))
	  /* Avoid returning sth alloca'd */
	  res = xstrdup (font_name);
    }
  while (!res && (font_name = strtok (NULL, ",<>;")));

  /* We've found nothing interesting.  Last chance is the default
   * font */
  if (!res)
    {
      if (encoding->default_font)
	res = encoding->default_font;
      else
	error (1, 0, "Cannot find font %s, nor any substitute",
	       font_name);
    }

  message (msg_enc,
	   (stderr, "In encoding %s, font %s is resolved as %s\n",
	    encoding->key, font_name, res));
  return res;
}

/*
 * Read of *.edf file, given its path
 */
#define GET_TOKEN(from) (strtok ((from), " \t\n"))
#define GET_LINE_TOKEN(from) (strtok ((from), "\n"))
#define CHECK_TOKEN()                                                   \
  if (token2 == NULL)                                                   \
    error_at_line (1, 0, fname, firstline,				\
		   _("missing argument for `%s'"), quotearg (token));

static void
load_encoding_description_file (a2ps_job * job,
				struct encoding * encoding)
{
  FILE * stream;
  char * buf = NULL;
  char * fname;
  size_t bufsiz = 0;
  char * token, * token2;
  int firstline = 0, lastline = 0;
  static int first_time = 1;
  static struct obstack documentation_stack;

  if (first_time)
    {
      /* Initialize the obstack we use to record the docuementation
       * lines */
      first_time = 0;
      obstack_init (&documentation_stack);
    }

  message (msg_enc,
	   (stderr, "Loading encoding file `%s.edf'\n", encoding->key));
  fname = xpw_find_file (job->common.path, encoding->key, ".edf");
  stream = xrfopen (fname);

  while (getshline_numbered (&firstline, &lastline,
			     &buf, &bufsiz, stream) != -1)
    {
      token = GET_TOKEN (buf);

      if (!token)
	/* Blank line */
	continue;

      if (strequ (token, "Vector:"))
	{
	  int c = 0;
	  char buf2 [256];

	  while (c < 256)
	    {
	      if (!fgets (buf2, sizeof (buf2), stream))
		error_at_line (1, 0, fname, firstline,
			       _("missing argument for `%s'"), "`Vector:'");
	      lastline++;
	      token = GET_TOKEN (buf2);
	      encoding->vector [c++] = xstrdup (token);
	      while ((token2 = GET_TOKEN (NULL)))
		encoding->vector [c++] = xstrdup (token2);
	  }
	}
      else if (strequ (token, "Name:"))
	{
	  token2 = GET_TOKEN (NULL);
	  CHECK_TOKEN ();
	  encoding->name = xstrdup (token2);
	}
      else if (strequ (token, "Default:"))
	{
	  token2 = GET_TOKEN (NULL);
	  CHECK_TOKEN ();
	  encoding->default_font = xstrdup (token2);
	}
      else if (strequ (token, "Documentation"))
	{
	  char * documentation;
	  char buf2 [BUFSIZ];
	  size_t read_length;

	  /* We don't use getshline, because we do want the
	   * ``empty'' lines to be kept: they separate the
	   * paragraphs */
	  while (fgets (buf2, sizeof(buf2), stream)
		 && !strprefix ("EndDocumentation", buf2))
	    {
	      read_length = strlen (buf2);
	      if (read_length < sizeof (buf2))
		lastline++;
	      /* Grow the obstack with the doc content */
	      obstack_grow (&documentation_stack, buf2, read_length);
	    }
	  if (!strprefix ("EndDocumentation", buf2))
	    error (1, 0, fname, firstline,
		   _("missing argument for `%s'"), "`Documentation'");
	  /* Finish the obstack, and store in the encoding entry */
	  obstack_1grow (&documentation_stack, '\0');
	  documentation =
	    (char *) obstack_finish (&documentation_stack);
	  obstack_free (&documentation_stack, documentation);
	  encoding->documentation = xustrdup (documentation);
    	}
      else if (strequ (token, "Substitute:"))
	{
	  char * orig, * subs;

	  token2 = GET_TOKEN (NULL);
	  CHECK_TOKEN ();
	  orig = token2;
	  token2 = GET_TOKEN (NULL);
	  CHECK_TOKEN ();
	  subs = token2;
	  encoding_add_font_substitute (encoding, orig, subs);
	}
      else
        error_at_line (1, 0, fname, firstline,
                       _("invalid option `%s'"), quotearg (token));

    }
  fclose (stream);
  free (buf);
  free (fname);
}

/*
 * Used by --list-encodings
 */
static void
encoding_print_signature (struct encoding * item, FILE * stream)
{
  int i, title_len;

  title_len = (strlen (" ()")
	       + strlen (item->name)
	       + strlen (item->key));
  fprintf (stream, "%s (%s)\n", item->name, item->key);
  for (i = 0 ; i < title_len ; i++)
    putc ('-', stream);
  putc ('\n', stream);

  documentation_print_plain (item->documentation, "%s", stream);
}

/*
 * Output the correct setup of ENCODING
 */
static void
encoding_output_ps_vector (struct a2ps_job * job,
			   struct encoding * encoding)
{
  int c;

  output (job->ps_encodings, "%%%%BeginResource: encoding %sEncoding\n",
	  encoding->name);
  output (job->ps_encodings, "/%sEncoding [",
	  encoding->name);
  for (c = 0 ; c < 256 ; c++)
    {
      if (!(c % 8))
	output_char (job->ps_encodings, '\n');
      output (job->ps_encodings, "/%s ", encoding->vector [c]);
    }
  output (job->ps_encodings, "\n] def\n");
  output (job->ps_encodings, "%%%%EndResource\n");
}

/*
 * Used for very verbose reports
 */
void
encoding_self_print (struct encoding * item, FILE * stream)
{
  int c;

  encoding_print_signature (item, stream);

  if (! IS_EMPTY (item->default_font))
    fprintf (stream, "Default font is `%s'\n", item->default_font);

  fprintf (stream, "Substitution of fonts:\n");
  pair_table_list_long (item->substitutes, stream);
  fprintf (stream, "Name of fonts used (before substitution):\n");
  da_self_print (item->font_names_used, stream);

  fprintf (stream, "Encoding array:");
  for (c = 0 ; c < 256 ; c++)
    {
      if (!(c % 8))
	putc ('\n', stream);
      fprintf (stream, "%-10s ", item->vector [c]);
    }
  putc ('\n', stream);

  fprintf (stream, "Fonts:\n");
  font_table_self_print (item->fonts, stream);
  if (msg_test (msg_enc))
    {
      enum face_e face;
      int i;
      fprintf (stream, "Faces:\n");
      for (i = 0 ; base_faces [i] != -1 ; i++)
	{
	  face = base_faces [i];
	  fprintf (stream, "Face %s (%d)\n", face_to_string (face), face);
	  wx_self_print (item->faces_wx [face], stream);
	}
    }
}

/*
 * Return the name of ENC
 */
const char *
encoding_get_name (struct encoding * enc)
{
  return enc->name;
}

/*
 * Return the key of ENC
 */
const char *
encoding_get_key (struct encoding * enc)
{
  return enc->key;
}

int
encoding_char_exists (struct encoding * enc,
			    enum face_e face, uchar c)
{
  return enc->faces_wx[face][c];
}

/*
 * Prepare the environment (a dictionary) for the support
 * of ENCODING, dump it into STREAM.
 *
 * This routine is called at the end of the whole job, while
 * undiverting, so that everything we need to know is known.
 */
static void
dump_encoding_setup (FILE * stream,
		     struct a2ps_job * job,
		     struct encoding * encoding)
{
  size_t i, nb;
  const char * real_font_name;		/* After subsitution	*/
  char ** font_names = (char **) encoding->font_names_used->content;

  /* How many fonts are there? */
  da_qsort (encoding->font_names_used);
  da_unique (encoding->font_names_used, (da_map_func_t) free);

  /* We do not want to reencode the fonts that should not be
   * reencoded */
  for (i = 0 ; i < encoding->font_names_used->len ; i++)
    {
      real_font_name = encoding_resolve_font_substitute (job, encoding,
							 font_names [i]);
      if (!font_is_to_reencode (job, real_font_name))
	da_remove_at (encoding->font_names_used, i, (da_map_func_t) free);
    }

  /* The number of fonts that, finally, have to be encoded
   * in the current ENCODING	*/
  nb = encoding->font_names_used->len;

  /* Create the dictionary and fill it */
  fprintf (stream, "%% Dictionary for %s support\n",
	  encoding->name);
  fprintf (stream, "/%sdict %d dict begin\n", encoding->key, nb);
  for (i = 0 ; i < nb ; i++)
    fprintf (stream, "  /f%s %sEncoding /%s reencode_font\n",
	     font_names [i],
	     encoding->name,
	     encoding_resolve_font_substitute (job, encoding, font_names [i]));
  fputs ("currentdict end def\n", stream);
}

/************************************************************************/
/*		     encodings hash table handling			*/
/************************************************************************/
/*
 * 1. hash table service routines on cells
 */
static unsigned long
encoding_hash_1 (struct encoding * entry)
{
  return_STRING_HASH_1 (entry->key);
}

static unsigned long
encoding_hash_2 (struct encoding * entry)
{
  return_STRING_HASH_2 (entry->key);
}

static int
encoding_hash_cmp (struct encoding * x, struct encoding * y)
{
  return_STRING_COMPARE (x->key, y->key);
}

static int
encoding_hash_qcmp (struct encoding ** x,
			  struct encoding ** y)
{
  return_STRING_COMPARE ((*x)->key, (*y)->key);
}

/*
 * 2. Hash table routines
 */
struct hash_table_s *
encodings_table_new (void)
{
  struct hash_table_s * res;

  res = XMALLOC (hash_table, 1);
  hash_init (res, 32,
	     (hash_func_t) encoding_hash_1,
	     (hash_func_t) encoding_hash_2,
	     (hash_cmp_func_t) encoding_hash_cmp);
  return res;
}

/*
 * Free the table and content
 */
void
encodings_table_free (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) encoding_free);
  free (table);
}

/*
 * Create an encoding, reading its associated file.edf
 */
static void
encoding_setup (struct a2ps_job * job,
		      struct encoding * encoding)
{
  char * cp;

  load_encoding_description_file (job, encoding);
  switch (job->output_format)
    {
    case ps:
    case eps:     /* Producing PostScript */
      /* FIXME: Remove this
       * Include its encoding definition */
      /* This, too, should be handled by the pseudo PDD files */
      cp = ALLOCA (char, strlen (encoding->name) + strlen ("Encoding") + 1);
      sprintf (cp, "%sEncoding", encoding->name);

      add_supplied_resource (job, "encoding", cp);
      encoding_output_ps_vector (job, encoding);
      break;
    }
}

/*
 * Add an encoding in the hash table,
 * Reading its associated file.edf
 */
static void
encoding_add (struct a2ps_job * job, const char * key)
{
  struct encoding * encoding;

  encoding = encoding_new (key);
  encoding_setup (job, encoding);

  /* If yet the association between faces and fonts is known,
   * get the WX per face */
  if (face_eo_font_is_set (job))
    encoding_build_faces_wx (job, encoding);
  if (msg_test (msg_enc))
    encoding_self_print (encoding, stderr);
  hash_insert (job->encodings, encoding);
}

void
encoding_add_font_name_used (struct encoding * encoding,
				   const char * name)
{
  da_append (encoding->font_names_used, xstrdup (name));
}

/*
 * Make an encoding do the association between the fonts
 * and the faces, so that the faces can be immediately used when
 * looking for the wx.
 *
 * The resolution of substitution for the fonts must be done here too,
 * because it depends on the encoding.
 */
void
encoding_build_faces_wx (a2ps_job * job, struct encoding * encoding)
{
  int i;
  enum face_e face;
  const char * true_font_name;

  for (i = 0 ; base_faces [i] != -1 ; i++)
    {
      face = base_faces [i];
      encoding_add_font_name_used (encoding,
				   job->face_eo_font [face]);
      /* E.g. in Latin 2, don't use Courier but Courier-Ogonki */
      true_font_name =
	encoding_resolve_font_substitute (job, encoding,
					  job->face_eo_font [face]);
      /* Get the wx related to the FACE */
      font_info_get_wx_for_vector (job,
				   true_font_name,
				   encoding->vector,
				   encoding->faces_wx [face]);
    }
}

/*
 * Retreive an encoding
 */
static struct encoding *
encoding_get (struct a2ps_job * job, const char * key)
{
  struct encoding token, *res;

  token.key = (char *) key;
  res = (struct encoding *) hash_find_item (job->encodings, &token);

  if (!res)
    {	/* The encoding has never been read yet */
      encoding_add (job, key);
      res = (struct encoding *) hash_find_item (job->encodings, &token);
    }
  return res;
}

/*
 * Return the encoding and NULL if none
 */
struct encoding *
get_encoding_by_alias (struct a2ps_job * job, char * alias)
{
  const char * key;

  key = encodings_map_resolve_alias (job->encodings_map, strlower (alias));
  if (key)
    return encoding_get (job, key);
  else
    /* This is not a valid alias */
    return NULL;
}

/*
 * Prepare all the encodings used.  This is easy: there
 * are all the members of JOB->encodings
 */
void
dump_encodings_setup (FILE * stream,
		      struct a2ps_job * job)
{
  int i;
  struct encoding ** encodings;

  /* Get the list of the encodings */
  encodings = (struct encoding **)
    hash_dump (job->encodings, NULL,
	       (hash_cmp_func_t) encoding_hash_qcmp);

  for (i = 0 ; encodings [i] ; i++)
    dump_encoding_setup (stream, job, encodings [i]);

  free (encodings);
}

/************************************************************************/
/*		     Computing the width of a char/string		*/
/************************************************************************/
/*
 *	Returns the WX of a char (including M- and ^)
 */
unsigned int
char_WX (a2ps_job * job, uchar c)
{
#define _WX_(char)  (job->encoding->faces_wx[job->status->face][(int) char])
  unsigned int wx = _WX_(c);

  /* Only printable characters have a positive wx */
  if (wx)
    return wx;

  switch (job->unprintable_format)
    {
    case hexa:
      {
	char buf [3];
	sprintf (buf, "%02x", c);
	return _WX_ ('\\') + _WX_('x') + _WX_ (buf[0]) + _WX_ (buf[1]);
      }

    case octal:
      {
	char buf [4];
	sprintf (buf, "%03o", c);
	return (_WX_ ('\\')
		+ _WX_ (buf [0]) + _WX_ (buf [1]) + _WX_ (buf [2]));
      }

    case space:
      return _WX_(' ');

    case question_mark:
      return _WX_('?');

    case caret:
      if (0177 < c)
	{
	  wx += _WX_('M') + _WX_('-');
	  c &= 0177;
	}
      if (c == 0177)
	wx += _WX_('^') + _WX_('?');
      else if (c < ' ')
	wx += _WX_ ('^') + _WX_ (c + '@');
      else
	wx += _WX_ (c);
      return wx;

    case Emacs:
      if (0177 < c)
	{
	  wx += _WX_('M') + _WX_('-');
	  c &= 0177;
	}
      if (c == 0177)
	wx += _WX_('C') + _WX_('-') + _WX_('?');
      else if (c < ' ')
	wx += _WX_('C') + _WX_('-') + _WX_ (c + '@');
      else
	wx += _WX_ (c);
      return wx;

    default:
      abort ();
    }

  return 0;	/* For -Wall */
}

/*
 *	Returns the WX of a string (including M- and ^)
 */
unsigned int
string_WX (a2ps_job * job, uchar * string)
{
  unsigned int result=0;

  for (/* skip */; *string ; string ++)
    result += char_WX(job, *string);

  return result;
}

/*
 * Use this encoding now
 */
void
set_encoding (struct a2ps_job * job, struct encoding * enc)
{
  job->encoding = enc;
}

/* Compare names with strverscmp, so that latin 15 is after latin 2.  */

static int
da_encoding_name_cmp (char * key1, char * key2, struct a2ps_job * job)
{
  return strverscmp ((char *) encoding_get (job, key1)->name,
		     (char *) encoding_get (job, key2)->name);
}

/*
 * Report the known encodings for --list-features
 */
void
list_encodings_short (a2ps_job * job, FILE * stream)
{
  fputs (_("Known Encodings"), stream);
  putc ('\n', stream);
  pw_lister_on_suffix (stream, job->common.path, EDF_SUFFIX);
}

/*
 * Report the known encodings for --list-encodings
 */
void
list_encodings_long (a2ps_job * job, FILE * stream)
{
  struct darray * entries;
  struct encoding * encoding;
  size_t i;

  entries = pw_glob_on_suffix (job->common.path, EDF_SUFFIX);

  /* We want them to be in order of the names, not keys */
  da_qsort_with_arg (entries,
		     (da_cmp_arg_func_t) da_encoding_name_cmp, job);

  fputs (_("Known Encodings"), stream);
  putc ('\n', stream);

  for (i = 0 ; i < entries->len ; i++)
    {
      /* Don't forget to cut the suffix */
      encoding = encoding_get (job, entries->content[i]);
      encoding_print_signature (encoding, stream);
      putc ('\n', stream);
    }
  da_free (entries, (da_map_func_t) free);
}
/************************************************************************/
/*		Report in Texinfo format				*/
/************************************************************************/
/*
 * Print a short signature (i.e., name, comments)
 * In texinfo format
 */
static void
encoding_texinfo_print_signature (struct encoding * encoding,
					FILE * stream)
{
  fprintf (stream, "@deftp {Encoding} {%s} (@file{%s.edf})\n",
	   encoding->name, encoding->key);

  documentation_print_texinfo (encoding->documentation, "%s", stream);
  fputs ("@end deftp\n\n", stream);
}

/*
 * Report the known encodings for --list-encodings
 */
void
list_texinfo_encodings_long (a2ps_job * job, FILE * stream)
{
  struct darray * entries;
  struct encoding * encoding;
  unsigned int i;

  entries = pw_glob_on_suffix (job->common.path, EDF_SUFFIX);

  /* We want them to be in order of the names, not keys */
  da_qsort_with_arg (entries,
		     (da_cmp_arg_func_t) da_encoding_name_cmp, job);

  fputs ("The known encodings are:\n", stream);

  for (i = 0 ; i < entries->len ; i++)
    {
      /* Don't forget to cut the suffix */
      encoding = encoding_get (job, entries->content[i]);
      encoding_texinfo_print_signature (encoding, stream);
    }
  da_free (entries, (da_map_func_t) free);
}
