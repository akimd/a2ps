/*
 * generate.c -- Input files and pretty printing.
 *
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

#include "main.h"

extern int isdir PARAMS ((const char *path));

/* Name of the file in which the tmp sample file is stored. */
char *sample_tmpname = NULL;

/*
 * What kind of treatement should be applied
 */
enum style_kind_e
{
  no_style, binary, sshparser, unprintable, delegate
};

static enum style_kind_e
string_to_style_kind (const char * string)
{
  if (strequ (string, "binary"))
    return binary;
  else if (strequ (string, "UNPRINTABLE"))
    return unprintable;
  else if (strequ (string, "plain"))
    return no_style;
  else if (strequ (string, "delegate"))
    return delegate;
  return sshparser;
}
/************************************************************************/
/*			The inputs					*/
/************************************************************************/
static buffer_t *
input_new (uchar * name)
{
  NEW (buffer_t, res);
  struct file_job * file_job;
  struct stat statbuf;		/* to get file modification time */
  struct tm *tm;

  a2ps_open_input_session (job, name);
  file_job = CURRENT_FILE (job);

  /* Retrieve file modification date and hour */
  if (IS_EMPTY(name) || ustrequ (name, "-"))
    {
      file_job->is_stdin = true;
      file_job->name = job->stdin_filename;
      /* Create the buffer in charge of stdin */
      buffer_stream_set (res, stdin, end_of_line);
      /* Ask it to make a sample of the file. */
      tempname_ensure (sample_tmpname);
      buffer_sample_get (res, sample_tmpname);
    }
  else
    {
      FILE * input_stream;
      /* This is a true file (not stdin) */
      file_job->is_stdin = false;

      /* Printing a file given by its path */
      if (isdir ((char *) name))
	{
	  error (0, 0, _("`%s' is a directory"), quotearg ((char *) name));
	  file_job->printable = false;
	}

      file_job->name = (uchar *) name;
      if ((input_stream = fopen ((char *) name, "r")) == NULL)
	{
	  error (0, errno,
		 _("cannot open file `%s'"), quotearg ((char *) name));
	  file_job->printable = false;
	}
      else if (stat ((char *) name, &statbuf) == -1)
	{
	  error (0, errno, _("cannot get informations on file `%s'"),
		 quotearg ((char *) name));
	  file_job->printable = false;
	}
      else
	{
	  time_t tim = statbuf.st_mtime;
	  tm = localtime (&tim);
	  memcpy (&(file_job->mod_tm), tm, sizeof (*tm));
	}
      /* Create the buffer in charge of the input stream */
      buffer_stream_set (res, input_stream, end_of_line);
    }


  /*
   * What should be done out of this file?
   * Find the command associated to that file
   * - UNPRINTABLE for unprintable
   * - requested style sheet key
   * - style sheet key
   */
  if (!file_job->printable)
    file_job->type = "UNPRINTABLE";
  else if (!IS_EMPTY (style_request))
    file_job->type = style_request;
  else
    file_job->type = get_command (file_job->name,
				  (sample_tmpname
				   ? (uchar *) sample_tmpname
				   : file_job->name));

  /* Remove the sample file */
  if (sample_tmpname)
    unlink (sample_tmpname);
  return res;
}

static void
input_end (buffer_t * buffer)
{
  if (buffer->stream && buffer->stream!= stdin)
    fclose (buffer->stream);
  buffer_release (buffer);
  free (buffer);

  a2ps_close_input_session (job);
}

/************************************************************************/
/*			The producers					*/
/************************************************************************/
/*
 * Make on message on what we did
 */
static void
msg_file_pages_printed (a2ps_job * Job, const char * stylename)
{
  int sheets;

  sheets = CURRENT_FILE (Job)->sheets;
  if (Job->duplex)
    sheets = (sheets + 1) / 2;

  if (CURRENT_FILE (Job)->pages == 1)
    /* 1 page on 1 sheet */
    message (msg_report2,
	     (stderr, _("[%s (%s): 1 page on 1 sheet]\n"),
	      CURRENT_FILE (Job)->name,
	      stylename));
  else if (sheets == 1)
    /* several pages on 1 sheet */
    message (msg_report2,
	     (stderr, _("[%s (%s): %d pages on 1 sheet]\n"),
	      CURRENT_FILE (Job)->name,
	      stylename,
	      CURRENT_FILE (Job)->pages));
  else
    /* several sheets */
    message (msg_report2,
	     (stderr, _("[%s (%s): %d pages on %d sheets]\n"),
	      CURRENT_FILE (Job)->name,
	      stylename,
	      CURRENT_FILE (Job)-> pages,
	      sheets));
}

/*
 * Total printed
 */
