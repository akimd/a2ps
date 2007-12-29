/* lister.c -- Listing data in various formats
   Copyright (C) 1998-1999 Free Software Foundation, Inc.

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

#ifndef LISTER_H_
#define LISTER_H_

#ifndef PARAMS
# if defined PROTOTYPES || defined __STDC__
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

enum lister_justification
{
  lister_left   = 0,
  lister_center = 1,
  lister_right  = 2
};

struct lister;

typedef size_t (*lister_width_t) PARAMS ((void const *item));
typedef void (*lister_print_t) PARAMS ((void const *item, FILE *stream));

/* Initialize the values taking the environment into account (for line
   width, and tabsize). */
void lister_initialize PARAMS ((struct lister *lister, FILE *stream));

/* Set the tiny term of LISTER to TTERM.  Returns the previous value.  */
struct tterm *lister_tterm_set PARAMS ((struct lister *lister,
					struct tterm *tterm));

/* Set the width of the white prefix in LISTER to SIZE.  Returns the
   previous value. */
size_t  lister_before_set PARAMS ((struct lister * lister, size_t size));

/* Set the width of the white suffix in LISTER to SIZE.  Returns the
   previous value. */
size_t lister_after_set PARAMS ((struct lister * lister, size_t size));


void lister_fprint_horizontal
  PARAMS ((struct lister * lister, FILE * stream,
	   void **items, size_t item_number,
	   lister_width_t item_width_fn, lister_print_t item_print_fn));

void lister_fprint_vertical
   PARAMS ((struct lister * lister, FILE * stream,
	    void **items, size_t item_number,
	    lister_width_t item_width_fn, lister_print_t item_print_fn));

void lister_fprint_separated
   PARAMS ((struct lister * lister, FILE * stream,
	    void **items, size_t item_number,
	    lister_width_t item_width_fn, lister_print_t item_print_fn));

/* Same as the previous functions, but using the default stream,
   width_fn and print_fn. */

void lister_print_vertical
  PARAMS ((struct lister * lister, void **items, size_t item_number));

void lister_print_horizontal
  PARAMS ((struct lister * lister, void **items, size_t item_number));

void lister_print_separated
  PARAMS ((struct lister * lister, void **items, size_t item_number));

#endif /* ! LISTER_H_ */
