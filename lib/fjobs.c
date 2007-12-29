/*
 * fjobs.c -- Recording information about the file jobs
 * Copyright (c) 1995-99 Akim Demaille, Miguel Santana
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
#include "jobs.h"
#include "fjobs.h"
#include "routines.h"

typedef struct file_job fjob_t;

/*
 * Creating/Removing a file_job named NAME, which is the NUM job,
 * which tmp file is in TMPDIR, and mod time is default to RUN_TM
 *
 */
fjob_t *
_a2ps_file_job_new (uchar *name, int num, struct tm *run_tm)
{
  NEW (fjob_t, res);

  /* liba2ps.h must not try to read a file.  This is the job of its
     clients.  Hence, make sure, at least, to have reasonable values.  */
  res->name = name;
  res->delegation_tmpname = NULL;
  res->stdin_tmpname = NULL;

  /* The type of the file is unknown */
  res->type = NULL;

  res->is_toc = false;

  /* Set date to now */
  res->mod_tm = *run_tm;

  /* If for some reason (is a dir, is not readable...), will
   * be later set to false */
  res->printable = true;

  /* By default it is not supposed to be stdin */
  res->is_stdin = false;

  /* Initialize the file dependant counters */
  /* Add one to job->sheets and pages, because this initialization
   * is done before the job went to the next page */

  res->first_sheet = 0;
  res->sheets = 0;
  res->last_sheet = 0;
  res->first_page = 0;
  res->pages = 0;
  res->last_page = 0;
  res->top_line = 1;
  res->top_page = 0;
  res->lines = 1;
  res->num = num;

  return res;
}

/*
 * Release a file_job
 */
void
file_job_free (fjob_t *file_job)
{
  XFREE (file_job->delegation_tmpname);
  XFREE (file_job->stdin_tmpname);
  free (file_job);
}

void
file_job_self_print (fjob_t *file, FILE *stream)
{
  fprintf (stream,
	   "File `%s': Pages %d-%d (%d), Sheets %d-%d (%d)\n",
	   file->name,
	   file->first_page, file->last_page, file->pages,
	   file->first_sheet, file->last_sheet, file->sheets);
  fprintf (stream,
	   "\tdel-tmp: `%s', stdin-tmp: `%s', is_toc: %d",
	   UNNULL (file->delegation_tmpname),
	   UNNULL (file->stdin_tmpname),
	   file->is_toc);
}

/*
 * Synchronise the current file informations onto JOB
 */
void
file_job_synchronize_sheets (struct a2ps_job * job)
{
  fjob_t * file = CURRENT_FILE (job);

  /* Set the number of sheets printed */
  if (!file->first_sheet)
    file->first_sheet = job->sheets;

  /* Is this actually a sheet number we never saw? */
  file->last_sheet = job->sheets;
  file->sheets = file->last_sheet - file->first_sheet + 1;
}

/*
 * Synchronise the current file informations onto JOB
 */
void
file_job_synchronize_pages (struct a2ps_job * job)
{
  fjob_t * file = CURRENT_FILE (job);

  /* Set the number of pages/lines printed */
  if (!file->first_page)
    file->first_page = job->pages;

  if (!file->top_page)
    file->top_page = 1;

  file->last_page = job->pages;
  file->top_line = CURRENT_FILE (job)->lines;

  file->pages = file->last_page - file->first_page + 1;
}

/*
 * Release the temp file associated to that file_job
 */
void
file_job_unlink_tmpfile (fjob_t * file)
{
  if (file->delegation_tmpname)
    unlink (file->delegation_tmpname);
  if (file->stdin_tmpname)
    unlink (file->stdin_tmpname);
}

/*
 * Compare two files upon the name
 */
int
file_name_cmp (fjob_t *f1, fjob_t *f2)
{
  return ustrcmp (f1->name, f2->name);
}
