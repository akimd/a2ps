/* filtdir.c -- filter file names in directories

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Akim Demaille <demaille@inf.enst.fr>
 * Based on savedir.c, written by David MacKenzie <djm@gnu.ai.mit.edu>. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#ifdef CLOSEDIR_VOID
/* Fake a return value. */
#define CLOSEDIR(d) (closedir (d), 0)
#else
#define CLOSEDIR(d) closedir (d)
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif
#ifndef errno
extern int errno;
#endif

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define textdomain(Domain)
# define _(Text) Text
#endif

#if HAVE_STDBOOL_H
# include <stdbool.h>
#else
typedef enum {false = 0, true = 1} bool;
#endif

#include "error.h"
#include "darray.h"
#include "filtdir.h"
#include "xfnmatch.h"

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

  if (CLOSEDIR (dirp))
    error (1, errno, _("cannot close directory `%s'"), dir);
}
