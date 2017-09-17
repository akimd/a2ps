/* useropt.c - user option handling
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
