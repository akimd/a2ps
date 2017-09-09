/* caret.h - how non printable chars are represented
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

#ifndef _CARET_H_
#define _CARET_H_

struct a2ps_job;

/* How are written non printable chars */
enum unprintable_format {
  caret, space, question_mark, octal, hexa, Emacs
};

const char * unprintable_format_to_string 
	(enum unprintable_format format);

/* Escape C, put result in RES, and return strlen RES */
int escape_unprintable (struct a2ps_job * job, 
				int c,
				unsigned char * res);

#endif
