/*
 * select.h
 *
 * Selection of a style
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
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
 * $Id: select.h,v 1.1.1.1.2.1 2007/12/29 01:58:36 mhatta Exp $
 */

#ifndef _SELECT_H_
#define _SELECT_H_
#include "ssheet.h"
#include "darray.h"

/* Used as hook when reading the options */
void set_requested_style PARAMS ((const char * arg));

/************************************************************************/
/*			sheets.map handling				*/
/************************************************************************/
/*
 * Structure in charge of remembering the sheets read
 */
struct darray * sheets_map_new PARAMS ((void));
void
sheets_map_add PARAMS ((const char *pattern, int on_file_verdict,
			bool insenstive, const char *key));

/* Pattern matching is run on NAME_TO_MATCH, and file(1)
 * is run of NAME_TO_FILE
 */
const char * get_command PARAMS ((const uchar * name_to_match,
				  const uchar * name_to_file));

struct pattern_rule *
pattern_rule_new PARAMS ((const char *pattern,
			  int on_file_verdict,
			  bool insensitive_p,
			  const char *command));

void
sheets_map_load PARAMS ((const char *filename));

#endif /* !defined(_SELECT_H_) */
