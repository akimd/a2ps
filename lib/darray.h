/* darray.h - dynamic arrays handling
   Copyright 1996-2017 Free Software Foundation, Inc.

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

/* Author: Akim Demaille <demaille@inf.enst.fr> */

#ifndef DARRAY_H_
# define DARRAY_H_

enum da_growth
{
  da_steady,			/* i.e. you're responsible	*/
  da_linear,			/* i.e. 1 2 3 4...		*/
  da_geometrical		/* i.e. 1 2 4 8...		*/
};

typedef void (*da_map_func_t) (void *);
typedef void (*da_maparg_func_t) (void *, void *);
typedef void (*da_print_func_t) (const void *, FILE * stream);
typedef int (*da_cmp_func_t) (const void * k1, const void * k2);
typedef int (*da_cmp_arg_func_t) (const void * k1,
					  const void * k2,
					  const void * arg);

struct darray
{
  const char * name;
  size_t size;
  size_t original_size;		/* The size with which it has been created */
  enum da_growth growth;
  size_t increment;
  size_t len;		/* assert (len + 1 <= size) */
  void * * content;
  da_print_func_t self_print;
  da_cmp_func_t cmp;
};

extern int da_exit_error;		/* exit value when encounters	*
					 * an error (default is 1)	*/

/*
 * Maintaining
 */
struct darray *
da_new (const char * name, size_t size,
		enum da_growth growth, size_t increment,
		da_print_func_t self_print,
		da_cmp_func_t cmp);
void da_free_content (struct darray * arr, da_map_func_t free_func);
void da_free (struct darray * arr, da_map_func_t free_func);
void da_erase (struct darray * arr);
void da_print_stats (struct darray * arr, FILE * stream);
void da_resize (struct darray * arr, size_t size);
void da_grow (struct darray * arr);


/*
 * Copying
 */
struct darray * da_clone (struct darray * arr);

/*
 * Testing
 */
int da_is_full (struct darray * arr);
int da_is_sorted (struct darray * arr);
#define da_is_full(da) ((da)->len + 1 >= (da)->size)
#define da_is_empty(da) ((da)->len == 0)
/* Do they have same size, and same pointers in content? */
int da_equal (struct darray * ar1, struct darray * ar2);
/* Do they have same size, and equal contents arcording to ar1->cmp? */
int da_cmp_equal (struct darray * ar1, struct darray * ar2);
int da_where (struct darray * arr, const void * stuff);
int da_includes (struct darray * arr, const void * stuff);

/*
 * Adding/removing
 */
void da_append (struct darray * arr, void * elem);
void da_insert_at (struct darray * arr, void * elem, size_t where);
void da_remove_at (struct darray * arr,
			   size_t where, da_map_func_t free_func);
void da_concat (struct darray * arr, struct darray * arr2);
void da_prefix (struct darray * arr, struct darray * arr2);

/*
 * Mapped treatement on elements
 */
void da_qsort (struct darray * arr);
void da_qsort_with_arg (struct darray * arr,
				da_cmp_arg_func_t cmp,
				const void * arg);
void da_self_print (struct darray * arr, FILE * stream);
void da_unique (struct darray * arr, da_map_func_t free_func);

/* In case of equality, keep the first, or the second? */
enum da_include_policy {
  da_1_wins, da_2_wins
};

/* Merge A2 in A1, according to the POLICY, and free not retained
 * items by FREE_FUNC if not NULL */
void da_merge (struct darray * a1, struct darray * a2,
		       da_map_func_t free_func,
		       enum da_include_policy policy);


void da_map (struct darray * arr, da_map_func_t func);
void da_maparg (struct darray * arr,
			da_maparg_func_t func, void * arg);

/*
 * ready to use auxiliary functions
 */
int da_str_cmp (const char * s1, const char * s2);
void da_str_print (const char * s1, FILE * stream);
void da_str_printnl (const char * s1, FILE * stream);

#endif
