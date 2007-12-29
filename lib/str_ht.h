/*
 * str_ht.h
 *
 * Single (char *) hash table
 *
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
 * $Id: str_ht.h,v 1.1.1.1.2.1 2007/12/29 01:58:23 mhatta Exp $
 */

#ifndef _STR_HT_H_
#define _STR_HT_H_

struct string_htable;

/*
 * Create / Kill
 */
struct string_htable * string_htable_new PARAMS ((void));
void string_htable_free PARAMS ((struct string_htable * table));

/*
 *  Add, get
 */
void string_htable_add PARAMS ((struct string_htable * table, 
				const char * key));
char * string_htable_get PARAMS ((struct string_htable * table,
				  const char * key));

/*
 * Print/Dump
 */
void string_htable_self_print PARAMS ((struct string_htable * table,
				       FILE * stream));
char ** string_htable_dump_sorted PARAMS ((struct string_htable * table));

#endif /* !defined(_STR_HT_H_) */
