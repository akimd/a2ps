/* hash.h -- decls for hash table
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/*
 * This version has been modified by Akim Demaille <demaille@inf.enst.fr>
 * so that
 * - hash_free_item takes as argument the free function to be applied
 *   to the items.
 * - hash_free takes a function pointer supposed to be the free
 *   function to be used, instead of an integer which was saying if
 *   free should have been called or not.
 * - hash_out made to print a hash_table.
 */

#ifndef HASHTAB_H_
# define HASHTAB_H_

# ifndef PARAMS
#  if defined (__GNUC__) || (defined (__STDC__) && __STDC__)
#   define PARAMS(args) args
#  else
#   define PARAMS(args) ()
#  endif  /* GCC.  */
# endif  /* Not PARAMS.  */

typedef unsigned long (*hash_func_t) PARAMS((void const *key));
typedef int (*hash_cmp_func_t) PARAMS((void const *x, void const *y));
typedef int (*hash_select_func_t) PARAMS ((void const *item));
typedef void (*hash_map_func_t) PARAMS((void const *item));
typedef void (*hash_maparg_func_t) PARAMS((void const *item, void const * arg));

struct hash_table_s
{
  void **ht_vec;
  unsigned long ht_size;	/* total number of slots (power of 2) */
  unsigned long ht_capacity;	/* usable slots, limited by loading-factor */
  unsigned long ht_fill;	/* items in table */
  unsigned long ht_collisions;	/* # of failed calls to comparison function */
  unsigned long ht_lookups;	/* # of queries */
  unsigned int ht_rehashes;	/* # of times we've expanded table */
  hash_func_t ht_hash_1;	/* primary hash function */
  hash_func_t ht_hash_2;	/* secondary hash function */
  hash_cmp_func_t ht_compare;	/* comparison function */
};

typedef int (*qsort_cmp_t) PARAMS((void const *, void const *));

void hash_init PARAMS((struct hash_table_s *ht, unsigned long size,
		    hash_func_t hash_1, hash_func_t hash_2, hash_cmp_func_t hash_cmp));
void hash_load PARAMS((struct hash_table_s *ht, void *item_table,
		    unsigned long cardinality, unsigned long size));
void **hash_find_slot PARAMS((struct hash_table_s *ht, void const *key));
void *hash_find_item PARAMS((struct hash_table_s *ht, void const *key));
void *hash_insert PARAMS((struct hash_table_s *ht, void *item));
void *hash_insert_at PARAMS((struct hash_table_s *ht, void *item, void const *slot));
void *hash_delete PARAMS((struct hash_table_s *ht, void const *item));
void *hash_delete_at PARAMS((struct hash_table_s *ht, void const *slot));
void hash_delete_items PARAMS((struct hash_table_s *ht));
void hash_free_items PARAMS((struct hash_table_s *ht, hash_map_func_t free_fn));
void hash_free PARAMS((struct hash_table_s *ht, hash_map_func_t free_fn));
void hash_map PARAMS((struct hash_table_s *ht, hash_map_func_t map));
void hash_maparg PARAMS((struct hash_table_s *ht, hash_maparg_func_t map,
		      void const *arg, qsort_cmp_t compare));
void hash_print_stats PARAMS((struct hash_table_s *ht, FILE *out_FILE));
void **hash_dump PARAMS((struct hash_table_s *ht, void **vector_0, qsort_cmp_t compare));
void **hash_dump_select PARAMS((struct hash_table_s *ht, void **vector_0,
				hash_select_func_t select_fn, qsort_cmp_t compare));

extern void *hash_deleted_item;
# define HASH_VACANT(item) ((item) == 0 || (void *) (item) == hash_deleted_item)


/* hash and comparison macros for string keys. */

# define STRING_HASH_1(_key_, _result_) do { \
  unsigned char const *kk = (unsigned char const *) (_key_) - 1; \
  while (*++kk) \
    (_result_) += (*kk << (kk[1] & 0xf)); \
} while (0)
# define return_STRING_HASH_1(_key_) do { \
  unsigned long result = 0; \
  STRING_HASH_1 ((_key_), result); \
  return result; \
} while (0)

# define STRING_HASH_2(_key_, _result_) do { \
  unsigned char const *kk = (unsigned char const *) (_key_) - 1; \
  while (*++kk) \
    (_result_) += (*kk << (kk[1] & 0x7)); \
} while (0)
# define return_STRING_HASH_2(_key_) do { \
  unsigned long result = 0; \
  STRING_HASH_2 ((_key_), result); \
  return result; \
} while (0)

