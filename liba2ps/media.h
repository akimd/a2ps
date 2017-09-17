/* media.h - used paper formats
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

#ifndef _MEDIAS_H_
#define _MEDIAS_H_

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

struct hash_table_s * new_medium_table (void);

void free_medium_table (struct hash_table_s * table);

struct medium * a2ps_get_medium (struct a2ps_job * job, 
					 const char *string);

const char  * a2ps_get_medium_name (struct a2ps_job * job, 
					    const char *string);

bool a2ps_medium_libpaper_p (struct a2ps_job * job, 
					const char * name);

void add_medium (struct a2ps_job * job,
			const char * name,
			int w, int h,
			int llx, int lly, int urx, int ury);

void list_media_long (struct a2ps_job * job, FILE * stream);
void list_media_short (struct a2ps_job * job, FILE * stream);

#endif
