/*
 * pathwalk.c -- functions for ooking for files, reading files etc.
 *
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
 */

/*
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

/* I know this file would need a full rewrite, nevertheless, since
   some day we should use kpathsea, it would be a waste of time.
   There are other files to rewrite :). */

#include "a2ps.h"
#include "pathwalk.h"
#include "darray.h"
#include "message.h"
#include "routines.h"
#include "filtdir.h"
#include "lister.h"
#include "strverscmp.h"
#include "quotearg.h"
#include "dirname.h"

/*---------------------------------.
| Alloca D to contain "DIR/FILE".  |
`---------------------------------*/
#define apathconcat(D,Dir,File)				\
   do {							\
	char * tmp1 = (char *) (Dir);			\
	const char * tmp2 = (const char *) (File);	\
	D = ALLOCA (char, (strlen (tmp1) 		\
			     + strlen (tmp2) + 2));	\
	tmp1 = stpcpy (D, tmp1);			\
        (*tmp1++) = DIRECTORY_SEPARATOR;		\
        stpcpy (tmp1, tmp2);				\
   } while (0)

/************************************************************************/
/*	Handling the path: an array, NULL terminated of char *		*/
/************************************************************************/
static char **
pw_internal_string_to_path (const char * path, char sep, int * length)
{
  char **res = NULL;
  int allocated = 5;	/* num of entries yet allocated in res	*/
  int entries = 0;
  const char *cp, *cp2;
  int len;

  res = XCALLOC (char *, allocated);
  for (cp = path; cp; cp = strchr (cp, sep))
    {
      if (cp != path)
        cp++;

      cp2 = strchr (cp, sep);
      if (cp2)
        len = cp2 - cp;
      else
        len = strlen (cp);

      if (len == 0)
	{
	/* Skip empty entries */
        cp++;
        continue;
	}
      else
	{
	  /* Make sure _not_ to include that last DIRECTORY_SEPARATOR */
	  if (cp [len] == DIRECTORY_SEPARATOR)
	    len --;
	}

      res [ entries ] = XMALLOC (char, len + 1);
      strncpy (res [entries],  cp, len);
      res [entries] [len] = '\0';

      entries++;
      if (entries >= allocated)
	{
	  allocated *= 2;
	  res = XREALLOC (res, char *, allocated);
	}
    }
  *length = entries;

  /* Make it null-terminated, and exactely that size */
  res [*length] = NULL;
  res = XREALLOC (res, char *, *length + 1);
  return res;
}

/*
 * Length of a path
 */
static inline int
pw_path_length (char ** path)
{
  int res;

  if (!path)
    return 0;

  for (res = 0 ; path [res] ; res ++)
    /* Nada */;
  return res;
}

/*-----------------------------------------------------------------.
| Build a path as array from a PATH as string, given the separator |
| PATH is read only.                                               |
`-----------------------------------------------------------------*/

char **
pw_string_to_path (const char * path)
{
  int dummy;
  return pw_internal_string_to_path (path, PATH_SEPARATOR, &dummy);
}

/*
 * Concat PATH2 to PATH1, and return the result.  Free PATH2
 */

static inline char **
pw_path_concat (char ** path1, int len1, char ** path2, int len2)
{
  int i;

  if (path2)
    {
      path1 = XREALLOC (path1, char *, len1 + len2 + 1);
      for (i = 0 ; i <= len2 ; i++)
	path1 [len1 + i] = path2 [i];
      free (path2);
    }

  return path1;
}

/*-------------------------------------.
| Append a string-path DIR2 to PATH1.  |
`-------------------------------------*/

char **
pw_append_string_to_path (char ** path1, const char * dir2)
{
  int len1, len2;
  char ** path2;

  len1 = pw_path_length (path1);
  path2 = pw_internal_string_to_path (dir2, PATH_SEPARATOR, &len2);

  return pw_path_concat (path1, len1, path2, len2);
}

/*--------------------------------------.
| Prepend a string-path DIR2 to PATH1.  |
`--------------------------------------*/
char **
pw_prepend_string_to_path (char ** path1, const char * dir2)
{
  int len1, len2;
  char ** path2;

  len1 = pw_path_length (path1);
  path2 = pw_internal_string_to_path (dir2, PATH_SEPARATOR, &len2);

  return pw_path_concat (path2, len2, path1, len1);
}

/*
 * Free a path array, and its content
 */
void
pw_free_path (char ** path)
{
  int i;
  if (path)
    for (i = 0 ; path[i] ; i++)
      free (path[i]);
  XFREE (path);
}

