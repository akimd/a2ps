/*
 * pair_ht.h
 *
 * Two (char *) hash table
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 *
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
 * $Id: pair_ht.h,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
 */
#ifndef _PAIR_HT_H_
#define _PAIR_HT_H_

struct pair_htable;

/*
 * The type of the functions given as argument to pair_table_map
 */
typedef void (* pair_ht_map_fn_t) PARAMS ((int i, 
					   const char * key, 
					   const char * value, 
					   void const * arg));
typedef int (* pair_ht_select_fn_t) PARAMS ((const char * key, 
					     const char * value));

/*
 * String_Entrys
 */
struct pair_htable * pair_table_new PARAMS ((void));
void pair_table_free PARAMS ((struct pair_htable * table));

/*
 * KEY and VALUE will be strdup'd
 */
void pair_add PARAMS ((struct pair_htable * table, 
		       const char * key, const char * value));

/*
 * The key and value of the matching item  will be free'd
 * (No problem if KEY matches nothing)
 */
void pair_delete PARAMS ((struct pair_htable * table, const char * key));

/*
 * Returns NULL when KEY is not used, otherwise its associated VALUE
 * in TABLE
 */
char * pair_get PARAMS ((struct pair_htable * table,
			 const char * key));

void pair_table_list_short PARAMS ((struct pair_htable * table,
				    FILE * stream));
void pair_table_list_long PARAMS ((struct pair_htable * table,
				   FILE * stream));
void pair_table_self_print PARAMS ((struct pair_htable * table,
				    FILE * stream));

/*
 * Map a function to the content of the table
 */
void pair_table_map PARAMS ((struct pair_htable * table,
			     pair_ht_map_fn_t map_fn,
			     pair_ht_select_fn_t select_fn,
			     void const * arg));

/*
 * Load entries from a map file
 */
int pair_table_load PARAMS ((struct pair_htable * table,
			     const char * file));
#endif
