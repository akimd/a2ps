/* madir.c - madir direction
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
#include "madir.h"
#include "argmatch.h"
#include "routines.h"

/*
 * Return a madir mode
 */
static const char *const madir_args[] =
{
  "rows", "columns", 0
};

static madir_t madir_types[] =
{
  madir_rows, madir_columns
};

madir_t
madir_argmatch (const char * option, const char * arg)
{
  return XARGMATCH (option, arg, madir_args, madir_types);
}

const char *
madir_to_string (madir_t madir)
{
  switch (madir)
    {
    case madir_rows:
      return _("rows first");

    case madir_columns:
      return _("columns first");

    default:
      abort ();
    }
}
