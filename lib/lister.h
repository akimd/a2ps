/* lister.h - listing data in various formats
   Copyright 1998-2017 Free Software Foundation, Inc.

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

#ifndef LISTER_H_
#define LISTER_H_

enum lister_justification
{
  lister_left   = 0,
  lister_center = 1,
  lister_right  = 2
};

struct lister;

typedef size_t (*lister_width_t) (void const *item);
typedef void (*lister_print_t) (void const *item, FILE *stream);

/* Initialize the values taking the environment into account (for line
   width, and tabsize). */
void lister_initialize (struct lister *lister, FILE *stream);

/* Set the tiny term of LISTER to TTERM.  Returns the previous value.  */
struct tterm *lister_tterm_set (struct lister *lister,
					struct tterm *tterm);

/* Set the width of the white prefix in LISTER to SIZE.  Returns the
   previous value. */
size_t  lister_before_set (struct lister * lister, size_t size);

/* Set the width of the white suffix in LISTER to SIZE.  Returns the
   previous value. */
size_t lister_after_set (struct lister * lister, size_t size);


void lister_fprint_horizontal
  (struct lister * lister, FILE * stream,
	   void **items, size_t item_number,
	   lister_width_t item_width_fn, lister_print_t item_print_fn);

void lister_fprint_vertical
   (struct lister * lister, FILE * stream,
	    void **items, size_t item_number,
	    lister_width_t item_width_fn, lister_print_t item_print_fn);

void lister_fprint_separated
   (struct lister * lister, FILE * stream,
	    void **items, size_t item_number,
	    lister_width_t item_width_fn, lister_print_t item_print_fn);

/* Same as the previous functions, but using the default stream,
   width_fn and print_fn. */

void lister_print_vertical
  (struct lister * lister, void **items, size_t item_number);

void lister_print_horizontal
  (struct lister * lister, void **items, size_t item_number);

void lister_print_separated
  (struct lister * lister, void **items, size_t item_number);

#endif /* ! LISTER_H_ */
