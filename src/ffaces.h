/*
 * ffaces.h
 *
 * definition of the flagged faces used by a2ps
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
 * $Id: ffaces.h,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

#ifndef _FFACES_H_
#define _FFACES_H_
#include "faces.h"

/*
 * These flags can be seen as an extension of the faces: they
 * offer a way to have more information about the way the text
 * should be processed by adding special flags _not_ to be
 * seen by liba2ps.
 *
 * Many things used to be implemented as pure face_e, but it is
 * better like this, and removes from liba2ps code/defs that were
 * ment to a2ps-prog only
 */
enum fflag_e { 
  ff_No_fflag = 0,

  /* Make the text invisible (don't print it) */
  ff_Invisible = 1 << 0,

  /* Grabbing from the text (used for headings etc) */
  ff_Tag1 = 1 << 1, 
  ff_Tag2 = 1 << 2, 
  ff_Tag3 = 1 << 3, 
  ff_Tag4 = 1 << 4,

  /* Store in the Indexes */
  ff_Index1 = 1 << 5, 
  ff_Index2 = 1 << 6, 
  ff_Index3 = 1 << 7, 
  ff_Index4 = 1 << 8,

  /* Grab an Encoding (Dynamic encoding switches) */
  ff_Encoding = 1 << 9
};

void fflag_self_print PARAMS ((enum fflag_e flags, FILE * stream));

/************************************************************************/
/*	Flagged faces							*/
/************************************************************************/

struct fface_s {
  enum face_e face;	/* The part which is the face to give to liba2ps */
  enum fflag_e flags;	/* The special flags for prog-a2ps		*/
};

void fface_self_print PARAMS ((struct fface_s face, FILE * stream));

/*
 * Read/Set the face part
 */
#define fface_set_face(ff,fa)			\
	((ff).face) = (fa)

#define fface_reset_face(ff)			\
	((ff).face) = (No_face)

#define fface_get_face(ff)			\
	((ff).face)


/*
 * Read/Set the flags part
 */
#define fface_set_flags(ff,fl)			\
	((ff).flags) = (fl)

#define fface_reset_flags(ff)			\
	((ff).flags) = ff_No_fflag

#define fface_get_flags(ff)			\
	((ff).flags)

/* Here I cast, because for MIPSpro, enum | enum => int */
#define fface_add_flags(ff,fl)			\
	fface_set_flags (ff, ((enum fflag_e) fface_get_flags (ff) | (fl)))

/*
 * Compare two ffaces
 */
#define fface_squ(ff1,ff2)			\
	(((ff1).face == (ff2).face) && ((ff1).flags == (ff2).flags))

/*
 * Some predefined ffaces.
 */
extern struct fface_s String_fface;
extern struct fface_s Plain_fface;
extern struct fface_s Symbol_fface;
extern struct fface_s No_fface;

#endif /* !defined(_FFACES_H_) */
