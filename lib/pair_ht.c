/*
 * hasstr.c
 *
 * 2 char * hash tables
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
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
 * $Id: pair_ht.c,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
 */
#include "a2ps.h"

/* Hack! */
#define hash_table_s pair_htable

#include "hashtab.h"
#include "message.h"
#include "pair_ht.h"
#include "getshline.h"
#include "lister.h"
#include "quotearg.h"
#include "routines.h"

/************************************************************************
 * hash tables with two char * fields					*
 ************************************************************************/
/* Definition of the hash structure */
struct pair
{
  char * key;
  char * value;
};

/*
 * Basic routines
 */
static unsigned long
pair_hash_1 (struct pair *pair)
{
  return_STRING_HASH_1 (pair->key);
}

static unsigned long
pair_hash_2 (struct pair *pair)
{
  return_STRING_HASH_2 (pair->key);
}

static int
pair_hash_cmp (struct pair *x, struct pair *y)
{
  return_STRING_COMPARE (x->key, y->key);
}

/*
 * For sorting them in alpha order
 */
static int
pair_hash_qcmp (struct pair **x, struct pair **y)
{
  return_STRING_COMPARE ((*x)->key, (*y)->key);
}

/*
 * Free a pair, and its content
 */
static void
pair_free (struct pair * pair)
{
  XFREE (pair->key);
  XFREE (pair->value);
  XFREE (pair);
}

/* Return the length of the key of PAIR */

static size_t
pair_key_len (struct pair * pair)
{
  return strlen (pair->key);
}

/* Fputs the key of PAIR to STREAM */
static void
pair_key_fputs (struct pair * pair, FILE * stream)
{
  fputs (pair->key, stream);
}



/*
 * Create the structure that stores the list of pairs
 */
struct hash_table_s *
pair_table_new (void)
{
  struct hash_table_s * res;

  res = XMALLOC (struct hash_table_s, 1);
  hash_init (res, 8,
	     (hash_func_t) pair_hash_1,
	     (hash_func_t) pair_hash_2,
	     (hash_cmp_func_t) pair_hash_cmp);
  return res;
}

/*
 * Free the whole structure
 */
void
pair_table_free (struct hash_table_s * table)
{
  hash_free (table, (hash_map_func_t) pair_free);
  free (table);
}

/*
 *  Add a pair, with your own allocation for them.
 * It KEY is yet used, override its value with VALUE
 */
void
pair_add (struct hash_table_s * table,
	  const char * key, const char * value)
{
  struct pair * item, token;

  token.key = (char *) key;
  item = (struct pair *) hash_find_item (table, &token);

  if (item) {
    if (item->value)
      free (item->value);
  } else {
    item = XMALLOC (struct pair, 1);
    item->key = xstrdup(key);
  }

  if (value)
    item->value = xstrdup (value);
  else
    item->value = NULL;

  hash_insert (table, item);
}

/*
 * Remove a pair and free it.
 * It KEY is yet used, override its value with VALUE
 */
void
pair_delete (struct hash_table_s * table, const char * key)
{
  struct pair * item, token;

  token.key = (char *) key;
  item = (struct pair *) hash_find_item (table, &token);

  if (item) {
    hash_delete (table, item);
    pair_free (item);
  }
}

/*
 * Get the value associated to KEY in TABLE
 * Return NULL upon error (this means that it is not
 * valid to enter NULL as a value)
 */
char *
pair_get (struct hash_table_s * table, const char * key)
{
  struct pair * item, token;

  token.key = (char *) key;
  item = (struct pair *) hash_find_item (table, &token);

  if (item)
    return item->value;
  else
    return NULL;
}

/*
 * Return the content of the hash table, ordered
 */
void
pair_table_map (struct hash_table_s * table,
		pair_ht_map_fn_t map_fn,
		pair_ht_select_fn_t select_fn,
		void const * arg)
{
  int i, num = 0;
  struct pair ** entries;
  entries = (struct pair **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) pair_hash_qcmp);

  for (i = 0 ; entries[i] ; i++) {
    if (!select_fn
	|| select_fn (entries[i]-> key, entries[i]->value))
      {
	map_fn (num, entries[i]-> key, entries[i]->value, arg);
	num++;
      }
  }

  free (entries);
}

/*
 * For typically for --list-features
 */
void
pair_table_list_short (struct hash_table_s * table, FILE * stream)
{
  struct pair ** entries;
  entries = (struct pair **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) pair_hash_qcmp);

  lister_fprint_vertical (NULL, stream,
			  (void **) entries, (size_t) -1,
			  (lister_width_t) pair_key_len,
			  (lister_print_t) pair_key_fputs);
  free (entries);
}

/*
 * For typically for --list-<something>
 */
void
pair_table_list_long (struct hash_table_s * table, FILE * stream)
{
  int i;
  struct pair ** entries;
  entries = (struct pair **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) pair_hash_qcmp);

  for (i = 0 ; entries[i] ; i++)
    fprintf (stream, "%-16s = %s\n",
	     entries[i]->key,
	     entries[i]->value ? entries[i]->value : "<NULL>");

  putc ('\n', stream);
  free (entries);
}

/*
 * Mostly for debbuging
 */
void
pair_table_self_print (struct hash_table_s * table, FILE * stream)
{
  int i;
  struct pair ** entries;
  entries = (struct pair **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) pair_hash_qcmp);

  for (i = 0 ; entries[i] ; i++)
    fprintf (stream, "%s:%s\n",
	     entries[i]->key,
	     entries[i]->value ? entries[i]->value : "<NULL>");

  putc ('\n', stream);
  free (entries);
}

#define GET_TOKEN(from) (strtok ((from), " \t\n"))
#define GET_LINE_TOKEN(from) (strtok ((from), "\n"))
#define CHECK_TOKEN() 							\
  if (token2 == NULL) 							\
    error_at_line (1, 0, file, firstline, 				\
		   _("missing argument for `%s'"), quotearg (token));

/*
 * Read from a FILE
 */
int
pair_table_load (struct hash_table_s * table, const char *file)
{
  FILE * fp;
  char *buf = NULL;
  size_t bufsiz = 0;
  char * token, * token2;
  int firstline = 0, lastline = 0;

  message (msg_file,
	   (stderr, "Loading map file `%s'\n", quotearg (file)));
  fp = xrfopen (file);

  while (getshline_numbered (&firstline, &lastline, &buf, &bufsiz, fp) != -1)
    {
      token = GET_TOKEN (buf);

      if (!token)
	/* Blank but not empty */
	continue;

      if (strequ (token, "***"))
	{
	  /* Load another map file */
	  token2 = GET_TOKEN (NULL);	/* A map file path	*/
	  CHECK_TOKEN ();
	  pair_table_load (table, token2);
	}
      else
	{
	  token2 = GET_TOKEN (NULL);	/* key		*/
	  CHECK_TOKEN ();
	  pair_add (table, token, token2);
	}
    }

  free (buf);
  fclose (fp);
  return 1;
}
