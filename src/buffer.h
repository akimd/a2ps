/* buffer.h - read line by line with conversion of EOL types
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "xobstack.h"

/****************************************************************/
/*		 	buffer_t Service routines 		*/
/****************************************************************/
/*
 * How the eol should be handled
 */
enum eol_e
{
  eol_r, 	/* \r only */
  eol_n,	/* \n 	*/
  eol_rn,	/* \r\n	*/
  eol_nr,	/* \n\r */
  eol_auto	/* Any of the previous */
};

const char * eol_to_string (enum eol_e eol);
enum eol_e option_string_to_eol (const char * option,
					 const char * arg);

/*
 * The buffer structure, which encloses a lower case version to
 * speed up case insensitive parsing
 * There are two types:
 * - buf != NULL :    input comes from stream_string, read line by line.
 * - stream != NULL : input comes from stream, read line by line.
 * - both non NULL :  first read buf, then stream.
 *
 */
typedef struct buffer_s
{
  const unsigned char * buf;	/* != 0 -> buffer on a string		*/
  size_t bufsize;	/* size of buf				*/
  size_t bufoffset;	/* Used when a buffer string		*/
  FILE * stream;	/* != 0 -> a buffer on a stream		*/
  bool pipe_p;	/* true -> stream has been popened	*/

  unsigned char *content;	/* Exactly what is read			*/
  enum eol_e eol;	/* What is an end of line?		*/
  bool lower_case;
  unsigned char * value;	/* if LOWER_CASE, then lower case of content */
  size_t line;		/* Num of the current line 		*/
  size_t allocsize;	/* Used to know how big lower_case is 	*/
  size_t len;
  size_t curr;
  struct obstack obstack;
} buffer_t;

void buffer_stream_set (buffer_t * buffer,
				FILE * stream, enum eol_e eol);

void buffer_pipe_set (buffer_t * buffer,
			      FILE * stream, enum eol_e eol);

void buffer_string_set (buffer_t * buffer,
				const unsigned char * string, enum eol_e eol);

void buffer_buffer_set (buffer_t * buffer,
				const unsigned char * buf, size_t bufsize,
				enum eol_e eol);

void buffer_set_lower_case (buffer_t * buffer, bool lower_case);
void buffer_self_print (buffer_t * buffer, FILE * stream);

void buffer_release (buffer_t * buffer);

void buffer_get (buffer_t * buffer);

void buffer_sample_get (buffer_t * buffer, const char * filename);
void buffer_save (buffer_t * buffer, const char * filename);

#define buffer_is_empty(Buf) (Buf->curr >= Buf->len)
#endif
