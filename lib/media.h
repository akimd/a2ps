/*
 * media.h
 *
 * Used paper formats
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: media.h,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
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
#ifndef _MEDIAS_H_
#define _MEDIAS_H_

#include "config.h"
#include "hashtab.h"

struct a2ps_job;

/* The name of the pseudo-medium that gives access to the choices set
   by libpaper.  */
#define LIBPAPER_MEDIUM "libpaper"

/* Definition of the media available */
struct medium
{
  char * name;
  int w;
  int h;
  int llx;
  int lly;
  int urx;
  int ury;
};

/* Possible orientations */
typedef enum
{
  portrait, landscape
} ORIENTATION;

struct hash_table_s * new_medium_table PARAMS ((void));

void free_medium_table PARAMS ((struct hash_table_s * table));

struct medium * a2ps_get_medium PARAMS ((struct a2ps_job * job, 
					 const char *string));

const char  * a2ps_get_medium_name PARAMS ((struct a2ps_job * job, 
					    const char *string));

bool a2ps_medium_libpaper_p PARAMS ((struct a2ps_job * job, 
					const char * name));

void add_medium PARAMS ((struct a2ps_job * job,
			const char * name,
			int w, int h,
			int llx, int lly, int urx, int ury));

void list_media_long PARAMS ((struct a2ps_job * job, FILE * stream));
void list_media_short PARAMS ((struct a2ps_job * job, FILE * stream));

#endif
