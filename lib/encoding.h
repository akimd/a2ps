/*
 * encoding.h
 *
 * definition of the char encodings used
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: encoding.h,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
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
/************************************************************************/
/*			encoding selection				*/
/************************************************************************/
#ifndef _ENCODING_H_
#define _ENCODING_H_
#include "a2ps.h"
#include "faces.h"
#include "hashtab.h"

struct a2ps_job;	/* Declared in jobs.h which includes this file */

#define COURIER_WX	600	/* Any char in courier has WX = 600 */

/*
 * Computation of the width of the chars in the current encoding
 */
unsigned int char_WX PARAMS ((struct a2ps_job * job, uchar c));
unsigned int string_WX PARAMS ((struct a2ps_job * job, uchar * string));


/************************************************************************/
/* Handling of the struct encoding				*/
/************************************************************************/
struct encoding;

const char * encoding_get_name PARAMS ((struct encoding * enc));
const char * encoding_get_key PARAMS ((struct encoding * enc));
void encoding_self_print PARAMS ((struct encoding * item,
					FILE * stream));
int encoding_char_exists PARAMS ((struct encoding * enc,
					enum face_e face, uchar c));
struct encoding *
get_encoding_by_alias PARAMS ((struct a2ps_job * job, 
			       char *string));

void set_encoding PARAMS ((struct a2ps_job * job,
			   struct encoding * enc));

/* When FONT_NAME is used with ENCODING, return the real font name to
 * use (e.g., in latin2, Courier-Ogonki should be returned when asked
 * for Courier).  */
const char * encoding_resolve_font_substitute
	PARAMS ((struct a2ps_job * job,
		 struct encoding * encoding,
		 const char * font_name));

/*
 * Have a struct encoding determine the faces_wx
 */
void encoding_build_faces_wx PARAMS ((struct a2ps_job * job,
				      struct encoding * encoding));
void encoding_add_font_name_used PARAMS ((struct encoding * enc,
						const char * name));

/* Dump on STREAM the encodings setup */
void dump_encodings_setup PARAMS ((FILE * stream, struct a2ps_job * job));

/*
 * Related to a2ps_job
 */
void list_encodings_short PARAMS ((struct a2ps_job * job, FILE * stream));
void list_encodings_long PARAMS ((struct a2ps_job * job, FILE * stream));
void list_texinfo_encodings_long PARAMS ((struct a2ps_job * job,
					  FILE * stream));

/************************************************************************/
/* Road map to the files defining the encodings				*/
/************************************************************************/
struct pair_htable * encodings_map_new PARAMS ((void));
void encodings_map_free PARAMS ((struct pair_htable * table));

struct hash_table_s * encodings_table_new PARAMS ((void));
void encodings_table_free PARAMS ((struct hash_table_s * table));

int load_main_encodings_map PARAMS ((struct a2ps_job * job));
#endif
