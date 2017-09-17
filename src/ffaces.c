/* ffaces.c - definition of the flagged faces used by a2ps
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

#include <config.h>

#include "a2ps.h"
#include "ffaces.h"
#include "routines.h"
#include "jobs.h"

/*
 * List of the fflags and their readable names.
 */
struct fflag_and_name {	/* Auxialiary struct just for the following list */
  const char * name;
  enum fflag_e flag;
};

static const struct fflag_and_name fflag_and_name [] = {
  { "Invisible",ff_Invisible },
  { "Tag1", 	ff_Tag1 },
  { "Tag2", 	ff_Tag2 },
  { "Tag3", 	ff_Tag3 },
  { "Tag4",	ff_Tag4 },
  { "Index1", 	ff_Index1 },
  { "Index2", 	ff_Index2 },
  { "Index3", 	ff_Index3 },
  { "Index4", 	ff_Index4 },
  { "Encoding",	ff_Encoding },
  { NULL, ff_No_fflag }
};

/*
 * Some predefined ffaces.
 */
struct fface_s String_fface = { String, ff_No_fflag };
struct fface_s Plain_fface = { Plain, ff_No_fflag };
struct fface_s Symbol_fface = { Symbol, ff_No_fflag };
struct fface_s No_fface = { No_face, ff_No_fflag };


/*
 * Report the flagged face (included the face)
 */
void
fflag_self_print (enum fflag_e flags, FILE * stream)
{
  int i;
  int first = true;

  putc ('(', stream);
  
  if (flags == ff_No_fflag)
    fputs ("No_fflag", stream);
  else
    {
      /* Report the flags: make a loop onto the bits */
      for (i = 0 ; fflag_and_name [i].flag ; i++)
	if (fflag_and_name [i].flag & flags) {
	  if (!first)
	    fputs (" + ", stream);
	  else
	    first = false;
	  fputs (fflag_and_name [i].name, stream);
	}
    }
  
  putc (')', stream);
}

/*
 * Report the flagged face (included the face)
 */
void
fface_self_print (struct fface_s fface, FILE * stream)
{
  int i;

  if (fface.flags) {
    putc ('(', stream);

    /* Report the base face */
    face_self_print (fface.face, stream);
    
    /* Report the flags: make a loop onto the bits */
    for (i = 0 ; fflag_and_name [i].flag ; i++)
      if (fflag_and_name [i].flag & fface.flags) {
	fputs (" + ", stream);
	fputs (fflag_and_name [i].name, stream);
      }
  
    putc (')', stream);
  }
  else
    /* There is nothing but the pure face to report */
    face_self_print (fface.face, stream);
}
