/* hash.c -- hash table maintenance
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Greg McGary <gkm@gnu.ai.mit.edu>

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <stdio.h>

#if defined STDC_HEADERS || defined _LIBC || defined HAVE_STDLIB_H
# include <stdlib.h>
#endif

#include "hashtab.h"
#include "xalloc.h"
#include "error.h"

static void hash_rehash PARAMS((struct hash_table_s* ht));
static unsigned long round_up_2 PARAMS((unsigned long rough));

/* Implement double hashing with open addressing.  The table size is
   always a power of two.  The secondary (`increment') hash function
   is forced to return an odd-value, in order to be relatively prime
   to the table size.  This guarantees that the increment can
   potentially hit every slot in the table during collision
   resolution.  */

void *hash_deleted_item = &hash_deleted_item;

/* Force the table size to be a power of two, possibly rounding up the
   given size.  */

void
hash_init (struct hash_table_s* ht, unsigned long size,
	   hash_func_t hash_1, hash_func_t hash_2, hash_cmp_func_t hash_cmp)
{
  ht->ht_size = round_up_2 (size);
  if (ht->ht_size > (128 * 1024)) /* prevent size from getting out of hand */
    ht->ht_size /= 2;
  ht->ht_vec = (void**) XCALLOC (struct token *, ht->ht_size);
  ht->ht_capacity = ht->ht_size * 15 / 16; /* 93.75% loading factor */
  ht->ht_fill = 0;
  ht->ht_collisions = 0;
  ht->ht_lookups = 0;
  ht->ht_rehashes = 0;
  ht->ht_hash_1 = hash_1;
  ht->ht_hash_2 = hash_2;
  ht->ht_compare = hash_cmp;
}

/* Load an array of items into `ht'.  */

void
hash_load (struct hash_table_s* ht, void *item_table, unsigned long cardinality, unsigned long size)
{
  char *items = (char *) item_table;
  while (cardinality--)
    {
      hash_insert (ht, items);
      items += size;
    }
}

/* Returns the address of the table slot matching `key'.  If `key' is
   not found, return the address of an empty slot suitable for
   inserting `key'.  The caller is responsible for incrementing
   ht_fill on insertion.  */

void **
hash_find_slot (struct hash_table_s* ht, void const *key)
{
  void **slot;
  void **deleted_slot = 0;
  unsigned int hash_2 = 0;
  unsigned int hash_1 = (*ht->ht_hash_1) (key);

  ht->ht_lookups++;
  for (;;)
    {
      hash_1 %= ht->ht_size;
      slot = &ht->ht_vec[hash_1];

      if (*slot == 0)
	return deleted_slot ? deleted_slot : slot;
      if (*slot == hash_deleted_item)
	{
	  if (deleted_slot == 0)
	    deleted_slot = slot;
	}
      else
	{
	  if (key == *slot)
	    return slot;
	  if ((*ht->ht_compare) (key, *slot) == 0)
	    return slot;
	  ht->ht_collisions++;
	}
      if (!hash_2)
	  hash_2 = (*ht->ht_hash_2) (key) | 1;
      hash_1 += hash_2;
    }
}

void *
hash_find_item (struct hash_table_s* ht, void const *key)
{
  void **slot = hash_find_slot (ht, key);
  return ((HASH_VACANT (*slot)) ? 0 : *slot);
}

void *
hash_insert (struct hash_table_s* ht, void *item)
{
  void **slot = hash_find_slot (ht, item);
  return hash_insert_at (ht, item, slot);
}

void *
hash_insert_at (struct hash_table_s* ht, void *item, void const *slot)
{
  void *old_item = *(void **) slot;
  if (HASH_VACANT (old_item))
    {
      ht->ht_fill++;
      old_item = item;
    }
  *(void const **) slot = item;
  if (ht->ht_fill >= ht->ht_capacity)
    hash_rehash (ht);
  return old_item;
}

void *
hash_delete (struct hash_table_s* ht, void const *item)
{
  void **slot = hash_find_slot (ht, item);
  return hash_delete_at (ht, slot);
}

void *
hash_delete_at (struct hash_table_s* ht, void const *slot)
{
  void *item = *(void **) slot;
  if (!HASH_VACANT (item))
    {
      *(void const **) slot = hash_deleted_item;
      ht->ht_fill--;
      return item;
    }
  else
    return 0;
}

void
hash_free_items (struct hash_table_s* ht, hash_map_func_t free_fn)
{
  void **vec = ht->ht_vec;
  void **end = &vec[ht->ht_size];
  for (; vec < end; vec++)
    {
      void *item = *vec;
      if (!HASH_VACANT (item))
	(*free_fn) (item);
      *vec = 0;
    }
  ht->ht_fill = 0;
}

