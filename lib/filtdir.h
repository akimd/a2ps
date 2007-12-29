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

/* Support of prototyping when possible */
#ifndef PARAMS
#  if PROTOTYPES
#    define PARAMS(protos) protos
#  else /* no PROTOTYPES */
#    define PARAMS(protos) ()
#  endif /* no PROTOTYPES */
#endif

/* Functions mapped on file names. */
typedef void (*filterdir_fun_t)
  PARAMS ((const char *dir, const char *file, void *filtarg));

/* Functions used to filter the file names to keep. */
typedef bool (*filterdir_filter_t)
  PARAMS ((const char *dir, const char *file, void *filtarg));

void
filterdir PARAMS ((const char *dir,
		   filterdir_filter_t filter, void *filtarg,
		   filterdir_fun_t fun, void *arg));
