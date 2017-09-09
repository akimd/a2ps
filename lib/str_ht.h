/* str_ht.h - single (char *) hash table
   Copyright 1988-2017 Free Software Foundation, Inc.

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

#ifndef _STR_HT_H_
#define _STR_HT_H_

struct string_htable;

/*
 * Create / Kill
 */
struct string_htable * string_htable_new (void);
void string_htable_free (struct string_htable * table);

/*
 *  Add, get
 */
void string_htable_add (struct string_htable * table, 
				const char * key);
char * string_htable_get (struct string_htable * table,
				  const char * key);

/*
 * Print/Dump
 */
void string_htable_self_print (struct string_htable * table,
				       FILE * stream);
char ** string_htable_dump_sorted (struct string_htable * table);

#endif /* !defined(_STR_HT_H_) */
