/* darray.c -- dynamic arrays handling

   Copyright (c) 1996-99 Akim Demaille, Miguel Santana

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Author: Akim Demaille <demaille@inf.enst.fr> */

#include <system.h>

#include "darray.h"

int da_exit_error = 1;			/* exit value when encounters	*
					 * an error 			*/

#define QSORT_INSERT_SORT_LIMIT	37	/* Bellow, insert sort is used */
#define QSORT_STACK		100
#define DA_SWAP(a,i,j) 	\
   do {					\
     tmp = a->content [i];		\
     a->content [i] = a->content [j];	\
     a->content [j] = tmp ;		\
   } while (0)

/*
 * Create a dynamic array
 */
struct darray *
da_new (const char * name, size_t size,
	enum da_growth growth, size_t increment,
	da_print_func_t self_print,
	da_cmp_func_t cmp)
{
  struct darray * res;

  /* No longer relevant: size_t cannot be null */
  if (size == 0)
    error (da_exit_error, 0, "invalid size for dynamic array `%s': %d",
	   name, size);
  if (increment == 0 && growth != da_steady)
    error (da_exit_error, 0, "invalid increment for dynamic array `%s': %d",
	   name, increment);

  res = XMALLOC (struct darray, 1);
  res->name = name;
  res->original_size = size;
  res->size = size;
  res->content = XCALLOC (void *, res->size);
  res->growth = growth;
  res->increment = increment;
  res->len = 0;

  /* Routines */
  res->self_print = self_print;
  res->cmp = cmp;

  return res;
}

static inline void
_da_erase (struct darray * arr)
{
  if (arr) {
    XFREE (arr->content);
    free (arr);
  }
}

void
da_erase (struct darray * arr)
{
  _da_erase (arr);
}

/*
 * Set length of ARR to 0, and free with FREE_FUNC if non NULL
 */
static inline void
_da_free_content (struct darray * arr, da_map_func_t free_func)
{
  size_t i;

  if (free_func)
    for (i = 0 ; i < arr->len ; i++)
      (*free_func) (arr->content [i]);

  arr->len = 0;
}

void
da_free_content (struct darray * arr, da_map_func_t free_func)
{
  _da_free_content (arr, free_func);
}

/*
 * Set length of ARR to 0, and free with FREE_FUNC if non NULL
 * and free the structure
 */
void
da_free (struct darray * arr, da_map_func_t free_func)
{
  _da_free_content (arr, free_func);
  _da_erase (arr);
}

/*
 * Report the status of the array
 */
void
da_print_stats (struct darray * arr, FILE * stream)
{
  const char * cp = NULL;

  fprintf (stream, _("Dynamic array `%s':\n"), arr->name);
  fprintf (stream, _("\tload: %d/%d (%2.1f%%)\n"),
	   arr->len, arr->size, 100.0 * arr->len / arr->size);
  switch (arr->growth) {
  case da_steady:
    /* growth is steady, i.e., it cannot grow, it is constant */
    cp = "[const]";
    break;
  case da_linear:
    /* growth is linear. eg. 2, 4, 6, 8 */
    cp = "+=";
    break;
  case da_geometrical:
    /* growth is exponential. eg. 2, 4, 8, 16 */
    cp = "*=";
    break;
  default:
    abort ();
  }
  fprintf (stream, _("\toriginal size: %d, growth: %s %d\n"),
	   arr->original_size, cp, arr->increment);
}

/*
 * Resize, unless too small to fit
 */
void
da_resize (struct darray * arr, size_t size)
{
  if (arr->len + 1 < size)
    {
      arr->size = size;
      arr->content = XREALLOC (arr->content, void *, arr->size);
    }
}

/*
 * Make a dyn. array bigger
 */
void
da_grow (struct darray * arr)
{
  switch (arr->growth) {
  case da_steady:
    return;

  case da_linear:
    arr->size += arr->increment;
    break;

  case da_geometrical:
    arr->size *= arr->increment;
    break;

  default:
    abort ();
  }
  arr->content = XREALLOC (arr->content, void *, arr->size);
}

/*
 * Make a clone
 */
struct darray *
da_clone (struct darray * array)
{
  struct darray * res;
  res = CLONE (array);
  res->content = CCLONE (array->content, array->len);
  return res;
}


