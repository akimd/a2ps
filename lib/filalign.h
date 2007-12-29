/*
 * filalign.h
 *
 * madir direction
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 * $Id: filalign.h,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
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

#ifndef FILALIGN_H_
# define FILALIGN_H_ 1

/* How different files share the medium. */

typedef int file_align_t;

/* Always on different sheets. */
#define file_align_sheet -1

/* Always on different pages. */
#define file_align_page -2

/* Always on different rank (i.e., columns or rows depending upon the
   madir). */
#define file_align_rank -3

/* Put as many files as you can per page. */
#define file_align_virtual -4

/* Positive values are allowed and then means that each first *page*
   of a file must start on a page which is (a multiple of that number)
   plus one.  For instance, for `2', the files must start on odd
   pages.

   This is to be compatible with enscript. */

file_align_t file_align_argmatch
  PARAMS ((const char * option, const char * arg));

const char * file_align_to_string
  PARAMS ((file_align_t file_align));
#endif /* !defined (MADIR_H_) */
