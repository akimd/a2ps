/* stream.h - open streams on pipes or files
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

#ifndef _STREAM_H_
#define _STREAM_H_
#include "xbackupfile.h"

struct stream
{
  bool is_file;	/* Is it a file (no : pipe)		*/
  FILE *fp;
};

/* Open for reading */
struct stream * stream_ropen (const char * command,
				      bool is_file);
/* Open for writing */
struct stream * stream_wopen (const char * command,
				      bool is_file);
/* Open for writing, using backup services */
struct stream * stream_wopen_backup (const char * command,
					     bool is_file,
					     enum backup_type backup_type);

/*
 * Uses the perl convention to know whether file or pipe
 * I.e., command starts by '>', or '|'
 */
struct stream * stream_perl_open_backup
   (char const * perl_command, enum backup_type backup,
	    const char **name);

/* Closes and frees.  */
void stream_close (struct stream * stream);

#endif
