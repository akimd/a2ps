/* dsc.h - recording information about the PostScript resources
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

#ifndef _DSC_H_
#define _DSC_H_

struct a2ps_job;

/*
 * Handling the multivalued_entry hash tables
 */
struct hash_table_s * multivalued_table_new PARAMS ((void));
void multivalued_table_free PARAMS ((struct hash_table_s * table));

/*
 * needed ps resources (such as fonts)
 */
void add_needed_resource PARAMS ((struct a2ps_job * job, 
				  const char * key, const char * value));
int exist_resource PARAMS ((struct a2ps_job * job, 
			    const char * key, const char * value));

void add_process_color PARAMS ((struct a2ps_job * job,
				const char * value));
void dump_process_color PARAMS ((FILE * stream,
				 struct a2ps_job * job));

void add_needed_resource PARAMS ((struct a2ps_job * job, 
				  const char * key, const char * value));
void dump_needed_resources PARAMS ((FILE * stream,
				    struct a2ps_job * job));

void add_supplied_resource PARAMS ((struct a2ps_job * job, 
				    const char * key, const char * value));
void dump_supplied_resources PARAMS ((FILE * stream, 
				      struct a2ps_job * job));

/*
 * Declaration of the fonts, etc.
 */
void add_required_font PARAMS ((struct a2ps_job * job, const char * name));
char ** required_fonts_get PARAMS ((struct a2ps_job * job));
void dump_setup PARAMS ((FILE * stream, struct a2ps_job * job));

#endif