/*
 * Is it sorted?
 */
int
da_is_sorted (struct darray * arr)
{
  size_t i;

  for (i = 1 ; i < arr->len ; i++)
    if (arr->cmp (arr->content [i], arr->content [i - 1]) < 0)
      return 0;
  return 1;
}

/*
 * Are two darray equal (pointer-wise)?
 */
int
da_equal (struct darray * ar1, struct darray * ar2)
{
  size_t i;

  if (ar1->len != ar2->len)
    return 0;

  for (i = 0 ; i< ar1->len ; i++)
    if (ar1->content [i] != ar2->content [i])
      return 0;
  return 1;
}

/*
 * Do two arrays have same semantics (wrt cmp) content?
 * (ar1->cmp is used for the comparison)
 */
int
da_cmp_equal (struct darray * ar1, struct darray * ar2)
{
  size_t i;

  if (ar1->len != ar2->len)
    return 0;

  for (i = 0 ; i< ar1->len ; i++)
    if (ar1->cmp (ar1->content [i], ar2->content [i]))
      return 0;
  return 1;
}

/*
 * Where is STUFF in ARR (equal in the sense of self_cmp)
 * -1 if is not in.
 */
int
da_where (struct darray * arr, const void * stuff)
{
  size_t i;

  for (i = 0 ; i < arr->len ; i++)
    if (!arr->cmp (arr->content[i], stuff))
      return (int) i;

  return -1;
}

/*
 * Does this stuff is selfcmp equal to an item of the darray?
 */
int
da_includes (struct darray * arr, const void * stuff)
{
  return (da_where (arr, stuff) != -1);
}

/*
 * Append an element
 */
void
da_append (struct darray * arr, void * elem)
{
  if (da_is_full (arr))
    da_grow (arr);

  arr->content [arr->len++] = elem;
}

/*
 * Insert an element at a given place.
 */
void
da_insert_at (struct darray * arr, void * elem, size_t where)
{
  size_t i;

  if (where > arr->len)
    error (da_exit_error, 0, "can't insert at %d in darray %s [0,%d]\n",
	   where, arr->name, arr->len - 1);

  if (da_is_full (arr))
    da_grow (arr);

  for (i = arr->len ; where < i ; i--)
    arr->content [i] = arr->content [i - 1];

  arr->content [ where ] = elem;
  arr->len ++;
}

/*
 * Remove an element at a given place.
 */
void
da_remove_at (struct darray * arr, size_t where, da_map_func_t free_func)
{
  size_t i;

  if (where >= arr->len)
    error (da_exit_error, 0, "can't remove at %d in darray %s [0,%d]\n",
	   where, arr->name, arr->len - 1);

  if (free_func)
    (*free_func) (arr->content [where]);

  for (i = where + 1 ; i < arr->len ; i++)
    arr->content [i - 1] = arr->content [i];

  arr->len --;
}

/*
 * Concat the second in the first
 */
void
da_concat (struct darray * arr, struct darray * arr2)
{
  size_t i;
  size_t len = arr->len + arr2->len;

  if (len > arr->size) {
    arr->size = len + 1;
    arr->content = XREALLOC (arr->content, void *, arr->size);
  }

  for (i = 0 ; i < arr2->len ; i++)
    arr->content [arr->len++] = arr2->content[i];
}

/*
 * Prefix the content of ARR by that of ARR2
 */
void
da_prefix (struct darray * arr, struct darray * arr2)
{
  int i;
  size_t len = arr->len + arr2->len;

  if (len > arr->size) {
    arr->size = len + 1;
    arr->content = XREALLOC (arr->content, void *, arr->size);
  }

  /* Move the content of ARR */
  for (i = (int) arr->len - 1 ; i >= 0 ; i--)
    arr->content [ i + arr2->len ] = arr->content [ i ];

  /* Copy the content of ARR2 */
  for (i = 0 ; i < (int) arr2->len ; i++)
    arr->content [ i ] = arr2->content[ i ];

  arr->len += arr2->len;
}

/*
 * Implementation of QSORT as given by Sedgewick
 */
