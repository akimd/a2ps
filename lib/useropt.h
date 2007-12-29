/*
 * useropt.h
 *
 * Recording information about the print jobs
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
 * $Id: useropt.h,v 1.1.1.1.2.1 2007/12/29 01:58:25 mhatta Exp $
 */
#ifndef _USEROPT_H_
#define _USEROPT_H_

/* Declare these two, avoiding includes that generate many depends */
struct job;
struct pair_htable;

void dict_entry_print PARAMS ((void const * item));


/*
 * User options
 */
struct pair_htable * user_options_table_new PARAMS ((void));
void user_options_table_free PARAMS ((struct pair_htable * table));

void user_option_add PARAMS ((struct a2ps_job * job, 
			      const char * key, const char * value));
char * user_option_get PARAMS ((struct a2ps_job * job, 
				char * key));
void user_options_list_short PARAMS ((struct a2ps_job * job, FILE * stream));
void user_options_list_long PARAMS ((struct a2ps_job * job, FILE * stream));

#endif
