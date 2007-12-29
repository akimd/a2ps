/*
 * caret.h
 *
 * How non printable chars are represented
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
 * $Id: caret.h,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */
#ifndef _CARET_H_
#define _CARET_H_

struct a2ps_job;

/* How are written non printable chars */
enum unprintable_format {
  caret, space, question_mark, octal, hexa, Emacs
};

const char * unprintable_format_to_string 
	PARAMS ((enum unprintable_format format));

/* Escape C, put result in RES, and return strlen RES */
int escape_unprintable PARAMS ((struct a2ps_job * job, 
				int c,
				uchar * res));

#endif