void
pw_fprintf_path (FILE * stream, const char * format, char * const * path)
{
  if (path)
    while (*path) {
      fprintf (stream, format, *path);
      path++;
    }
}

/*-------------------------------------------------------------------.
| Return the index+1 in PATH of the directory that contains the file |
| concat(NAME, SUFFIX).                                              |
`-------------------------------------------------------------------*/

static int
pw_find_file_index (char * const * path,
		    const char *name, const char *suffix)
{
  int i;
  struct stat stat_st;
  char * filename, * fullpath;

  if (suffix)
    astrcat2 (filename, name, suffix);
  else
    filename = (char *) name;

  message (msg_pw,
	   (stderr, "pw: looking for `%s'\n", filename));

  if (path)
    for (i = 0 ; path [i] ; i ++)
      {
	apathconcat (fullpath, path [i], filename);
	if (stat (fullpath, &stat_st) == 0)
	  {
	    /* File exists */
	    message (msg_pw, (stderr, "pw: success in %s\n", path[i]));
	    return i + 1;
	  }
      }

  if (msg_test (msg_pw))
    {
      fprintf  (stderr, "pw: did not find `%s' in path\n", filename);
      pw_fprintf_path (stderr, "pw:  %s\n", path);
    }

  return 0;
}

/*--------------------------------.
| Return non 0 if the file exists |
`--------------------------------*/

int
pw_file_exists_p (char * const * path,
		    const char *name, const char * suffix)
{
  return pw_find_file_index (path, name, suffix);
}

/*------------------------------------------------------------------.
| Return the malloc'd full path of existing file named concat(NAME, |
| SUFFIX).                                                          |
`------------------------------------------------------------------*/

static inline char *
_pw_find_file (char * const * path,
	       const char * name, const char * suffix)
{
  char * res;
  int i;

  i = pw_find_file_index (path, name, suffix);

  if (i)
    {
      /* Return a malloc'ed full file name */
      if (suffix)
	{
	  res = XMALLOC (char,
			 strlen (path[i-1]) + 2
			 + strlen (name) + strlen (suffix));
	  sprintf (res, "%s%c%s%s", path [i-1], DIRECTORY_SEPARATOR,
		   name, suffix);
	}
      else
	{
	  res = XMALLOC (char,
			 strlen (path[i-1]) + 2
			 + strlen (name));
	  sprintf (res, "%s%c%s", path [i-1], DIRECTORY_SEPARATOR,
		   name);
	}
      return res;
    }
  else
    return NULL;
}

/*
 * Inline wrapper
 */
char *
pw_find_file (char * const * path,
	      const char * name, const char * suffix)
{
  return _pw_find_file (path, name, suffix);
}

/* Return the malloc'd full path of existing file named concat(NAME,
   SUFFIX), exits on failure. */

char *
xpw_find_file (char * const * path,
	       const char * name, const char * suffix)
{
  char * res = _pw_find_file (path, name, suffix);

  if (!res)
    {
      char *file;
      file = ALLOCA (char, strlen (name) + (suffix ? strlen (suffix) : 0) + 1);
      sprintf (file, "%s%s",  name, UNNULL (suffix));
      error (1, errno, _("cannot find file `%s'"), quotearg (file));
    }
  return res;
}

/* Idem, but look first around the given INCLUDING_FILE. */

char *
xpw_find_included_file (char * const *path,
			const char *including_file,
			const char *name, const char *suffix)
{
  char *dir;	/* Of the including file. */
  char *res;
  struct stat statbuf;

  if (*name == DIRECTORY_SEPARATOR)
    /* Path is absolute */
    dir = NULL;
  else
    /* Relative.  Give its root. */
    dir = dir_name (including_file);

  res = ALLOCA (char, (strlen (dir)
		       + strlen (name)
		       + (suffix ? strlen (suffix) : 0)
		       + 2));
  sprintf (res, "%s%c%s%s", dir, DIRECTORY_SEPARATOR,
	   name, suffix ? suffix : "");
  XFREE (dir);
  if (stat (res, &statbuf) == 0)
    return xstrdup (res);

  /* Find in the library. */
  return xpw_find_file (path, name, suffix);
}

/*
 * Dump a library file content
 */
