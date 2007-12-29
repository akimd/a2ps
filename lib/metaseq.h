/*
 * metaseq.h
 *
 * handling of the metasequences
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: metaseq.h,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
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
 * $Id: metaseq.h,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
 */

#ifndef _METASEQ_H_
#define _METASEQ_H_

struct a2ps_job;
struct file_job;
struct pair_htable;

/* Macro meta sequences hash table */

struct pair_htable * macro_meta_sequence_table_new PARAMS ((void));
void macro_meta_sequence_table_free PARAMS ((struct pair_htable * table));

/* Return true if valid definition, false otherwise */
bool macro_meta_sequence_add PARAMS ((struct a2ps_job * job,
					 const char * key,
					 const char * value));
void macro_meta_sequence_delete PARAMS ((struct a2ps_job * job,
					 const char * key));
char * macro_meta_sequence_get PARAMS ((struct a2ps_job * job,
					const char * key));
void macro_meta_sequences_list_short PARAMS ((struct a2ps_job * job,
					     FILE * stream));

void macro_meta_sequences_list_long PARAMS ((struct a2ps_job * job,
					    FILE * stream));

/*
 * Expand escape sequences in a string
 * Note: no malloc is done on the result!
 * The result must either be used before the following
 * call to expand_user_string, or strdup'ed
 */
uchar * expand_user_string PARAMS ((struct a2ps_job * job,
				    struct file_job * file,
				    const uchar * context_name,
				    const uchar * str));

/* Definition of the integer divertions */
#define JOB_NB_PAGES		1
#define JOB_NB_SHEETS		2
#define JOB_NB_FILES		3
#define FILE_NB_PAGES		4
#define FILE_NB_SHEETS		5
#define FILE_NB_LINES		6
#define FILE_LAST_PAGE		7
#define FILE_LAST_SHEET		8

#define VAR_USER_COMMENTS	"user.comments"
#define VAR_USER_HOME		"user.home"
#define VAR_USER_HOST		"user.host"
#define VAR_USER_LOGIN		"user.login"
#define VAR_USER_NAME		"user.name"

#define VAR_OPT_VIRTUAL_FORCE	"opt.virtual.force"
#endif
