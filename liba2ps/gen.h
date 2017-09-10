/* gen.h - output formats handling
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

#ifndef _GEN_H_
#define _GEN_H_

struct a2ps_job;

enum output_format {
  eps, ps
};

const char * output_format_to_key (enum output_format format);

/* liba2ps.h:begin */
/* Print a single char C in FACE */
void a2ps_print_char (struct a2ps_job * job, 
			    int c, 
			    enum face_e face);
/* Print a C string (nul terminated) in FACE */
void a2ps_print_string (struct a2ps_job * job, 
			      const unsigned char * string, 
			      enum face_e face); 
/* Print the N chars contained in BUFFER, in FACE */
void a2ps_print_buffer (struct a2ps_job * job, 
			      const unsigned char * buffer, 
			      size_t start, size_t end,
			      enum face_e face);
/* Open/close the outer structure */
void a2ps_open_output_session (struct a2ps_job * job);
void a2ps_close_output_session (struct a2ps_job * job);

/* Open/Close the section structure */
void a2ps_open_input_session (struct a2ps_job * job, unsigned char * name);
void a2ps_close_input_session (struct a2ps_job * job);

/* liba2ps.h:end */

#endif