# define STRING_COMPARE(_x_, _y_, _result_) do { \
  unsigned char const *xx = (unsigned char const *) (_x_) - 1; \
  unsigned char const *yy = (unsigned char const *) (_y_) - 1; \
  do { \
    if (*++xx == '\0') { \
      yy++; \
      break; \
    } \
  } while (*xx == *++yy); \
  (_result_) = *xx - *yy; \
} while (0)
# define return_STRING_COMPARE(_x_, _y_) do { \
  int result; \
  STRING_COMPARE (_x_, _y_, result); \
  return result; \
} while (0)

/* hash and comparison macros for string keys, case insensitive. */

# define STRING_CASE_HASH_1(_key_, _result_) do { \
  unsigned char const *kk = (unsigned char const *) (_key_) - 1; \
  while (*++kk) \
    (_result_) += (tolower (*kk) << (tolower (kk[1]) & 0xf)); \
} while (0)
# define return_STRING_CASE_HASH_1(_key_) do { \
  unsigned long result = 0; \
  STRING_CASE_HASH_1 ((_key_), result); \
  return result; \
} while (0)

# define STRING_CASE_HASH_2(_key_, _result_) do { \
  unsigned char const *kk = (unsigned char const *) (_key_) - 1; \
  while (*++kk) \
    (_result_) += (tolower(*kk) << (tolower(kk[1]) & 0x7)); \
} while (0)
# define return_STRING_CASE_HASH_2(_key_) do { \
  unsigned long result = 0; \
  STRING_HASH_2 ((_key_), result); \
  return result; \
} while (0)

# define STRING_CASE_COMPARE(_x_, _y_, _result_) do { \
  unsigned char const *xx = (unsigned char const *) (_x_) - 1; \
  unsigned char const *yy = (unsigned char const *) (_y_) - 1; \
  do { \
    if (*++xx == '\0') { \
      yy++; \
      break; \
    } \
    yy++; \
  } while (tolower (*xx) == tolower (*yy)); \
  (_result_) = tolower(*xx) - tolower(*yy); \
} while (0)
# define return_STRING_CASE_COMPARE(_x_, _y_) do { \
  int result; \
  STRING_CASE_COMPARE (_x_, _y_, result); \
  return result; \
} while (0)

/* hash and comparison macros for integer keys. */

# define INTEGER_HASH_1(_key_, _result_) do { \
  (_result_) += ((unsigned long)(_key_)); \
} while (0)
# define return_INTEGER_HASH_1(_key_) do { \
  unsigned long result = 0; \
  INTEGER_HASH_1 ((_key_), result); \
  return result; \
} while (0)

# define INTEGER_HASH_2(_key_, _result_) do { \
  (_result_) += ~((unsigned long)(_key_)); \
} while (0)
# define return_INTEGER_HASH_2(_key_) do { \
  unsigned long result = 0; \
  INTEGER_HASH_2 ((_key_), result); \
  return result; \
} while (0)

# define INTEGER_COMPARE(_x_, _y_, _result_) do { \
  (_result_) = _x_ - _y_; \
} while (0)
# define return_INTEGER_COMPARE(_x_, _y_) do { \
  int result; \
  INTEGER_COMPARE (_x_, _y_, result); \
  return result; \
} while (0)

/* hash and comparison macros for address keys. */

# define ADDRESS_HASH_1(_key_, _result_) INTEGER_HASH_1 (((unsigned long)(_key_)) >> 3, (_result_))
# define ADDRESS_HASH_2(_key_, _result_) INTEGER_HASH_2 (((unsigned long)(_key_)) >> 3, (_result_))
# define ADDRESS_COMPARE(_x_, _y_, _result_) INTEGER_COMPARE ((_x_), (_y_), (_result_))
# define return_ADDRESS_HASH_1(_key_) return_INTEGER_HASH_1 (((unsigned long)(_key_)) >> 3)
# define return_ADDRESS_HASH_2(_key_) return_INTEGER_HASH_2 (((unsigned long)(_key_)) >> 3)
# define return_ADDRESS_COMPARE(_x_, _y_) return_INTEGER_COMPARE ((_x_), (_y_))

#endif /* not _HASHTAB_H_ */
