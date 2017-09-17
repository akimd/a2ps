/* xbackupfile.h - backup helping routines
   Copyright 1990-2017 Free Software Foundation, Inc.

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

/* Written by Akim Demaille <demaille@inf.enst.fr> */

#ifndef _XBACKUPFILES_H_
#define _XBACKUPFILES_H_

#include <stdio.h>
#include <sys/types.h>

#include "backupfile.h"

/* Non failing wrapping of find_backup_file_name */
char * xfind_backup_file_name (const char * file,
				       enum backup_type backup_type);

FILE * fopen_backup (const char * filename, 
			     enum backup_type backup_type);

int create_file_for_backup (char const *file, int oflag, mode_t mode,
				    enum backup_type backup_type);
#endif /* !_XBACKUPFILES_H_ */
