/*
 * delegate.c
 *
 * Handling the delegations
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
 *
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
#include "output.h"
#include "delegate.h"
#include "message.h"
#include "routines.h"
#include "metaseq.h"
#include "dsc.h"
#include "buffer.h"
#include "lister.h"
#include "quotearg.h"

/* Priviledge access to job and delegations */
extern struct a2ps_job *job;
extern struct hash_table_s *delegation_table;

/************************************************************************/
/*      Handling of the hash table of the delegations                   */
/************************************************************************/
/*
 * The hashing is done on the contract, not the name (who
 * cares the name of the contract!)
 */
static unsigned long
delegate_hash_1 (void const *key)
{
  return_STRING_HASH_1 (((const struct delegation *) key)->contract);
}

static unsigned long
delegate_hash_2 (void const *key)
{
  return_STRING_HASH_2 (((const struct delegation *) key)->contract);
}

static int
delegate_hash_cmp (void const *x, void const *y)
{
  return_STRING_COMPARE (((const struct delegation *) x)->contract,
			 ((const struct delegation *) y)->contract);
}

static int
delegate_hash_qcmp (const struct delegation **x,
		    const struct delegation **y)
{
  return_STRING_COMPARE ((*x)->name, (*y)->name);
}

static size_t
delegate_name_len (struct delegation *delegation)
{
  return strlen (delegation->name);
}

static void
delegate_name_fputs (struct delegation *delegation, FILE * stream)
{
  fputs (delegation->name, stream);
}

static void
delegate_free (struct delegation *delegation)
{
  free (delegation->name);
  free (delegation->contract);
  free (delegation->command);
  free (delegation);
}

/*
 * Create the table handling the subcontracts
 */
struct hash_table_s *
delegation_table_new (void)
{
  NEW (struct hash_table_s, res);
  hash_init (res, 8,
	     delegate_hash_1, delegate_hash_2, delegate_hash_cmp);
  return res;
}

/*
 * Free the whole table
 */
void
delegation_table_free (struct hash_table_s *table)
{
  hash_free (table, (hash_map_func_t) delegate_free);
  free (table);
}

/************************************************************************/
/*              Use of the subcontracts                                 */
/************************************************************************/
/*
 * Read a (PS) file, and extract the NeededResources etc.
 * To include in the whole file Prolog.
 * Get the number of pages too.
 */

/*
 * Read a line "Delegation: <CONTRACT_LINE>" in a config file
 */
#define error_if_null(_str_)						\
  if (_str_ == NULL) 							\
    error_at_line (1, 0, filename, line,				\
		   _("missing argument for `%s'"), quotearg (contract_line));

void
add_delegation (const char *filename, int line,
		char *contract_line)
{
  char *cp, *cp2;
  struct delegation *contract;

  contract = XMALLOC (struct delegation, 1);

  /* Structure of the line:
     <name of contract> <source type>:<destination type> <command> */
  cp = strtok (contract_line, " \t\n");
  error_if_null (cp);
  contract->name = xstrdup (cp);

  cp = strtok (NULL, " \t\n:");
  error_if_null (cp);
  cp2 = strtok (NULL, " \t\n");
  error_if_null (cp2);
  contract->contract = XMALLOC (char, strlen (cp) + strlen (cp2) + 2);
  sprintf (contract->contract, "%s:%s", cp, cp2);

  cp = strtok (NULL, "\n");
  error_if_null (cp);
  contract->command = xstrdup (cp + strspn (cp, "\t "));

  /* Put it in the table */
  hash_insert (delegation_table, contract);
}


/*
 * Return the subcontract if there is, otherwise NULL
 */
struct delegation *
get_subcontract (const char *src_type, const char *dest_type)
{
  struct delegation token;

  token.contract = ALLOCA (char, strlen (src_type) + strlen (dest_type) + 2);
  sprintf (token.contract, "%s:%s", src_type, dest_type);

  return (struct delegation *) hash_find_item (delegation_table, &token);
}

