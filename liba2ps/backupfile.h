/* backupfile.h - declarations for making emacs style backup file names
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

#ifndef BACKUPFILE_H_
# define BACKUPFILE_H_

/* When to make backup files. */
enum backup_type
{
  /* Never make backups. */
  none,

  /* Make simple backups of every file. */
  simple,

  /* Make numbered backups of files that already have numbered backups,
     and simple backups of the others. */
  numbered_existing,

  /* Make numbered backups of every file. */
  numbered
};

# define VALID_BACKUP_TYPE(Type)	\
  ((Type) == none			\
   || (Type) == simple			\
   || (Type) == numbered_existing	\
   || (Type) == numbered)

extern char const *simple_backup_suffix;

char *find_backup_file_name (char const *, enum backup_type);
enum backup_type get_version (char const *context, char const *arg);
enum backup_type xget_version (char const *context, char const *arg);
void addext (char *, char const *, int);

#endif /* ! BACKUPFILE_H_ */
