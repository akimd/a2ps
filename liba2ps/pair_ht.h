/* pair_ht.h - two (char *) hash table
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

#ifndef _PAIR_HT_H_
#define _PAIR_HT_H_

struct pair_htable;

/*
 * The type of the functions given as argument to pair_table_map
 */
typedef void (* pair_ht_map_fn_t) (int i, 
					   const char * key, 
					   const char * value, 
					   void const * arg);
typedef int (* pair_ht_select_fn_t) (const char * key, 
					     const char * value);

/*
 * String_Entrys
 */
struct pair_htable * pair_table_new (void);
void pair_table_free (struct pair_htable * table);

/*
 * KEY and VALUE will be strdup'd
 */
void pair_add (struct pair_htable * table, 
		       const char * key, const char * value);

/*
 * The key and value of the matching item  will be free'd
 * (No problem if KEY matches nothing)
 */
void pair_delete (struct pair_htable * table, const char * key);

/*
 * Returns NULL when KEY is not used, otherwise its associated VALUE
 * in TABLE
 */
char * pair_get (struct pair_htable * table,
			 const char * key);

void pair_table_list_short (struct pair_htable * table,
				    FILE * stream);
void pair_table_list_long (struct pair_htable * table,
				   FILE * stream);
void pair_table_self_print (struct pair_htable * table,
				    FILE * stream);

/*
 * Map a function to the content of the table
 */
void pair_table_map (struct pair_htable * table,
			     pair_ht_map_fn_t map_fn,
			     pair_ht_select_fn_t select_fn,
			     void const * arg);

/*
 * Load entries from a map file
 */
int pair_table_load (struct pair_htable * table,
			     const char * file);
#endif
