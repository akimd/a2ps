/*
 * shmem.c
 *
 * liba2ps shared memory (data used by several modules)
 *
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
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

/*
 * $Id: common.c,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */

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
  NEW (struct a2ps_common_s, res);
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
