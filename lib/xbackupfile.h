/* xbackupfile.c -- Backup helping routines
   Copyright (C) 1990-1997, 1998 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Written by Akim Demaille <demaille@inf.enst.fr> */

#ifndef _XBACKUPFILES_H_
#define _XBACKUPFILES_H_

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#include "backupfile.h"

/* Non failing wrapping of find_backup_file_name */
char * xfind_backup_file_name PARAMS ((const char * file,
				       enum backup_type backup_type));

FILE * fopen_backup PARAMS ((const char * filename, 
			     enum backup_type backup_type));

int create_file_for_backup PARAMS ((char const *file, int oflag, mode_t mode,
				    enum backup_type backup_type));
#endif /* !_XBACKUPFILES_H_ */