void
da_qsort (struct darray * arr)
{
  int ir, j, k, l, i;
  int jstack, *istack;
  void * a, * tmp;

  /* Do not sort an empty array */
  if (arr->len <= 1)
    return;

  istack = XMALLOC (int, QSORT_STACK);
  ir = arr->len - 1;
  l = 0;
  jstack = 0;

  for (;;) {
      if (ir - l < QSORT_INSERT_SORT_LIMIT)
	{	/* Insertion sort is then prefered */
	  for (j = l + 1 ; j <= ir ; j++) {
	    a = arr->content [j];
	    for (i = j - 1 ; i >= l ; i--) {
	      if (arr->cmp (arr->content [i], a) <= 0)
		break;
	      arr->content [i + 1] = arr->content [i];
	    }
	    arr->content [i + 1] = a;
	  }
	  if (jstack == 0)
	    break;
	  ir = istack [jstack--];
	  l = istack [jstack--];
	}
      else
	{
	  k = (l + ir) / 2;
	  DA_SWAP (arr, k, l + 1);
	  if (arr->cmp (arr->content [l], arr->content [ir]) > 0)
	    DA_SWAP (arr, l, ir);
	  if (arr->cmp (arr->content [l + 1], arr->content [ir]) > 0)
	    DA_SWAP (arr, l + 1, ir);
	  if (arr->cmp (arr->content [l], arr->content [l + 1]) > 0)
	    DA_SWAP (arr, l, l + 1);
	  i = l + 1;
	  j = ir;
	  a = arr->content [l + 1];
	  for (;;) {
	    do i++; while (arr->cmp (arr->content [i], a) < 0);
	    do j--; while (arr->cmp (arr->content [j], a) > 0);
	    if (j < i)
	      break;	/* Partion is completed	*/
	    DA_SWAP (arr, i, j);
	  }
	  arr->content [l + 1] = arr->content [j];
	  arr->content [j] = a;
	  jstack += 2;
	  /* Push pointers to larger subarry on stack.
	   * Process smaller subarrays now	*/
	  if (jstack > QSORT_STACK)
	    error (da_exit_error, 0, "da_qsort: QSORT_STACK too small (%d)",
			   QSORT_STACK);
	  if (ir - i + 1 >= j - l) {
	    istack [jstack]     = ir;
	    istack [jstack - 1] = i;
	    ir = j - 1;
	  } else {
	    istack [jstack]     = j - 1;
	    istack [jstack - 1] = l;
	    l = i;
	  }
	}
  }
  free (istack);
}

/*
 * Implementation of QSORT as given by Sedgewick
 */
void
da_qsort_with_arg (struct darray * arr, da_cmp_arg_func_t cmp,
		   const void * arg)
{
  int ir, j, k, l, i;
  int jstack, *istack;
  void * a, * tmp;

  /* Do not sort an empty array */
  if (arr->len <= 1)
    return;

  istack = XMALLOC (int, QSORT_STACK);
  ir = arr->len - 1;
  l = 0;
  jstack = 0;

  for (;;) {
      if (ir - l < QSORT_INSERT_SORT_LIMIT)
	{	/* Insertion sort is then prefered */
	  for (j = l + 1 ; j <= ir ; j++) {
	    a = arr->content [j];
	    for (i = j - 1 ; i >= l ; i--) {
	      if (cmp (arr->content [i], a, arg) <= 0)
		break;
	      arr->content [i + 1] = arr->content [i];
	    }
	    arr->content [i + 1] = a;
	  }
	  if (jstack == 0)
	    break;
	  ir = istack [jstack--];
	  l = istack [jstack--];
	}
      else
	{
	  k = (l + ir) / 2;
	  DA_SWAP (arr, k, l + 1);
	  if (cmp (arr->content [l], arr->content [ir], arg) > 0)
	    DA_SWAP (arr, l, ir);
	  if (cmp (arr->content [l + 1], arr->content [ir], arg) > 0)
	    DA_SWAP (arr, l + 1, ir);
	  if (cmp (arr->content [l], arr->content [l + 1], arg) > 0)
	    DA_SWAP (arr, l, l + 1);
	  i = l + 1;
	  j = ir;
	  a = arr->content [l + 1];
	  for (;;) {
	    do i++; while (cmp (arr->content [i], a, arg) < 0);
	    do j--; while (cmp (arr->content [j], a, arg) > 0);
	    if (j < i)
	      break;	/* Partion is completed	*/
	    DA_SWAP (arr, i, j);
	  }
	  arr->content [l + 1] = arr->content [j];
	  arr->content [j] = a;
	  jstack += 2;
	  /* Push pointers to larger subarry on stack.
	   * Process smaller subarrays now	*/
	  if (jstack > QSORT_STACK)
	    error (da_exit_error, 0, "da_qsort: QSORT_STACK too small (%d)",
			   QSORT_STACK);
	  if (ir - i + 1 >= j - l) {
	    istack [jstack]     = ir;
	    istack [jstack - 1] = i;
	    ir = j - 1;
	  } else {
	    istack [jstack]     = j - 1;
	    istack [jstack - 1] = l;
	    l = i;
	  }
	}
  }
  free (istack);
}

