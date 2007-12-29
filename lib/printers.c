/*
 * printers.c - Information about the printers (named outputs)
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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

#define printer_table hash_table_s

#include "a2ps.h"
#include "jobs.h"
#include "printers.h"
#include "routines.h"
#include "xstrrpl.h"
#include "hashtab.h"
#include "printers.h"
#include "message.h"
#include "metaseq.h"
#include "stream.h"
#include "ppd.h"
#include "pathwalk.h"
#include "xbackupfile.h"
#include "lister.h"
#include "title.h"
#include "strverscmp.h"

#define DEFAULT_PRINTER _("Default Printer")
#define UNKNOWN_PRINTER _("Unknown Printer")

/* Road map of this file
  1. struct printer and functions.
  2. struct printer_table and functions.
  2. struct a2ps_printers_s and functions.
  4. interface for struct a2ps_job
*/

/************************************************************************
 * hash tables for the printers						*
 ************************************************************************/
/*
 * Description of a printer
 */
struct printer
{
  char * key;
  char * ppdkey;	/* Key to the ppd file which describe it */
  char * command;	/* Command to run to run the printer	*/
};

/*
 * Basic routines
 */
static unsigned long
printer_hash_1 (struct printer *printer)
{
  return_STRING_HASH_1 (printer->key);
}

static unsigned long
printer_hash_2 (struct printer *printer)
{
  return_STRING_HASH_2 (printer->key);
}

static int
printer_hash_cmp (struct printer *x, struct printer *y)
{
  return_STRING_COMPARE (x->key, y->key);
}

static int
printer_hash_qcmp (struct printer **x, struct printer **y)
{
  return strverscmp ((*x)->key, (*y)->key);
}

/* Return the length of the key of the PRINTER. */

static size_t
printer_key_len (struct printer *printer)
{
  return strlen (printer->key);
}

/* Fputs on STREAM the key of the PRINTER. */

static void
printer_key_fputs (struct printer * printer, FILE * stream)
{
  fputs (printer->key, stream);
}

/* Give void values. */

static void
printer_create (struct printer *p, const char *key)
{
  p->key = xstrdup (key);
  p->ppdkey = NULL;
  p->command = NULL;
}

/* Set the value. */
static void
printer_set (struct printer *p,
	     const char * ppdkey, const char * command)
{
  xstrcpy (p->ppdkey, ppdkey);
  xstrcpy (p->command, command);
}

/* Allocate, and set. */

static struct printer *
printer_new (const char *key)
{
  NEW (struct printer, res);
  printer_create (res, key);
  return res;
}


/* Free the content, but not the pointer. */

static void
printer_free (struct printer * printer)
{
  /* Default and Unknown printers have NULL key. */
  XFREE (printer->key);
  XFREE (printer->ppdkey);
  XFREE (printer->command);
}

/*
 * Format the presentation of a printer and its command for
 * --list-printers.
 */
static void
printer_self_print (struct printer * printer, FILE * stream)
{
  fputs ("- ", stream);
  fputs (printer->key, stream);
  if (printer->ppdkey)
    fprintf (stream, " (PPD: %s)", printer->ppdkey);
  putc ('\n', stream);

  if (printer->command)
    fprintf (stream, "  %s\n", printer->command);
}


/************************************************************************/
/* 2. Printer_table functions						*/
/************************************************************************/
/*
 * Create the structure that stores the list of printers
 */
static inline struct printer_table *
printer_table_new (void)
{
  NEW (struct hash_table_s, res);

  hash_init (res, 8,
	     (hash_func_t) printer_hash_1,
	     (hash_func_t) printer_hash_2,
	     (hash_cmp_func_t) printer_hash_cmp);

  return (struct printer_table *) res;
}

/*
 * Free the whole structure
 */
static inline void
printer_table_free (struct printer_table * table)
{
  hash_free (table, (hash_map_func_t) printer_free);
  free (table);
}

/* Return the printer corresponding to KEY in TABLE if exist,
   otherwise NULL. */

static inline struct printer *
printer_table_find (struct printer_table *table, const char *key)
{
  struct printer token;

  token.key = (char *) key;
  return (struct printer *) hash_find_item (table, &token);
}

/*
 *  Add a pair, with your own allocation for them.
 * It KEY is yet used, override its value with VALUE
 */
