/* filalign.c - file alignment managing
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

ARGMATCH_VERIFY(file_align_args, file_align_types);

/* Return the file_align mode corresponding to ARG.
   FIXME: The error message is not clear enough. */

file_align_t
file_align_argmatch (const char * option, const char * arg)
{
  int i = ARGMATCH (arg, file_align_args, file_align_types);
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
    return ARGMATCH_TO_ARGUMENT (&file_align,
				 file_align_args, file_align_types);

  sprintf (buf, "%d", file_align);
  return buf;
}
