/*
 * faces.h
 *
 * definition of the faces used by a2ps
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
 * $Id: faces.h,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
 */

#ifndef _FACES_H_
#define _FACES_H_

struct a2ps_job;
/*
 * Rationale for the values of the faces.
 * - We want to keep the interval as small as possible
 * - We want to be able to strip as fast as possible: masking.
 *
 * Therefore we want to have a single bit to test the comments.
 * We also want, for the same reason, Invisible to be 0.
 *
 * I came down to 6 bits.  If somebody has something better, 
 * I'll take it!
 * 5 4 3 2 1 0
 *           +- Weak/Strong
 *     +++++--- Keyword/Label/String/Symbol/Error
 *   +--------- Comment
 * +----------- Comment strong.
 * 
 * What gives:
 * 000000 Invisible			0
 * 000001 Plain				1
 * 000010 Keyword			2
 * 000011 Keyword_strong		3
 * 000100 Label				4
 * 000101 Label_strong			5
 * 000110 String			6
 * 001000 Symbol 			8
 * 001010 Error (the face "Error")	10
 * 010000 Comment			16
 * 100000 Comment_strong.		32
 *
 * Note that we can't use the bit Weak/Strong for the Comment Weak/Strong
 * (i.e., 01000/01001) because masking upon Comment_strong would
 * be positive with any Strong face, which is definitely not what we
 * want.
 */

/* liba2ps.h:begin */
/* 
 * Available faces.  No_face should never be given to liba2ps.
 */
enum face_e { 
  No_face = -1,
  First_face = 0,
  Plain = 0,
  Keyword = 1,
  Keyword_strong = 2,
  Label = 3,
  Label_strong = 4,
  String = 5,
  Symbol = 6,
  Error = 7,
  Comment = 8,
  Comment_strong = 9,
  Last_face = 9
};
/* liba2ps.h:end */


/* 
 * What must be the dimension of the array indexed
 * by the faces? I.e. last plus one.
 */
#define NB_FACES  10 /* Up to Comment_strong */

/* List of the faces really used */
extern enum face_e base_faces [];

enum face_e string_to_face PARAMS ((const char * string));
const char * face_eo_ps PARAMS ((enum face_e face));
const char * face_to_string PARAMS ((enum face_e face));
void face_self_print PARAMS ((enum face_e face, FILE * stream));

void init_face_eo_font PARAMS ((struct a2ps_job * job));
void face_eo_font_free PARAMS ((char * face_eo_font [NB_FACES]));
void check_face_eo_font PARAMS ((struct a2ps_job * job));
int face_eo_font_is_set PARAMS ((struct a2ps_job * job));
void face_set_font PARAMS ((struct a2ps_job * job,
			    enum face_e face, const char * font_name));
#endif