static inline void
printer_table_add (struct printer_table * table,
		   const char * key, const char * ppdkey,
		   const char * command)
{
  struct printer *printer;

  printer = printer_table_find (table, key);
  if (!printer)
    printer = printer_new (key);

  printer_set (printer, ppdkey, command);

  hash_insert (table, printer);
}

/*
 * Report content (short form)
 */
static void
printer_table_short_self_print (struct printer_table * table, FILE * stream)
{
  struct printer ** entries;
  size_t size;

  entries = (struct printer **)
    hash_dump (table, NULL,
	       (hash_cmp_func_t) printer_hash_qcmp);

  for (size = 0 ; entries [size] ; size++)
    /* nothing */ ;

  lister_fprint_vertical (NULL, stream,
			  (void *) entries, size,
			  (lister_width_t) printer_key_len,
			  (lister_print_t) printer_key_fputs);

  free (entries);
}

/*
 * Report content (long form)
 */
static void
printer_table_self_print (struct printer_table * table, FILE * stream)
{
  hash_maparg (table, (hash_maparg_func_t) printer_self_print,
	       stream, (qsort_cmp_t) printer_hash_qcmp);
}

/************************************************************************/
/*	Handling the printers module					*/
/************************************************************************/
struct a2ps_printers_s
{
  /* Shared mem. */
  struct a2ps_common_s * common;

  /* User defined printers, default printer, unknown printer. */
  struct printer_table *printers;
  struct printer default_printer;	/* Can't use `default'! */
  struct printer unknown_printer;

  /* PPD handling */
  char *default_ppdkey;	/* Key of the ppd to use as default	*/
  char *request_ppdkey;	/* User has specified			*/
  struct ppd *ppd;	/* Name of the ppd file asked		*/

  /* Selected destination.  */

  /* FLAGS values correspond to the choice of the user. */
  bool flag_output_is_printer;	/* ? -P : -o. */
  char *flag_output_name;		/* Arg to -P or -o. */

  /* These values correspond to what has really happened. */
  bool output_is_file;	/* -o, or -P on a saving printer. */
  /* If OUTPUT_IS_FILE, the file name, else the printer name. */
  char *output_name;
};


/* Allocate and reset the printers module. */

struct a2ps_printers_s *
a2ps_printers_new (struct a2ps_common_s * common)
{
  NEW (struct a2ps_printers_s, res);

  /* Shared mem */
  res->common = common;

  /* Available printers (outputs). */
  res->printers = printer_table_new ();
  printer_create (&res->default_printer, DEFAULT_PRINTER);
  printer_create (&res->unknown_printer, UNKNOWN_PRINTER);

  /* PPD */
  res->request_ppdkey = NULL;
  res->default_ppdkey = xstrdup ("level1"); /* By default, level1 PS */
  res->ppd = NULL;		/* Printer's ppd are not read yet */

  /* Output */
  /* Default is to send to default printer */
  res->flag_output_is_printer = true;
  res->flag_output_name = NULL;
  res->output_is_file = true;
  res->output_name = NULL;

  return res;
}

/*
 * Release the mem used by a PRINTERS module
 * The module is freed
 */
void
a2ps_printers_free (struct a2ps_printers_s * printers)
{
  /* Don't free common, a2ps_job is in charge */

  printer_table_free (printers->printers);
  printer_free (&printers->default_printer);
  printer_free (&printers->unknown_printer);

  /* PPD */
  XFREE (printers->request_ppdkey);
  XFREE (printers->default_ppdkey);
  ppd_free (printers->ppd);

  /* Output */
  XFREE (printers->flag_output_name);
  XFREE (printers->output_name);

  free (printers);
}


/* Find the PPD key associated with the printer KEY.  If undefined,
   resolve to the unknown and default printers. */

static const char *
a2ps_printers_ppdkey_get (struct a2ps_printers_s *printers,
			  const char * key)
{
  struct printer *printer;

  /* If key is empty, it's the default printer (user used `-d'). */
  if (!key)
    return printers->default_printer.ppdkey;

  /* The printer has name: user used `-P'. */
  printer = printer_table_find (printers->printers, key);

  if (printer && printer->ppdkey)
    /* The printer is declared and has a PPD. */
    return printer->ppdkey;

  /* Printer is either unknown or has no PPD: use that of the unknown
     printer. */
  return printers->unknown_printer.ppdkey;
}

