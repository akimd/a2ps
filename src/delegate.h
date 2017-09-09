/* delegate.h - handling the delegations
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

#ifndef _DELEGATE_H_
#define _DELEGATE_H_

#include "hashtab.h"
#include "buffer.h"

struct file_job;


struct delegation
  {
    char *name;			/* e.g. groff                           */
    char *contract;		/* e.g. roff:ps                         */
    char *command;		/* e.g. groff -man $f                   */
  };

struct hash_table_s *delegation_table_new (void);
void delegation_table_free (struct hash_table_s * table);

void add_delegation (const char *filename, int line,
			     char *contract_line);
struct delegation *get_subcontract (const char *src_type,
					    const char *dest_type);
char *get_delegate_command (struct delegation * contract,
				    struct file_job * file,
				    int evaluate);
/* Execute the sub contract */
int subcontract (struct file_job * file, buffer_t * buffer,
			 struct delegation * contractor);
/*
 * For the command line interface
 */
void delegations_list_long (struct hash_table_s * contracts,
				    FILE * stream);
void delegations_list_short (struct hash_table_s * contracts,
				     FILE * stream);
#endif /* !defined(_DELEGATE_H_) */
