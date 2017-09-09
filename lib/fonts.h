/* fonts.h - reading an AFM file
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

#ifndef _LEXAFM_H_
#define _LEXAFM_H_

struct a2ps_job;

/*
 * Handling of fonts.map files
 */
struct pair_htable * fonts_map_new (void);
void fonts_map_free (struct pair_htable * table);
int load_main_fonts_map (struct a2ps_job * job);


struct hash_table_s * font_info_table_new (void);
void font_info_table_free (struct hash_table_s * table);
void font_info_add (struct a2ps_job * job, const char * key);

int font_exists (struct a2ps_job * job, const char * key);
int font_is_to_reencode (struct a2ps_job * job, const char * key);

void font_info_get_wx_for_vector (struct a2ps_job * job,
					  const char * key, 
					  char * vector [256],
					  unsigned int * wx);

/* Dump the setup of the special fonts (e.g. Symbol) */
void font_info_table_dump_special_font_setup (FILE * stream,
						      struct a2ps_job * job);

/* Dump the definition of the fonts */
void dump_fonts (FILE * stream, struct a2ps_job * job);
#endif