/*
 * Return the command associated a sub contract.
 * if EVALUATE, return the evaluated command.
 * The result is not malloc'ed, and must be used before
 * any other call to expand_user_string.
 */
char *
get_delegate_command (struct delegation *contract,
		      struct file_job *file,
		      int evaluate)
{
  if (evaluate)
    return (char *) expand_user_string (job, file,
					(const uchar *) "delegating command",
					(const uchar *) contract->command);
  else
    return contract->command;
}

/*
 * Subcontract FILE to CONTRACTOR
 * This should produce a tmp file, then insert in the OUTPUT struture
 * the routine to dump and remove it.
 * Return true open success, false otherwise
 */
enum continuation_e
{
  no_continuation, needed_resource
};

int
subcontract (struct file_job *fjob, buffer_t * buffer,
	     struct delegation *contractor)
{
  char *command, *stdin_content_filename = NULL;
  FILE *in_stream, *out_stream;
  int lines_read = 0;
  char buf[512];

  /* Here we store the type of the last %%??Resource: tag we saw,
   * to be ready to handle continuation (%%+ ) */
  enum continuation_e continuation = no_continuation;

  /* This is an awful kludge.  I dunno how to do it nicely...  The
     problem is that a2ps can be fed by stdin, but delegations cannot.
     So we first dump stdin into a temporary file.  */
  if (fjob->name == job->stdin_filename)	/* not strcmp */
    {
      /* Dump the content of the buffer */
      tempname_ensure (fjob->stdin_tmpname);
      stdin_content_filename = fjob->stdin_tmpname;
      buffer_save (buffer, stdin_content_filename);

      /* We change the name of the file so that the correct file name
	 is used in the command (that of the temporary file containing
	 stdin). */
      fjob->name = (uchar *) stdin_content_filename;
      command = get_delegate_command (contractor, fjob, 1);
      fjob->name = job->stdin_filename;
    }
  else
    {
      command = get_delegate_command (contractor, fjob, 1);
    }

  /* First, before it break :), say what you do */
  message (msg_file,
	   (stderr, "Delegating `%s' to `%s' (%s)\n",
	    fjob->name, contractor->name, command));

  /* Open a pipe from the delegation, and the temp file in which
   * the result is stored */
  tempname_ensure (fjob->delegation_tmpname);
  out_stream = fopen (fjob->delegation_tmpname, "w");
  if (!out_stream)
    {
      error (0, errno, _("cannot create file `%s'"),
	     quotearg (fjob->delegation_tmpname));
      return false;
    }

  in_stream = popen (command, "r");
  if (!in_stream)
    {
      fclose (out_stream);
      error (0, errno, _("cannot open a pipe on `%s'"),
	     quotearg (command));
      return false;
    }

  /* Make the file know its first page/sheet */
  file_job_synchronize_sheets (job);
  file_job_synchronize_pages (job);

  /* Now, read the file, update the PS info, and store the result in
   * out_stream */
  while (fgets (buf, sizeof (buf), in_stream))
    {
      /* This is not exactely the number of lines,
       * but anyway it is only used to be sure something was read,
       * to track the failure of a delegation */
      lines_read++;
#define PAGE_TAG	"%%Page: "
      if (strprefix (PAGE_TAG, buf))
	{
	  /* We suppose that it has respected the number of virtual
	   * pages per sheet */
	  job->pages += job->rows * job->columns;
	  job->sheets++;
	  file_job_synchronize_sheets (job);
	  file_job_synchronize_pages (job);
	}
#define NEEDED_RES_TAG	"%%DocumentNeededResources: "
      else if (strprefix (NEEDED_RES_TAG, buf))
	{
	  /* The needed resources must be included too.
	   * Take care of the %%+ continuation tag */
	  char *value, *res, *buf_copy;

	  continuation = needed_resource;
	  astrcpy (buf_copy, buf + strlen (NEEDED_RES_TAG));
	  res = strtok (buf_copy, " \n\t");

	  /* This while saves us from a special case of
	   * %%DocumentNeededResources: (atend)
	   */
	  while ((value = strtok (NULL, " \n\t")))
	    add_needed_resource (job, res, value);
	}
#define CONTINUATION_TAG	"%%+ "
      else if (strprefix (CONTINUATION_TAG, buf))
	{
	  char *value, *res, *buf_copy;

	  astrcpy (buf_copy, buf + strlen (CONTINUATION_TAG));
	  res = strtok (buf_copy, " \n\t");
	  while ((value = strtok (NULL, " \n\t")))
	    switch (continuation)
	      {
	      case needed_resource:
		add_needed_resource (job, res, value);
		break;

	      default:
		break;
	      }
	}

      /* The content should be left untouched */
      fputs (buf, out_stream);
    }

