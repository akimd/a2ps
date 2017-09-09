/* useropt.h - user option handling
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

#ifndef _USEROPT_H_
#define _USEROPT_H_

/* Declare these two, avoiding includes that generate many depends */
struct job;
struct pair_htable;

void dict_entry_print (void const * item);


/*
 * User options
 */
struct pair_htable * user_options_table_new (void);
void user_options_table_free (struct pair_htable * table);

void user_option_add (struct a2ps_job * job, 
			      const char * key, const char * value);
char * user_option_get (struct a2ps_job * job, 
				char * key);
void user_options_list_short (struct a2ps_job * job, FILE * stream);
void user_options_list_long (struct a2ps_job * job, FILE * stream);

#endif
