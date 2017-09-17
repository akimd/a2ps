/* filtdir.c - filter file names in directories
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

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <fnmatch.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)

#include "error.h"
#include "darray.h"
#include "filtdir.h"
#include "routines.h"

/* Is NAME . or ..? */
#define IS_NOTDOTDOT(Name)			\
  (Name[0] != '.'				\
   || (Name[1] != '\0'				\
       && (Name[1] != '.' || Name[2] != '\0')))

/*----------------------------------------------------------------.
| Perform a call to FUN (FILE, DIR, ARG) on each FILE in DIR that |
| statisfies a call to FILTER (FILE, DIR, FILTARG).               |
`----------------------------------------------------------------*/

void
filterdir (const char *dir,
	   filterdir_filter_t filter, void *filtarg,
	   filterdir_fun_t fun, void *arg)
{
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir (dir);
  if (dirp == NULL)
    return;
/*    error (1, errno, _("cannot open directory `%s'"), dir);*/

  while ((dp = readdir (dirp)) != NULL)
    /* Skip "." and ".." (some NFS filesystems' directories lack them). */
    if (IS_NOTDOTDOT (dp->d_name)
	&& (!filter || (*filter) (dir, dp->d_name, filtarg)))
      fun (dir, dp->d_name, arg);

  if (closedir (dirp))
    error (1, errno, _("cannot close directory `%s'"), dir);
}
