/*
 * filalign.c
 *
 * File alignment managing.
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 * $Id: filalign.c,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
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

#include "a2ps.h"
#include "filalign.h"
#include "argmatch.h"
#include "getnum.h"
#include <assert.h>

/*
 * Return a file_align mode
 */
static const char *const file_align_args[] =
{
  "sheet",
  "page", "no",
  "rank",
  "virtual", "fill", "yes",
  0
};

static file_align_t const file_align_types[] =
{
  file_align_sheet,
  file_align_page, file_align_page,
  file_align_rank,
  file_align_virtual, file_align_virtual, file_align_virtual
};

/* Return the file_align mode corresponding to ARG.
   FIXME: The error message is not clear enough. */

file_align_t
file_align_argmatch (const char * option, const char * arg)
{
  int i;

  ARGMATCH_ASSERT (file_align_args, file_align_types);

  i = ARGCASEMATCH (arg, file_align_args, file_align_types);
  if (i >= 0)
    return file_align_types[i];

  i = get_integer_in_range (option, arg, 1, 0, range_min);
  return i;
}

/* Return a human readable string describing the FILE_ALIGN mode. */

const char *
file_align_to_string (file_align_t file_align)
{
  static char buf[25];

  if (file_align < 0)
    return ARGMATCH_TO_ARGUMENT (file_align,
				 file_align_args, file_align_types);

  sprintf (buf, "%d", file_align);
  return buf;
}
