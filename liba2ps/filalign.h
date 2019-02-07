/* filalign.h - file alignment management
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

#ifndef FILALIGN_H_
# define FILALIGN_H_ 1

/* How different files share the medium. */

typedef enum
  {
   /* Always on different sheets. */
   file_align_sheet = -1,

   /* Always on different pages. */
   file_align_page = -2,

   /* Always on different rank (i.e., columns or rows depending upon
      the madir). */
   file_align_rank = -3,

   /* Put as many files as you can per page. */
   file_align_virtual = -4
  }
  file_align_t;

/* Positive values are allowed and then means that each first *page*
   of a file must start on a page which is (a multiple of that number)
   plus one.  For instance, for `2', the files must start on odd
   pages.

   This is to be compatible with enscript. */

file_align_t file_align_argmatch
  (const char * option, const char * arg);

const char * file_align_to_string
  (file_align_t file_align);
#endif /* !defined (MADIR_H_) */