int
pw_paste_file (char * const * path,
	       const char * name, const char * suffix)
{
  char buf[512];
  char * fullpath;
  FILE * fp;
  int line = 0;

  message (msg_pw,
	   (stderr, "pw: pasting `%s%s'\n", name, suffix ? suffix : ""));

  fullpath = _pw_find_file (path, name, suffix);

  if (!fullpath)
    return 0;

  fp = fopen (fullpath, "r");

  if (fp == NULL)
    return 0;

  /* Find the end of the header. */
#define HDR_TAG "% -- code follows this line --"
  while ((fgets (buf, sizeof (buf), fp)))
    {
      line++;
      if (strnequ (buf, HDR_TAG, strlen (HDR_TAG)))
	break;
    }

  /* Dump rest of file. */
#define INCL_TAG "% -- include file:"
  while ((fgets (buf, sizeof (buf), fp)))
    {
      line++;
      if (strnequ (buf, INCL_TAG, strlen (INCL_TAG)))
	{
	  char * file = buf + strlen (INCL_TAG);
	  file = strtok (file, " \n\t");
	  message (msg_pw,
		   (stderr,
		    "pw: including file '%s' upon request given in '%s':%d\n",
		    file, fullpath, line));
	  if (!pw_paste_file (path, file, NULL))
	    error_at_line (1, errno, fullpath, line,
			   _("cannot find file `%s'"), quotearg (file));
	  continue;
	}
      fputs (buf, stdout);
    }

  fclose (fp);
  free (fullpath);
  return 1;
}

/* Helping functions for pw_glob. */

static bool
pw_filter_fnmatch (PARAM_UNUSED const char * dir, const char *file,
		   const char *pattern)
{
  return !fnmatch (pattern, file, 0);
}

static void
pw_filter_da_append (PARAM_UNUSED const char * dir, const char *file,
		     struct darray *da)
{
  da_append (da, xstrdup (file));
}

static void
pw_filter_print (const char * dir, const char *file, FILE *stream)
{
  fprintf (stream, "%s%c%s\n", dir, DIRECTORY_SEPARATOR, file);
}

static void
pw_filterdir (char * const * path,
	      filterdir_filter_t filter, void *filtarg,
	      filterdir_fun_t fun, void *arg)
{
  for (/* Nothing */ ; *path ; path++)
    filterdir (*path, filter, filtarg, fun, arg);
}


/*------------------------------------------------------------------.
| Call glob on PATTERN in each dir of PATH.  Return a malloc'd char |
| ** (char * malloc'ed too).                                        |
`------------------------------------------------------------------*/

struct darray *
pw_glob (char * const * path, const char * pattern)
{
  struct darray * res;

  if (msg_test (msg_pw))
    {
      fprintf  (stderr, "pw: globbing `%s'\n", pattern);
      pw_fprintf_path (stderr, "\t-> %s\n", path);
    }

  res = da_new ("Dir entries", 20,
		da_geometrical, 2,
		(da_print_func_t) da_str_print,
		(da_cmp_func_t) strverscmp);

  pw_filterdir (path,
		(filterdir_filter_t) pw_filter_fnmatch, (void *) pattern,
		(filterdir_fun_t) pw_filter_da_append, res);

  da_qsort (res);
  da_unique (res, (da_map_func_t) free);

  return res;
}

void
pw_glob_print (char * const * path, const char * pattern, FILE *stream)
{
  pw_filterdir (path,
		(filterdir_filter_t) pw_filter_fnmatch, (void *) pattern,
		(filterdir_fun_t) pw_filter_print, stream);
}

/*
 * Cut the suffix of a string (i.e. cut at last `.')
 */

static void
da_str_cut_suffix (char * string)
{
  *strrchr (string, '.') = '\0';
}

/*----------------------------------------------------------------.
| Return malloc'd array of malloc'ed char * of the prefix part of |
| the file having SUFFIX as suffix in PATH.                       |
`----------------------------------------------------------------*/

struct darray *
pw_glob_on_suffix (char * const * path, const char * suffix)
{
  struct darray * res;
  char * pattern;

  /* Build the pattern and glob */
  astrcat2 (pattern, "*", suffix);
  res = pw_glob (path, pattern);

  /* Cut the suffixes */
  da_map (res, (da_map_func_t) da_str_cut_suffix);

  return res;
}

/* Use lister to report on STREAM the list of files in PATH that end
   by SUFFIX. */

void
pw_lister_on_suffix (FILE * stream, char * const * path, const char * suffix)
{
  struct darray * entries;

  entries = pw_glob_on_suffix (path, suffix);
  lister_fprint_vertical (NULL, stream,
			  (void *) entries->content, entries->len,
			  (lister_width_t) strlen,
			  (lister_print_t) fputs);
  da_free (entries, (da_map_func_t) free);
}