void
msg_job_pages_printed (a2ps_job * Job)
{
  int sheets;
  uchar *ucp;

  sheets = Job->sheets;
  if (Job->duplex)
    sheets = (sheets + 1) / 2;

  /* Make a nice message to tell where the output is sent */
  ucp = a2ps_destination_to_string (Job);

  /* Report the pages */
  if (Job->pages == 1)
    /* 1 page on 1 sheet "sent to the default printer" etc. */
    message (msg_report1,
	     (stderr, _("[Total: 1 page on 1 sheet] %s\n"), ucp));
  else if (sheets == 1)
    /* several pages on 1 sheet */
    message (msg_report1,
	     (stderr, _("[Total: %d pages on 1 sheet] %s\n"),
	      Job->pages, ucp));
  else
    /* several sheets */
    message (msg_report1,
	     (stderr, _("[Total: %d pages on %d sheets] %s\n"),
	      Job-> pages, sheets, ucp));

  /* Report the number of lines that were too long. */
  if (macro_meta_sequence_get (Job, "cfg.wrapped")
      && Job->lines_folded)
    {
      if (Job->lines_folded == 1)
	message (msg_report1,
		 (stderr, _("[1 line wrapped]\n")));
      else
	message (msg_report1,
		 (stderr, _("[%d lines wrapped]\n"),
		  Job->lines_folded));
    }

  free (ucp);
}
/*
 * Total printed
 */
void
msg_nothing_printed (void)
{
  message (msg_report1,
	   (stderr, _("[No output produced]\n")));
}

void
print_toc (const uchar * name, const uchar * value, int * native_jobs)
{
  buffer_t toc_buffer;
  uchar * toc_content;

  /* Create a entry for the toc, as if it were a regular file */
  a2ps_open_input_session (job, xustrdup (name));
  /* But it is not a regular file: we need to be able to know
   * that it is indeed a toc, so that --pages=toc can be honored */
  CURRENT_FILE (job)->is_toc = true;

  austrcpy (toc_content,
	    expand_user_string (job, CURRENT_FILE (job),
				name, value));
  buffer_string_set (&toc_buffer, toc_content, end_of_line);

  /* We typeset it with PreScript */
  ssh_print_postscript (job, &toc_buffer, get_style_sheet ("pre"));
  (*native_jobs)++;

  a2ps_close_input_session (job);
}

/*
 * Called by the main loop.
 * The file to print is the last of the darray job->jobs.
 * Return true if was a success, false otherwise
 */
void
print (uchar * filename, int * native_jobs, int * delegated_jobs)
{
  char buf[512];
  struct delegation * contract = NULL;
  struct style_sheet * sheet;
  buffer_t * input_buffer;
  enum style_kind_e style_kind;
  struct file_job * file_job;

  /*
   * First, open that file and get info about it
   * It may seem useless in some cases (e.g. the file will be delegated)
   * but it ensures that readbility, and stat can be correctly done.
   */
  input_buffer = input_new (filename);

  /* Get the file_job _after_ it has been created by input_new */
  file_job = CURRENT_FILE (job);

  if (delegate_p
      && (contract =
	  get_subcontract (file_job->type,
			   output_format_to_key (job->output_format))))
    style_kind = delegate;
  else
    style_kind = string_to_style_kind (file_job->type);

  message (msg_file,
	   (stderr, "Getting ready to print file `%s', with command `%s'\n",
	    file_job->name, file_job->type));

  /*
   * Then do it
   */
  switch (style_kind)
    {
    case delegate:
      /* In ps generation, we must begin a new page */
      page_flush (job);
      sprintf (buf, _("%s, delegated to %s"),
	       file_job->type, contract->name);
      if (subcontract (file_job, input_buffer, contract))
	{
	  (*delegated_jobs)++;
	  msg_file_pages_printed (job, buf);
	}
      else
	message (msg_report2, (stderr, _("[%s (%s): failed.  Ignored]\n"),
			       file_job->name, buf));
      break;

    case unprintable:
      /* The job will not be processed correctly */
      message (msg_report2,
	       (stderr,
		_("[%s (unprintable): ignored]\n"), file_job->name));
      break;

    case binary:
      if (job->print_binaries)
	goto plain_print;

      message (msg_report2,
	       (stderr,
		_("[%s (binary): ignored]\n"), file_job->name));
      break;

    case sshparser:
      /* If highlight_level == none, don't */
      if (highlight_level == 0)
	goto plain_print;
      sheet = get_style_sheet (file_job->type);
      if (!sheet)
	goto plain_print;

      buffer_set_lower_case (input_buffer,
			     sheet->sensitiveness == case_insensitive);
      ssh_print_postscript (job, input_buffer, sheet);
      msg_file_pages_printed (job, (const char *) sheet->name);
      (*native_jobs)++;
      break;

    plain_print:
    case no_style:
      plain_print_postscript (job, input_buffer);
      msg_file_pages_printed (job, _("plain"));
      (*native_jobs)++;
      break;
    }

  input_end (input_buffer);
}

/*
 * Called by the main loop.
 * Almost like the above `PRINT' function, but just reports the guesses.
 * This is a dirty hack of sth OK in 4.11
 */
void
guess (uchar * filename)
{
  buffer_t * buffer;
  struct file_job * file_job;

  buffer = input_new (filename);
  file_job = CURRENT_FILE (job);
  printf ("[%s (%s)]\n", file_job->name, file_job->type);

  /* Close the files, free, and close the input session */
  if (buffer->stream && buffer->stream != stdin)
    fclose (buffer->stream);
}
