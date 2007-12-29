/*
 * media.c
 *
 * Used paper formats
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
 * $Id: media.c,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
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
#include "a2ps.h"
#include "media.h"
#include "jobs.h"
#include "routines.h"
#include "lister.h"
#include "quotearg.h"

#if HAVE_SYSTEMPAPERNAME
#  if HAVE_PAPER_H
#    include <paper.h>
#  else
extern char *systempapername PARAMS ((void));
#  endif
#else
#define systempapername() NULL;
#endif

/************************************************************************/
/*				medium selection			*/
/************************************************************************/
/************************************************************************
 * Hash tables routines for the media					*
 ************************************************************************/
/*
 * Used with the page device(-D), and status dict (-S) definitions
 */
static unsigned long
medium_hash_1 (void const *key)
{
  return_STRING_CASE_HASH_1 (((const struct medium *)key)->name);
}

static unsigned long
medium_hash_2 (void const *key)
{
  return_STRING_CASE_HASH_2 (((const struct medium *)key)->name);
}

static int
medium_hash_cmp (void const *x, void const *y)
{
  return_STRING_CASE_COMPARE (((const struct medium *)x)->name,
			 ((const struct medium *)y)->name);
}

static int
medium_hash_qcmp (void const *x, void const *y)
{
  return_STRING_CASE_COMPARE ((*(struct medium * const *)x)->name,
			      (*(struct medium * const *)y)->name);
}

/* Return the size of the name of the MEDIUM */

static size_t
medium_name_len (struct medium * medium)
{
  return strlen (medium->name);
}

/* Fputs the name of the MEDIUM on STREAM */

static void
medium_name_fputs (struct medium * medium, FILE * stream)
{
  fputs (medium->name, stream);
}


/*
 * Add a medium
 */
void
add_medium (struct a2ps_job * job,
	   const char * name,
	   int w, int h,
	   int llx, int lly, int urx, int ury)
{
  struct medium * a_medium = XMALLOC (struct medium, 1);

  a_medium->name = xstrdup (name);
  a_medium->w = w;
  a_medium->h = h;
  a_medium->llx = llx;
  a_medium->lly = lly;
  a_medium->urx = urx;
  a_medium->ury = ury;

  hash_insert (job->media, a_medium);
}

/*
 * Free a medium
 */
static void
hash_free_medium (struct medium * medium)
{
  free (medium->name);
  free (medium);
}

/*
 * Create the structure
 */
struct hash_table_s *
new_medium_table (void)
{
  struct hash_table_s * res;

  res = XMALLOC (struct hash_table_s, 1);
  hash_init (res, 16,
	     medium_hash_1, medium_hash_2, medium_hash_cmp);

  return res;
}

/*
 * Free the media table
 */
void
free_medium_table (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) hash_free_medium);
  free (table);
}

/*
 * Retrieve a medium
 */
struct medium *
a2ps_get_medium (a2ps_job * job, const char * name)
{
  struct medium *item;
  struct medium token;

  token.name = NULL;
  if (strcaseequ (name, LIBPAPER_MEDIUM))
    token.name = (char *) systempapername ();

  if (!token.name)
    token.name = (char *) name;

  item = (struct medium *) hash_find_item (job->media, &token);

  if (item == NULL)
    error (1, 0, _("unknown medium `%s'"), quotearg (token.name));

  return item;
}

/* Report the name of the media NAME, while decoding, if needed,
   the use of libpaper.  */
const char *
a2ps_get_medium_name (a2ps_job * job, const char * name)
{
  struct medium * medium;

  /* We use the real name of the medium: that stored in the table,
     so that we get the correct case (Letter, instead of LeTTer).  */
  medium = a2ps_get_medium (job, name);
  return medium->name;
}

/* Is libpaper used? */
bool
a2ps_medium_libpaper_p (PARAM_UNUSED a2ps_job * job, const char * name)
{
  return strequ (name, LIBPAPER_MEDIUM) ? true : false;
}

/*
 * List the media defined, with their dimensions.
 */
void
list_media_long (a2ps_job * job, FILE * stream)
{
  int i;
  struct medium ** media;
  media = (struct medium **) hash_dump (job->media, NULL, medium_hash_qcmp);

  fputs (_("Known Media"), stream);
  putc ('\n', stream);
  fprintf (stream, "  %-10s\t%11s (%4s, %4s, %4s, %4s)\n",
	   _("Name"),
	   _("dimensions"),
	   "llx", "lly",
	   "urx", "ury");

  for (i = 0 ; media[i] ; i++)
    fprintf (stream, "  %-10s\t%4d x %4d (%4d, %4d, %4d, %4d)\n",
	     media[i]->name,
	     media[i]->w,   media[i]->h,
	     media[i]->llx, media[i]->lly,
	     media[i]->urx, media[i]->ury);
  putc ('\n', stream);

  free (media);
}

/*
 * Give just the names of the known media
 */
void
list_media_short (a2ps_job * job, FILE * stream)
{
  struct medium ** media;

  media = (struct medium **) hash_dump (job->media, NULL, medium_hash_qcmp);

  fputs (_("Known Media"), stream);
  putc ('\n', stream);
  lister_fprint_vertical (NULL, stream,
			  (void **) media, (size_t) -1,
			  (lister_width_t) medium_name_len,
			  (lister_print_t) medium_name_fputs);

  free (media);
}
