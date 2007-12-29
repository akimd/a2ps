/*
 * buffer.h - read line by line with conversion of EOL types
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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
 * $Id: buffer.h,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

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

const char * eol_to_string PARAMS ((enum eol_e eol));
enum eol_e option_string_to_eol PARAMS ((const char * option,
					 const char * arg));

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
  const uchar * buf;	/* != 0 -> buffer on a string		*/
  size_t bufsize;	/* size of buf				*/
  size_t bufoffset;	/* Used when a buffer string		*/
  FILE * stream;	/* != 0 -> a buffer on a stream		*/
  bool pipe_p;	/* true -> stream has been popened	*/

  uchar *content;	/* Exactly what is read			*/
  enum eol_e eol;	/* What is an end of line?		*/
  bool lower_case;
  uchar * value;	/* if LOWER_CASE, then lower case of content */
  size_t line;		/* Num of the current line 		*/
  size_t allocsize;	/* Used to know how big lower_case is 	*/
  size_t len;
  size_t curr;
  struct obstack obstack;
} buffer_t;

void buffer_stream_set PARAMS ((buffer_t * buffer,
				FILE * stream, enum eol_e eol));

void buffer_pipe_set PARAMS ((buffer_t * buffer,
			      FILE * stream, enum eol_e eol));

void buffer_string_set PARAMS ((buffer_t * buffer,
				const uchar * string, enum eol_e eol));

void buffer_buffer_set PARAMS ((buffer_t * buffer,
				const uchar * buf, size_t bufsize,
				enum eol_e eol));

void buffer_set_lower_case PARAMS ((buffer_t * buffer, bool lower_case));
void buffer_self_print PARAMS ((buffer_t * buffer, FILE * stream));

void buffer_release PARAMS ((buffer_t * buffer));

void buffer_get PARAMS ((buffer_t * buffer));

void buffer_sample_get PARAMS ((buffer_t * buffer, const char * filename));
void buffer_save PARAMS ((buffer_t * buffer, const char * filename));

#define buffer_is_empty(Buf) (Buf->curr >= Buf->len)
#endif
