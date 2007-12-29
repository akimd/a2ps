/*
 * useropt.c
 *
 * User Option handling
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
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
 * $Id: useropt.c,v 1.1.1.1.2.1 2007/12/29 01:58:25 mhatta Exp $
 */
#include "a2ps.h"
#include "jobs.h"
#include "useropt.h"
#include "routines.h"
#include "pair_ht.h"
#include "quotearg.h"

/*
 *  User options management
 */
struct pair_htable *
user_options_table_new (void)
{
  return pair_table_new ();
}

/*
 * Free a user option table
 */
void
user_options_table_free (struct pair_htable * table)
{
  pair_table_free (table);
}

/*
 * Add a custom option, making its own copy of the parameters
 */
void
user_option_add (a2ps_job * job, const char * key, const char * value)
{
  pair_add (job->user_options, key, value);
}

/*
 * Retrieve a user option
 */
char *
user_option_get (a2ps_job * job, char * shortcut)
{
  char * res;

  res = pair_get (job->user_options, shortcut);
  if (!res)
    error (1, 0, _("unknown user option `%s'"), quotearg (shortcut));

  return res;
}

/*
 * List the values defined
 */
void
user_options_list_short (a2ps_job * job, FILE * stream)
{
  fputs (_("Known User Options"), stream);
  putc ('\n', stream);
  pair_table_list_short (job->user_options, stream);
}

/*
 * List the values defined
 */
void
user_options_list_long (a2ps_job * job, FILE * stream)
{
  fputs (_("Known User Options"), stream);
  putc ('\n', stream);
  pair_table_list_long (job->user_options, stream);
}
