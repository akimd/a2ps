/* common.c - liba2ps shared memory (data used by several modules)
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

#include <config.h>

#include "common.h"
#include "pathwalk.h"

/*
 * Default a shared mem values
 */
void
a2ps_common_reset (struct a2ps_common_s * common)
{
  common->path = NULL;
}

/*
 * Create a shared mem with default values
 */
struct a2ps_common_s *
a2ps_common_new (void)
{
  struct a2ps_common_s * res = XMALLOC (struct a2ps_common_s);
  a2ps_common_reset (res);
  return res;
}

/*
 * Free the common mem
 */
void
a2ps_common_free (struct a2ps_common_s * common)
{
  pw_free_path (common->path);
}

/*
 * Finalize the common mem to the regular values
 * HOME is the user's home dir.
 */
void
a2ps_common_finalize (struct a2ps_common_s * common, const char * home)
{
  char buf [512];

  /* Add the user's home .a2ps dir to the lib path unless explicitly
   * discarded with NO_HOME_CONF */
  if (! getenv ("NO_HOME_CONF"))
    {
      sprintf (buf, "%s%c.a2ps", home, DIRECTORY_SEPARATOR);
      common->path = pw_prepend_string_to_path (common->path, buf);
    }
}