void
hash_delete_items (struct hash_table_s* ht)
{
  void **vec = ht->ht_vec;
  void **end = &vec[ht->ht_size];
  for (; vec < end; vec++)
    *vec = 0;
  ht->ht_fill = 0;
  ht->ht_collisions = 0;
  ht->ht_lookups = 0;
  ht->ht_rehashes = 0;
}

void
hash_free (struct hash_table_s* ht, hash_map_func_t free_fn)
{
  if (free_fn)
    hash_free_items (ht, free_fn);
  free (ht->ht_vec);
  ht->ht_vec = 0;
  ht->ht_fill = 0;
  ht->ht_capacity = 0;
}

void
hash_map (struct hash_table_s *ht, hash_map_func_t map)
{
  void **slot;
  void **end = &ht->ht_vec[ht->ht_size];

  for (slot = ht->ht_vec; slot < end; slot++)
    {
      if (!HASH_VACANT (*slot))
	(*map) (*slot);
    }
}

/* Double the size of the hash table in the event of overflow... */

static void
hash_rehash (struct hash_table_s* ht)
{
  unsigned long old_ht_size = ht->ht_size;
  void **old_vec = ht->ht_vec;
  void **ovp;
  void **slot;

  ht->ht_size *= 2;
  ht->ht_rehashes++;
  ht->ht_capacity = ht->ht_size - (ht->ht_size >> 4);
  ht->ht_vec = (void **) XCALLOC (struct token *, ht->ht_size);

  for (ovp = old_vec; ovp < &old_vec[old_ht_size]; ovp++)
    {
      if (*ovp == 0)
	continue;
      slot = hash_find_slot (ht, *ovp);
      *slot = *ovp;
    }
  free (old_vec);
}

void
hash_print_stats (struct hash_table_s *ht, FILE *out_FILE)
{
  fprintf (out_FILE, "Load=%ld/%ld=%.0f%%, ", ht->ht_fill, ht->ht_size,
	   100.0 * (double) ht->ht_fill / (double) ht->ht_size);
  fprintf (out_FILE, "Rehash=%d, ", ht->ht_rehashes);
  fprintf (out_FILE, "Collisions=%ld/%ld=%.0f%%\n",
	   ht->ht_collisions, ht->ht_lookups,
	   (ht->ht_lookups
	    ? (100.0 * (double) ht->ht_collisions / (double) ht->ht_lookups)
	    : 0));
}

/* Dump all items into a NULL-terminated vector.  Use the
   user-supplied vector, or malloc one.  */

void**
hash_dump (struct hash_table_s *ht, void **vector_0, qsort_cmp_t compare)
{
  void **vector;
  void **slot;
  void **end = &ht->ht_vec[ht->ht_size];

  if (vector_0 == 0)
    vector_0 = XMALLOC (void *, ht->ht_fill + 1);
  vector = vector_0;

  for (slot = ht->ht_vec; slot < end; slot++)
    if (!HASH_VACANT (*slot))
      *vector++ = *slot;
  *vector = 0;

  if (compare)
    qsort (vector_0, ht->ht_fill, sizeof (void *), compare);
  return vector_0;
}

/* Dump all items on which SELECT returns non 0, into a
   NULL-terminated vector.  Use the user-supplied vector, or malloc
   one.  */

void**
hash_dump_select (struct hash_table_s *ht, void **vector_0,
		  hash_select_func_t select_fn, qsort_cmp_t compare)
{
  void **slot;
  void **end = &ht->ht_vec[ht->ht_size];
  int vector_0_malloced = 0;
  size_t fill = 0;	/* Number of items in the result */

  if (!select_fn)
    return hash_dump (ht, vector_0, compare);

  if (vector_0 == 0)
    {
      vector_0 = XMALLOC (void *, ht->ht_fill + 1);
      vector_0_malloced = 1;
    }

  for (slot = ht->ht_vec; slot < end; slot++)
    if (!HASH_VACANT (*slot)
	&& (*select_fn) (*slot))
      vector_0 [fill++] = *slot;
  vector_0 [fill] = 0;

  if (vector_0_malloced)
    vector_0 = XREALLOC (vector_0, void *, fill + 1);

  if (compare && fill > 1)
    qsort (vector_0, fill, sizeof (void *), compare);
  return vector_0;
}

/* Call OUT_FN on all items (with extra ARG), sorted by COMPARE. */

void
hash_maparg (struct hash_table_s *ht, hash_maparg_func_t map,
	     void const * arg, qsort_cmp_t compare)
{
  void **vector, **vector_0;

  vector_0 = hash_dump (ht, NULL, compare);

  for (vector = vector_0 ; *vector ; vector++)
    (*map) (*vector, arg);

  free (vector_0);
}

/* Round a given number up to the nearest power of 2. */

static unsigned long
round_up_2 (unsigned long rough)
{
  int round;

  round = 1;
  while (rough)
    {
      round <<= 1;
      rough >>= 1;
    }
  return round;
}
