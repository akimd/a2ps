/*
 * delegate.h
 *
 * Handling the delegations
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
 * $Id: delegate.h,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

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

struct hash_table_s *delegation_table_new PARAMS ((void));
void delegation_table_free PARAMS ((struct hash_table_s * table));

void add_delegation PARAMS ((const char *filename, int line,
			     char *contract_line));
struct delegation *get_subcontract PARAMS ((const char *src_type,
					    const char *dest_type));
char *get_delegate_command PARAMS ((struct delegation * contract,
				    struct file_job * file,
				    int evaluate));
/* Execute the sub contract */
int subcontract PARAMS ((struct file_job * file, buffer_t * buffer,
			 struct delegation * contractor));
/*
 * For the command line interface
 */
void delegations_list_long PARAMS ((struct hash_table_s * contracts,
				    FILE * stream));
void delegations_list_short PARAMS ((struct hash_table_s * contracts,
				     FILE * stream));
#endif /* !defined(_DELEGATE_H_) */