/*
 * Leave the first of each doubles
 */
void
da_unique (struct darray * arr, da_map_func_t free_func)
{
  size_t c;

  c = 1;
  while (c < arr->len) {
    if (arr->cmp (arr->content [c - 1], arr->content[c]) == 0)
      da_remove_at (arr, c, free_func);
    else
      c++;
  }
}

/*
 * Merge A2 into A1.  Both *are sorted*.
 * In the result there are never two equal entries
 * (in the sense of self_cmp).
 *
 * In case of conflict (equal entries from the point of view
 * of a1->cmp),
 * - if POLICY == da_1_wins, keep that of A1
 * - if POLICY == da_2_wins, keep that of A2
 *
 * If there are doubles in a1 and/or in a2, they still will be doubles
 * in the returned result.
 */
void
da_merge (struct darray * a1, struct darray * a2,
	  da_map_func_t free_func, enum da_include_policy policy)
{
  size_t c1, c2;		/* Counters on a1, and a2	*/

  c1 = c2 = 0;

  while ((c1 != a1->len) || (c2 != a2->len))
    {
      /* Leave what is in a1 as long as it is strictly smaller than the
       * next item of a2 */
      while ((c1 < a1->len)
	     && ((c2 == a2->len)
		 || (a1->cmp (a1->content [c1], a2->content [c2]) < 0)))
	c1 ++;

      /* Skip whatever appears in a1, but is in a2 too */
      while ((c1 < a1->len) && (c2 < a2->len)
	     && (a1->cmp (a1->content [c1], a2->content [c2]) == 0))
	if (policy == da_1_wins)
	  {
	    if (free_func)
	      da_remove_at (a2, c2, free_func);
	    else
	      c2++;
	  }
	else
	  {
	    if (free_func)
	      da_remove_at (a1, c1, free_func);
	    else
	      c1++;
	  }

      /* Take what is is a2 as long as it is smaller or equal to
       * what appeared last in a1 */
      while ((c2 < a2->len)
	     && ((c1 == a1->len)
		 || (a1->cmp (a1->content [c1], a2->content [c2]) >= 0)))
	da_insert_at (a1, a2->content [c2++], c1);
    }
}


/*
 * Dump on stderr the content
 */
void
da_self_print (struct darray * arr, FILE * stream)
{
  size_t i;

  fprintf (stream, _("Dynamic array `%s':\n"), arr->name);
  if (!arr->self_print)
    abort ();
  for (i = 0 ; i < arr->len ; i++) {
    fprintf (stream, "[%2d] = ", i);
    arr->self_print (arr->content [i], stream);
    fprintf (stream, "\n");
  }
}

/*
 * For each item of ARR, call FN (ITEM)
 */
void
da_map (struct darray * arr, da_map_func_t fn)
{
  size_t i;

  for (i = 0 ; i < arr->len ; i++)
    (*fn) (arr->content [i]);
}

/*
 * Idem, but with an argument
 */
void
da_maparg (struct darray * arr, da_maparg_func_t func, void * arg)
{
  size_t i;
  for (i = 0 ; i < arr->len ; i++)
    (*func) (arr->content [i], arg);
}

/*
 * Some helping routines for special darray cases
 */
/*
 * darray of strings
 */
int
da_str_cmp (const char * s1, const char * s2)
{
  return strcmp (s1, s2);
}

void
da_str_print (const char * s1, FILE * stream)
{
  fputs ((const char *) s1, stream);
}

void
da_str_printnl (const char * s1, FILE * stream)
{
  fputs ((const char *) s1, stream);
  putc ('\n', stream);
}
