/*
 * gen.h
 *
 * Output formats handling
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: gen.h,v 1.1.1.1.2.1 2007/12/29 01:58:18 mhatta Exp $
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

#ifndef _GEN_H_
#define _GEN_H_

struct a2ps_job;

enum output_format {
  eps, ps
};

const char * output_format_to_key PARAMS ((enum output_format format));

/* liba2ps.h:begin */
/* Print a single char C in FACE */
void a2ps_print_char PARAMS ((struct a2ps_job * job, 
			    int c, 
			    enum face_e face));
/* Print a C string (nul terminated) in FACE */
void a2ps_print_string PARAMS ((struct a2ps_job * job, 
			      const uchar * string, 
			      enum face_e face)); 
/* Print the N chars contained in BUFFER, in FACE */
void a2ps_print_buffer PARAMS ((struct a2ps_job * job, 
			      const uchar * buffer, 
			      size_t start, size_t end,
			      enum face_e face));
/* Open/close the outer structure */
void a2ps_open_output_session PARAMS ((struct a2ps_job * job));
void a2ps_close_output_session PARAMS ((struct a2ps_job * job));

/* Open/Close the section structure */
void a2ps_open_input_session PARAMS ((struct a2ps_job * job, uchar * name));
void a2ps_close_input_session PARAMS ((struct a2ps_job * job));

/* liba2ps.h:end */

#endif
