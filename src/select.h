/* select.h - selection of a style sheet
   Copyright 1995-2017 Free Software Foundation, Inc.

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

#ifndef _SELECT_H_
#define _SELECT_H_
#include "ssheet.h"
#include "darray.h"

/* Used as hook when reading the options */
void set_requested_style (const char * arg);

/************************************************************************/
/*			sheets.map handling				*/
/************************************************************************/
/*
 * Structure in charge of remembering the sheets read
 */
struct darray * sheets_map_new (void);
void
sheets_map_add (const char *pattern, int on_file_verdict,
			bool insenstive, const char *key);

/* Pattern matching is run on NAME_TO_MATCH, and file(1)
 * is run of NAME_TO_FILE
 */
const char * get_command (const unsigned char * name_to_match,
				  const unsigned char * name_to_file);

struct pattern_rule *
pattern_rule_new (const char *pattern,
			  int on_file_verdict,
			  bool insensitive_p,
			  const char *command);

void
sheets_map_load (const char *filename);

#endif /* !defined(_SELECT_H_) */
