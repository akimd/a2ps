/*
 * str_ht.c
 *
 * Single (char *) hash table
 *
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
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
 * $Id: str_ht.c,v 1.1.1.1.2.1 2007/12/29 01:58:23 mhatta Exp $
 */

#include "system.h"

/* Hack! */
#define hash_table_s string_htable
#include "str_ht.h"
#include "hashtab.h"

/************************************************************************
 * hash tables with one char * field					*
 ************************************************************************/
/*
 * Basic routines
 */
static unsigned long
string_hash_1 (char *string)
{
  return_STRING_HASH_1 (string);
}

static unsigned long
string_hash_2 (char *string)
{
  return_STRING_HASH_2 (string);
}

static int
string_hash_cmp (char *x, char *y)
{
  return_STRING_COMPARE (x, y);
}

/*
 * For sorting them in alpha order
 */
static int
string_hash_qcmp (char **x, char **y)
{
  return_STRING_COMPARE (*x, *y);
}

/*
 * Create the structure that stores the list of strings
 */
struct string_htable *
string_htable_new (void)
{
  struct string_htable * res;
  
  res = XMALLOC (struct string_htable, 1);
  hash_init (res, 8,
	     (hash_func_t) string_hash_1,
	     (hash_func_t) string_hash_2,
	     (hash_cmp_func_t) string_hash_cmp);
  return res;
}

/*
 * Free the whole structure
 */
void
string_htable_free (struct string_htable * table)
{
  hash_free (table, (hash_map_func_t) free);
  free (table);
}

/*
 *  Add a string, with your own allocation for them.
 */
void
string_htable_add (struct string_htable * table, const char * key)
{
  if (!hash_find_item (table, key))
    hash_insert (table, xstrdup(key));
}

/*
 * Get the value associated to KEY in TABLE
 * Return NULL upon error (this means that it is not
 * valid to enter NULL as a value)
 */
char *
string_htable_get (struct string_htable * table, const char * key)
{
  return (char *) hash_find_item (table, key);
}

/*
 * Mostly for debbuging
 */
void
string_htable_self_print (struct string_htable * table, FILE * stream)
{
  int i;
  char ** entries;
  entries = (char **) 
    hash_dump (table, NULL,
	       (hash_cmp_func_t) string_hash_qcmp);

  for (i = 0 ; entries[i] ; i++)
    fprintf (stream, "%s\n", entries[i]);

  putc ('\n', stream);
  free (entries);
}

/*
 * Dump in a vector
 */
char **
string_htable_dump_sorted (struct string_htable * table)
{
  return (char **) hash_dump (table, NULL, (qsort_cmp_t) string_hash_qcmp);
}
