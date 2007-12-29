/*
 * madir.c
 *
 * madir direction
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 * $Id: madir.c,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
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
#include "madir.h"
#include "argmatch.h"

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
  return XARGCASEMATCH (option, arg, madir_args, madir_types);
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
