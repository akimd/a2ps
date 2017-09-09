/* metaseq.h - handling of the metasequences
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

#ifndef _METASEQ_H_
#define _METASEQ_H_

struct a2ps_job;
struct file_job;
struct pair_htable;

/* Macro meta sequences hash table */

struct pair_htable * macro_meta_sequence_table_new (void);
void macro_meta_sequence_table_free (struct pair_htable * table);

/* Return true if valid definition, false otherwise */
bool macro_meta_sequence_add (struct a2ps_job * job,
					 const char * key,
					 const char * value);
void macro_meta_sequence_delete (struct a2ps_job * job,
					 const char * key);
char * macro_meta_sequence_get (struct a2ps_job * job,
					const char * key);
void macro_meta_sequences_list_short (struct a2ps_job * job,
					     FILE * stream);

void macro_meta_sequences_list_long (struct a2ps_job * job,
					    FILE * stream);

/*
 * Expand escape sequences in a string
 * Note: no malloc is done on the result!
 * The result must either be used before the following
 * call to expand_user_string, or strdup'ed
 */
unsigned char * expand_user_string (struct a2ps_job * job,
				    struct file_job * file,
				    const unsigned char * context_name,
				    const unsigned char * str);

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
