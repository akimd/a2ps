/*
 * fonts.h
 *
 * Reading an AFM file
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: fonts.h,v 1.1.1.1.2.1 2007/12/29 01:58:18 mhatta Exp $
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
 * $Id: fonts.h,v 1.1.1.1.2.1 2007/12/29 01:58:18 mhatta Exp $
 */

#ifndef _LEXAFM_H_
#define _LEXAFM_H_

struct a2ps_job;

/*
 * Handling of fonts.map files
 */
struct pair_htable * fonts_map_new PARAMS ((void));
void fonts_map_free PARAMS ((struct pair_htable * table));
int load_main_fonts_map PARAMS ((struct a2ps_job * job));


struct hash_table_s * font_info_table_new PARAMS ((void));
void font_info_table_free PARAMS ((struct hash_table_s * table));
void font_info_add PARAMS ((struct a2ps_job * job, const char * key));

int font_exists PARAMS ((struct a2ps_job * job, const char * key));
int font_is_to_reencode PARAMS ((struct a2ps_job * job, const char * key));

void font_info_get_wx_for_vector PARAMS ((struct a2ps_job * job,
					  const char * key, 
					  char * vector [256],
					  unsigned int * wx));

/* Dump the setup of the special fonts (e.g. Symbol) */
void font_info_table_dump_special_font_setup PARAMS ((FILE * stream,
						      struct a2ps_job * job));

/* Dump the definition of the fonts */
void dump_fonts PARAMS ((FILE * stream, struct a2ps_job * job));
#endif
