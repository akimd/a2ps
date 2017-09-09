/* psgen.h - routines for the postscript generation
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

#ifndef _PSGEN_H_
#define _PSGEN_H_

struct a2ps_job;
struct encoding;

#include "faces.h"

void ps_print_char (struct a2ps_job * job,
			    int c, enum face_e face);
void ps_print_string (struct a2ps_job * job,
			      unsigned char * string, enum face_e face);
void ps_print_buffer (struct a2ps_job * job,
			      const unsigned char * buffer,
			      size_t start, size_t end,
			      enum face_e face);

void ps_begin_file (struct a2ps_job * job);
void ps_end_file (struct a2ps_job * job);

void page_flush (struct a2ps_job * job);
void require_fresh_page (struct a2ps_job * job);

/*
 * Multi-lingual routines
 */

void ps_set_encoding (struct a2ps_job * job,
			      struct encoding * encoding);
void ps_end_encoding (struct a2ps_job * job);
void ps_switch_encoding (struct a2ps_job * job,
				 struct encoding * encoding);

#endif
