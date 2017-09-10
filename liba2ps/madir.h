/* madir.h - madir direction
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

#ifndef MADIR_H_
# define MADIR_H_ 1

/* Should the virtual page go first to the left, or the bottom? */
typedef enum madir_e
{
  madir_rows, madir_columns
} madir_t;

madir_t madir_argmatch (const char * option, const char * arg);
const char * madir_to_string (madir_t madir);
#endif /* !defined (MADIR_H_) */
