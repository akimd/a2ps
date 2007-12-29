/*
 * dsc.h
 *
 * Recording information about the PostScript resources
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
 * $Id: dsc.h,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
 */
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