/* Find the command associated with the printer KEY.  If undefined,
   resolve to the unknown and default printers. */

static const char*
a2ps_printers_command_get (struct a2ps_printers_s *printers,
			   const char *key)
{
  struct printer *printer;

  /* If key is empty, it's the default printer (user used `-d'). */
  if (!key)
    {
      if (!printers->default_printer.command)
	/* TRANS: The first `%s' is typically the name of the printer
	   (default or unknown), while the last two strings expand
	   into the options that caused the message (i.e., "-d" "" in
	   the case of the default printer).  */
	error (1, 0,
	       _("no command for the `%s' (%s%s)"),
	       DEFAULT_PRINTER, "-d", "");
      return printers->default_printer.command;
    }

  /* The printer has name: user used `-P'. */
  printer = printer_table_find (printers->printers, key);

  if (printer && printer->command)
    /* The printer is declared and has a command. */
    return printer->command;

  /* Printer is either unknown or has no command (used when Printer:
     introduces only the PPD). */
  if (!printers->unknown_printer.command)
    {
      error (1, 0,
	     _("no command for the `%s' (%s%s)"),
	     UNKNOWN_PRINTER, "-P ", key);
    }
  return printers->unknown_printer.command;
}

/* Make a standard message upon the destination.  Mallocs the
   result.  If FILE_P, NAME is a file name, else a printer name. */
static uchar*
destination_to_string (const char *name, bool file_p)
{
  uchar *res;

  if (IS_EMPTY (name))
    {
      res = (file_p
	     ? xustrdup (_("sent to the standard output"))
	     : xustrdup (_("sent to the default printer")));
    }
  else
    {
      char *format = (file_p
		      ? _("saved into the file `%s'")
		      : _("sent to the printer `%s'"));
      res = XMALLOC (uchar, strlen (format) + strlen (name));
      sprintf ((char *) res, format, name);
    }
  return res;
}

/* Returns a string that describes the destination of the output,
   *before* evaluating the output.

   Result is malloced. */

uchar *
a2ps_flag_destination_to_string (a2ps_job * job)
{
  /* Make a nice message to tell where the output is sent */
  return destination_to_string (job->printers->flag_output_name,
		job->printers->flag_output_is_printer ? false : true);
}


/* Report where the output was really sent, i.e., evaluate the command
   and in the case of a file, report the file name. */

uchar *
a2ps_destination_to_string (a2ps_job * job)
{
  /* The main difference is when sending to a file, in which
   * case we want to have its real name */
  return destination_to_string (job->printers->output_name,
				job->printers->output_is_file);
}

/*
 * Finalize the printers module.
 * This is called once the configuration/options have been totally
 * parsed/treated
 */
void
a2ps_printers_finalize (struct a2ps_printers_s * printers)
{
  const char * ppdkey;

  /* 1. Get the right ppd key */
  if ((ppdkey = printers->request_ppdkey))
    /* Nothing */;
  else if (printers->flag_output_is_printer)
    ppdkey = a2ps_printers_ppdkey_get (printers, printers->flag_output_name);
  if (!ppdkey)
    ppdkey = printers->default_ppdkey;

  /* 2. Get the struct ppd */
  printers->ppd = _a2ps_ppd_get (printers->common->path, ppdkey);
  /* FIXME: Check for errors */
}


/*
 * Record a printer defined by the config line "Printer:"
 */
bool
a2ps_printers_add (struct a2ps_printers_s * printers,
		   const char * key, char * definition)
{
  char * ppdkey = NULL;
  char * token = NULL;
  char * command = NULL;

  /* Skip the blanks */
  token = definition + strspn (definition, " \t");

  /* PPD given ? */
  if ((*token != '>') && (*token != '|'))
    {
      /* If the first token does not start by `|' or `>', then ppdkey
        is defined */
      ppdkey = strtok (token, " \t");
      token = strtok (NULL, "\n");
    }
  else
    {
      /* Skip the blanks. */
      token += strspn (token, " \t");
    }

  /* What remains is the command itself (can be NULL). */
  command = token;

  /* Special printers. */
  if (strequ (key, DEFAULT_PRINTER))
    printer_set (&printers->default_printer, ppdkey, command);
  else if (strequ (key, UNKNOWN_PRINTER))
    printer_set (&printers->unknown_printer, ppdkey, command);
  else
    printer_table_add (printers->printers, key, ppdkey, command);

  /* Success. */
  return true;
}

