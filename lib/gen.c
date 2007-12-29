/*
 * postscript.c
 *
 * routines for the postscript generation
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 * $Id: gen.c,v 1.1.1.1.2.1 2007/12/29 01:58:18 mhatta Exp $
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

#include "a2ps.h"		/* most global variables 		*/
#include "psgen.h"
#include "prolog.h"
#include "encoding.h"
#include "routines.h"		/* general interest routines		*/
#include "output.h"		/* Diverted outputs			*/
#include "media.h"
#include "faces.h"
#include "jobs.h"
#include "fjobs.h"

/*
 * Print just a char (may be nul)
 */
void
a2ps_print_char (a2ps_job * job, int c, enum face_e new_face)
{
  switch (job->output_format)
    {
    case ps:
      ps_print_char (job, c, new_face);
      break;

    default:
      abort ();
    }
}

/*
 * Print a string (\0 terminated)
 */
void
a2ps_print_string (a2ps_job * job, const uchar * string, enum face_e new_face)
{
  switch (job->output_format)
    {
    case ps:
      while (*string)
	ps_print_char (job, *(string++), new_face);
      break;

    default:
      abort ();
    }
}

/*
 * Print a buffer (char array of length N
 */
void
a2ps_print_buffer (a2ps_job * job,
		   const uchar * buffer,
		   size_t start, size_t end,
		   enum face_e new_face)
{
  size_t i;

  switch (job->output_format)
    {
    case ps:
      for (i = start ; i < end ; i++)
	ps_print_char (job, buffer [i], new_face);
      break;

    default:
      abort ();
    }
}

/*
 * Finish printing a file
 */
void
a2ps_open_output_session (a2ps_job * job)
{
  switch (job->output_format)
    {
    case ps:
      ps_begin (job);
      break;

    default:
      abort ();
    }
}

/*
 * Build the file struct for a new file
 */
void
a2ps_open_input_session (struct a2ps_job * job, uchar * name)
{
  struct file_job * file_job;

  file_job = _a2ps_file_job_new (IS_EMPTY (name) ? job->stdin_filename : name,
				 ++(job->total_files),
				 &job->run_tm);

  /* Reset the tags */
  * job->tag1 = '\0';
  * job->tag2 = '\0';
  * job->tag3 = '\0';
  * job->tag4 = '\0';

  switch (job->output_format)
    {
    case ps:
      ps_begin_file (job);
      break;

    default:
      abort ();
    }

  /* Link it */
  da_append (job->jobs, file_job);
}

/*
 * Finish printing a file
 */
void
a2ps_close_input_session (a2ps_job * job)
{
  /* Make sure that the output is really processed.  It
   * could have been closed by a page range selection, but
   * the trailer must be dumped */
/*  output_to_void (job->divertion, false);*/

  switch (job->output_format)
    {
    case ps:
      ps_end_file (job);
      break;

    default:
      abort ();
    }
}

/*
 * Finish a printing session
 */
void
a2ps_close_output_session (a2ps_job * job)
{
  switch (job->output_format)
    {
    case ps:
      ps_end (job);
      break;

    default:
      abort ();
    }
}

const char *
output_format_to_key (enum output_format format)
{
  switch (format)
    {
    case ps:
    case eps:
      return "ps";

    default:
      abort ();
    }
  return NULL;	/* For -Wall */
}
