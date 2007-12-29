/*
 * psgen.h
 *
 * routines for the postscript output
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
 * $Id: psgen.h,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */

#ifndef _PSGEN_H_
#define _PSGEN_H_

struct a2ps_job;
struct encoding;

#include "faces.h"

void ps_print_char PARAMS ((struct a2ps_job * job,
			    int c, enum face_e face));
void ps_print_string PARAMS ((struct a2ps_job * job,
			      uchar * string, enum face_e face));
void ps_print_buffer PARAMS ((struct a2ps_job * job,
			      const uchar * buffer,
			      size_t start, size_t end,
			      enum face_e face));

void ps_begin_file PARAMS ((struct a2ps_job * job));
void ps_end_file PARAMS ((struct a2ps_job * job));

void page_flush PARAMS ((struct a2ps_job * job));
void require_fresh_page PARAMS ((struct a2ps_job * job));

/*
 * Multi-lingual routines
 */

void ps_set_encoding PARAMS ((struct a2ps_job * job,
			      struct encoding * encoding));
void ps_end_encoding PARAMS ((struct a2ps_job * job));
void ps_switch_encoding PARAMS ((struct a2ps_job * job,
				 struct encoding * encoding));

#endif