  pclose (in_stream);
  fclose (out_stream);

  /* If a temporary file was created to deal with stdin, unlink it. */
  if (stdin_content_filename)
    unlink (stdin_content_filename);

  /* FIXME: This is a trial to see when there is an error */
  if (lines_read == 0)
    return false;

  /* This one must not be cut by the page selection */
  {
    int saved_redirection_of_output;
    saved_redirection_of_output = output_is_to_void (job->divertion);
    output_to_void (job->divertion, false);

    /* Protect the rest of the file and give sane environment */
    output (job->divertion, "BeginInclude\n");
    output (job->divertion, "%%%%BeginDocument: %s\n", fjob->name);
    output_delayed_routine (job->divertion,
                            (delayed_routine_t) stream_dump,
                            (void *) fjob->delegation_tmpname);
    /* remove the file after its use */
    output_delayed_routine (job->divertion,
                            (delayed_routine_t) unlink2,
                            (void *) fjob->delegation_tmpname);
    output (job->divertion, "%%%%EndDocument\n");
    output (job->divertion, "EndInclude\n");

    output_to_void (job->divertion, saved_redirection_of_output);
  }

  /* The pages are no longer ordered.  Respect DSC */
  job->status->page_are_ordered = false;
  return true;
}

/************************************************************************/
/*              "Visible" interface of the subcontracts                 */
/************************************************************************/
/*
 * Print a single contract on STREAM
 */
static void
dump_contract (FILE * stream, struct delegation *contract)
{
  char *cp, *cp2;
  cp = xstrdup (contract->contract);
  cp = strtok (cp, ":");
  cp2 = strtok (NULL, ":");

  /* E.g.: Delegation `PsNup', from ps to ps */
  fprintf (stream, _("Delegation `%s', from %s to %s\n"),
	   contract->name, cp, cp2);
  fprintf (stream, "\t%s\n", contract->command);
  free (cp);
}

/*
 * List the subcontracts for --list-subcontracts
 */
void
delegations_list_long (struct hash_table_s *contracts,
		       FILE * stream)
{
  int i;
  struct delegation **ordered_contracts;
  ordered_contracts =
    ((struct delegation **)
     hash_dump (contracts, NULL, (qsort_cmp_t) delegate_hash_qcmp));

  fputs (_("Applications configured for delegation"), stream);
  putc ('\n', stream);

  for (i = 0; ordered_contracts[i]; i++)
    dump_contract (stream, ordered_contracts[i]);
  putc ('\n', stream);

  free (ordered_contracts);
}

/*
 * For --list-features
 */
void
delegations_list_short (struct hash_table_s *contracts,
			FILE * stream)
{
  struct delegation **ordered_contracts;
  ordered_contracts =
    ((struct delegation **)
     hash_dump (contracts, NULL, (qsort_cmp_t) delegate_hash_qcmp));

  fputs (_("Applications configured for delegation"), stream);
  putc ('\n', stream);

  lister_fprint_vertical (NULL, stream,
                          (void *) ordered_contracts, (size_t) -1,
                          (lister_width_t) delegate_name_len,
                          (lister_print_t) delegate_name_fputs);
  free (ordered_contracts);
}
