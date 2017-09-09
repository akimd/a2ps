/* document.h - handle report of various documenting formats
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

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

void
authors_print_plain (const unsigned char * authors, FILE * stream,
			     const char * before);
void
authors_print_html (const unsigned char * authors, FILE * stream,
			    const char * before);
void
authors_print_texinfo (const unsigned char * authors, FILE * stream,
			       const char * before);

typedef void (*documentation_fn_t) (const unsigned char * documentation,
				   const char * format,  FILE * stream) ;

void
documentation_print_plain (const unsigned char * documentation,
				   const char * format,  FILE * stream);

void
documentation_print_html (const unsigned char * documentation, 
				  const char * format, FILE * stream);

void
documentation_print_texinfo (const unsigned char * documentation,
				     const char * format, FILE * stream);

#endif /* !defined(_DOCUMENT_H_) */