/*
 * Accessing the PPD fields
 */
const char *
a2ps_printers_default_ppdkey_get (struct a2ps_printers_s * printers)
{
  return printers->default_ppdkey;
}

void
a2ps_printers_default_ppdkey_set (struct a2ps_printers_s * printers,
				  const char * ppdkey)
{
  xstrcpy (printers->default_ppdkey, ppdkey);
}

const char *
a2ps_printers_request_ppdkey_get (struct a2ps_printers_s * printers)
{
  return printers->request_ppdkey;
}

void
a2ps_printers_request_ppdkey_set (struct a2ps_printers_s * printers,
				  const char * ppdkey)
{
  xstrcpy (printers->request_ppdkey, ppdkey);
}

/*
 * Accessing the output fields
 */
void
a2ps_printers_flag_output_set (struct a2ps_printers_s * printers,
			       const char * flag_output_name,
			       bool is_printer)
{
  printers->flag_output_is_printer = is_printer;

  if (!is_printer && flag_output_name && strequ (flag_output_name, "-"))
    {
      /* Request for stdin */
      XFREE (printers->flag_output_name);
      printers->flag_output_name = NULL;
    }
  else
    xstrcpy (printers->flag_output_name, flag_output_name);
}

const char *
a2ps_printers_flag_output_name_get (struct a2ps_printers_s * printers)
{
  return (const char *) printers->flag_output_name;
}

bool
a2ps_printers_flag_output_is_printer_get (struct a2ps_printers_s * printers)
{
  return printers->flag_output_is_printer;
}

/*
 * Questioning the printers module
 */
int
a2ps_printers_font_known_p (struct a2ps_printers_s * printers,
			    const char * name)
{
  return ppd_font_known_p (printers->ppd, name);
}

/*
 * Interface to job
 */
void
a2ps_printers_list_short (struct a2ps_job * job, FILE * stream)
{
  fputs (_("Known Outputs (Printers, etc.)"), stream);
  putc ('\n', stream);
  printer_table_short_self_print (job->printers->printers, stream);
}

void
a2ps_printers_list_long (struct a2ps_job * job, FILE * stream)
{
  title (stream, '=', true, _("Known Outputs (Printers, etc.)"));
  putc ('\n', stream);
  printer_self_print (&job->printers->default_printer, stream);
  printer_self_print (&job->printers->unknown_printer, stream);
  printer_table_self_print (job->printers->printers, stream);
}

void
a2ps_ppd_list_short (struct a2ps_job * job, FILE * stream)
{
  _a2ps_ppd_list_short (job->common.path, stream);
}

void
a2ps_ppd_list_long (struct a2ps_job * job, FILE * stream)
{
  _a2ps_ppd_list_long (job->common.path, stream);
}

/*
 * Open a stream on the specified output in JOB
 */
void
a2ps_open_output_stream (struct a2ps_job * job)
{
  struct a2ps_printers_s *printers = job->printers;

  /* Open the destination. */

  if (!printers->flag_output_is_printer)
    {
      /* -o (can be stdout) */
      job->output_stream = stream_wopen_backup (printers->flag_output_name,
						true,
						job->backup_type);
      /* FLAG_OUTPUT_NAME can be NULL. */
      xstrcpy (printers->output_name, printers->flag_output_name);
      printers->output_is_file = true;
    }
  else
    {
      /* -P (or -d if NAME is NULL. */
      const char *name;
      const char *command, *printer_cmd;

      /* Open an output stream onto PRINTER */
      printer_cmd = a2ps_printers_command_get (printers,
					       printers->flag_output_name);

      /* Expand the metaseq before */
      command = (char *) expand_user_string (job, FIRST_FILE (job),
					     (uchar *) "output command",
					     (uchar *) printer_cmd);
      job->output_stream = stream_perl_open_backup (command,
						    job->backup_type,
						    &name);
      if (*command == '>')
	{
	  printers->output_is_file = true;
	  printers->output_name = xstrdup (name);
	}
      else
	{
	  /* flag_output_name can be NULL. */
	  xstrcpy (printers->output_name, printers->flag_output_name);
	  printers->output_is_file = false;
	}
    }
}

/*
 * Close the output stream with fclose or pclose
 */
void
a2ps_close_output_stream (struct a2ps_job * job)
{
  stream_close (job->output_stream);
}
