/*
 * document.h
 *
 * handle report of various documenting formats.
 *
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
 * $Id: document.h,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

void
authors_print_plain PARAMS ((const uchar * authors, FILE * stream,
			     const char * before));
void
authors_print_html PARAMS ((const uchar * authors, FILE * stream,
			    const char * before));
void
authors_print_texinfo PARAMS ((const uchar * authors, FILE * stream,
			       const char * before));

typedef void (*documentation_fn_t) PARAMS ((const uchar * documentation,
				   const char * format,  FILE * stream)) ;

void
documentation_print_plain PARAMS ((const uchar * documentation,
				   const char * format,  FILE * stream));

void
documentation_print_html PARAMS ((const uchar * documentation, 
				  const char * format, FILE * stream));

void
documentation_print_texinfo PARAMS ((const uchar * documentation,
				     const char * format, FILE * stream));

#endif /* !defined(_DOCUMENT_H_) */
