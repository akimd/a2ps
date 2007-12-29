/*
 * pathwalk.h
 *
 * functions for ooking for files, reading files etc.
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 * $Id: pathwalk.h,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
 */

/*
 * This file is part of a2ps.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _PATHWALK_H_
#define _PATHWALK_H_

/*
 * Build a path (as array null terminated) out of path as string
 * and the separator.
 */
char ** pw_string_to_path PARAMS ((const char * string));
char ** pw_append_string_to_path PARAMS ((char ** path1,
					  const char * dir2));
char ** pw_prepend_string_to_path PARAMS ((char ** path1,
					   const char * dir2));
void pw_free_path PARAMS ((char ** path));
void pw_fprintf_path PARAMS ((FILE * stream,
			   const char * format, char * const * path));

/*
 * Dump NAME.SUFFIX in the PATH on STDOUT
 */
int pw_paste_file PARAMS ((char * const * path,
			   const char *name, const char *suffix));

/*
 * Is the file CONTEXT somewhere in PATH
 */
int pw_file_exists_p PARAMS ((char * const * path,
			      const char *name, const char *suffix));

/*
 * Return malloc'ed path to NAME.SUFFIX if in PATH, NULL otherwise
 */
char * pw_find_file
  PARAMS ((char * const * path, const char *name, const char *suffix));
/* Idem, but exits upon failure */
char * xpw_find_file
  PARAMS ((char * const * path, const char *name, const char *suffix));

/* Idem, but look first if the file is not in the same dir as
   INCLUDING_FILE. */

char * xpw_find_included_file
  PARAMS ((char * const *path, const char *including_file,
	   const char *name, const char *suffix));
/*
 * Call glob on PATTERN in each dir of PATH.
 * Return a malloc'd char ** (char * malloc'ed too)
 */
struct darray * pw_glob PARAMS ((char * const * path,
				 const char * pattern));
void pw_glob_print PARAMS ((char * const * path,
			    const char * pattern, FILE *stream));

struct darray * pw_glob_on_suffix PARAMS ((char * const * path,
					   const char * suffix));

/* Use lister to report on STREAM the list of files in PATH that end
   by SUFFIX. */

void pw_lister_on_suffix PARAMS ((FILE * stream,
				  char * const * path, const char * suffix));

#endif /* not defined _PATHWALK_H_ */
