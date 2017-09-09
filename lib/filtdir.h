/* filtdir.h - filter file names in directories
   Copyright 1995-2017 Free Software Foundation, Inc.

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

/* Akim Demaille <demaille@inf.enst.fr>
 * Based on savedir.c, written by David MacKenzie <djm@gnu.ai.mit.edu>. */

/* Functions mapped on file names. */
typedef void (*filterdir_fun_t)
  (const char *dir, const char *file, void *filtarg);

/* Functions used to filter the file names to keep. */
typedef bool (*filterdir_filter_t)
  (const char *dir, const char *file, void *filtarg);

void
filterdir (const char *dir,
		   filterdir_filter_t filter, void *filtarg,
		   filterdir_fun_t fun, void *arg);
