/*
 * output.c
 *
 * routines for ram-diverted output
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
 * $Id: output.c,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
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
#include "routines.h"
#include "output.h"
#include "assert.h"
#include "pathwalk.h"
#include "darray.h"
#include "dstring.h"
#include "printers.h"
#include "message.h"
#include "dsc.h"
#include "stream.h"
#include "metaseq.h"
#include "quotearg.h"

#define MIN_CONTENT	1024 * 50 	/* 50 Kb buffer			*/
#define CHUNK_MARGIN	1024		/* Be sure there is always that left */
#define MIN_DERIVATIONS	10

enum derivation_type
{
  nothing,
  delayed_int,
  delayed_string,
  delayed_routine,
  delayed_chunk
};

struct derivation
{
  enum derivation_type type;
  void * arg;
  delayed_routine_t delayed_routine;
  void * delayed_routine_arg;
  int * delayed_int;
  uchar ** delayed_string;
  struct output * delayed_chunk;
};

struct output
{
  const char * name;
  struct dstring * chunk;
  struct darray * derivations;
  int to_void; 	/* if true, what is sent here is forgotten	*/
};


static struct derivation *
new_derivation (enum derivation_type type)
{
  struct derivation * res = XMALLOC (struct derivation, 1);
  res->type = type;
  return res;
}

static void
derivation_self_print (struct derivation * derivation, FILE * stream)
{
  fprintf (stream, "At %x: ", (int) derivation);
  switch (derivation->type)
    {
    case nothing:
      fprintf (stream, "nothing ");
      break;

    case delayed_int:
      fprintf (stream, "delayed_int (%d)", *derivation->delayed_int);
      break;

    case delayed_string:
      fprintf (stream, "delayed_string ");
      if (*derivation->delayed_string)
	fprintf (stderr, "(%s)", *derivation->delayed_string);
      else
	fprintf (stderr, "##BROKEN##");
      break;

    case delayed_routine:
      fprintf (stream, "delayed_routine ");
      break;

    case delayed_chunk:
      fprintf (stream, "delayed_chunk ");
      break;
    }
}

/************************************************************************/
/*		Output maintenance					*/
/************************************************************************/
/*
 * Create a new derivation
 */
struct output *
output_new (const char * name)
{
  struct output * res = NULL;

  res = XMALLOC (struct output, 1);
  res->name = name;
  res->chunk = ds_new (MIN_CONTENT, ds_geometrical, 2);
  res->derivations = da_new ("derivations", MIN_DERIVATIONS,
			    da_geometrical, 2,
			    (da_print_func_t) derivation_self_print,
			     NULL);
  res->to_void = false;

  return res;
}

/*
 * Release the chunk
 */
void
output_free (struct output * out)
{
  ds_erase (out->chunk);
  da_free (out->derivations, (da_map_func_t) free);
  free (out);
}

void
output_report (struct output * out, FILE * stream)
{
  fprintf (stream, "Output `%s' stats:\n", out->name);
  ds_print_stats (out->chunk, stream);
  da_print_stats (out->derivations, stream);
}

/*
 * Should the derivation forget what it receives?
 */
void
output_to_void (struct output * out, int forget)
{
  out->to_void = forget;
}

/*
 * Is it send to /dev/null?
 */
int
output_is_to_void (struct output * out)
{
  return out->to_void;
}

/************************************************************************/
/*		Putting stuff in the output				*/
/************************************************************************/
/*
 * Equivalent of printf into an output
 */
void
#if defined(VA_START) && __STDC__
output (struct output * out, const char *format, ...)
#else
output (out, format, va_alist)
  struct output * out;
  char * format;
  va_dcl
#endif
{
#ifdef VA_START
  va_list args;

  VA_START (args, format);
#endif

  if (out->to_void)
    return;

#ifdef VA_START
  ds_unsafe_cat_vsprintf (out->chunk,
			 format, args);
  va_end (args);
#else
  ds_unsafe_cat_vsprintf (out->chunk,
			 format, a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

/*
 * Add C to the end of output
 */
void
output_char (struct output * out, uchar c)
{
  if (out->to_void)
    return;

  ds_strccat (out->chunk, c);
}

/*
 * the routines is called with job as arg, at dump time
 */
void
output_delayed_routine (struct output * out,
			delayed_routine_t fn,
			void * fn_arg)
{
  struct derivation *tmp;

  if (out->to_void)
    return;

  tmp = new_derivation (delayed_routine);
  tmp->delayed_routine = fn;
  tmp->delayed_routine_arg = fn_arg;

  output_char (out, '\0');
  da_append (out->derivations, tmp);
}

void
output_delayed_chunk (struct output * out, struct output * out2)
{
  struct derivation *tmp;

  if (out->to_void)
    return;

  tmp = new_derivation (delayed_chunk);
  tmp->delayed_chunk = out2;

  output_char (out, '\0');
  da_append (out->derivations, tmp);
}

/*
 * The value pointed will be read only when undiverting
 */
void
output_delayed_int (struct output * out, int * ptr)
{
  struct derivation *tmp;

  if (out->to_void)
    return;

  tmp = new_derivation (delayed_int);
  tmp->delayed_int = ptr;

  output_char (out, '\0');
  da_append (out->derivations, tmp);
}

/*
 * The value pointed will be read only when undiverting
 */
void
output_delayed_string (struct output * out, uchar ** ptr)
{
  struct derivation *tmp;

  if (out->to_void)
    return;

  tmp = new_derivation (delayed_string);
  tmp->delayed_string = ptr;

  output_char (out, '\0');
  da_append (out->derivations, tmp);
}

/* FIXME: Must be robust to missing arguments */
#define GET_TOKEN(from) (strtok ((from), " \t\n"))
#define GET_LINE_TOKEN(from) (strtok ((from), "\n"))

#define CHECK_TOKEN() 							\
  if (token2 == NULL) 							\
    error_at_line (1, 0, filename, line, 				\
		   _("missing argument for `%s'"), quotearg (token));
/*
 * Dump a library file content
 */
void
output_file (struct output * out, a2ps_job * job,
	     const char *name, const char *suffix)
{
  char buf[512];
  FILE * stream;
  char * filename;
  char * token = NULL, * token2 = NULL;
  int line = 0;
  int dont_output = false;
  struct output * dest = out;

  if (out->to_void)
    return;

  message (msg_file, (stderr, "Outputing file %s%s\n", name, UNNULL (suffix)));

  filename = xpw_find_file (job->common.path, name, suffix);
  stream = xrfopen (filename);

  /* Find the end of the header. */
#define HDR_TAG "% -- code follows this line --"
  while ((fgets (buf, sizeof (buf), stream))) {
    line++;
    if (strprefix (HDR_TAG, buf))
      break;
  }

  /* Dump rest of file. */
  while ((fgets (buf, sizeof (buf), stream))) {
    line++;

#define END_FONTUSED_TAG	"%%EndFontUsed"
    if (strprefix (END_FONTUSED_TAG, buf))
      {
	dont_output = false;
	continue;
      }

    else if (dont_output)
      continue;

#define FONTUSED_TAG		"%%IfFontUsed:"
    /* After this tag, forget unless the font is used.
     * This is for small memory printers */
    else if (strprefix (FONTUSED_TAG, buf))
      {
#if 0
	char * fontname;
	font_t font;

	fontname =  buf + strlen (FONTUSED_TAG);
	/* What is the face declared? */
	fontname = strtok (fontname, " \t\n");
	font = string_to_font (fontname);

	if (font == unknown_font)
	  error_at_line (1, 0, filename, line,
			 "unknown font `%s'", quotearg (fontname));
	if (!(job->status->font_used [font]))
	  dont_output = true;
#endif
	continue;
      }

#define INCL_RES_TAG 	"%%IncludeResource:"
    if (strprefix (INCL_RES_TAG, buf))
      {
	char * value, * res, * buf2;
	buf2 = xstrdup (buf);
	token = GET_TOKEN (buf);
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	res = token2;
	if (strequ (res, "file"))
	  {
	    /* We want to include a file only once */
	    token2 = GET_TOKEN (NULL);
	    CHECK_TOKEN ();
	    value = token2;
	    if (!exist_resource (job, res, value))
	      {
		add_needed_resource (job, res, value);
		message (msg_file,
			 (stderr,
			  "Including file '%s' upon request given in '%s':%d\n",
			  value, filename, line));
		output_file (dest, job, value, NULL);
	      }
	  }
	else
	  {
	    /* Leave the line as it is */
	    output (dest, "%s", buf2);
	    while ((value = GET_TOKEN (NULL)))
	      add_needed_resource (job, res, value);
	  }
	free (buf2);
	continue;
      }
#define COLOR_TAG 	"%%DocumentProcessColors:"
    else if (strprefix (COLOR_TAG, buf))
      {
	token = GET_TOKEN (buf);
	token2= GET_TOKEN (NULL);
	CHECK_TOKEN ();
	add_process_color (job, token2);
	while ((token2 = GET_TOKEN (NULL)))
	  add_process_color (job, token2);
	continue;
      }
#define SUPP_RES_TAG 	"%%BeginResource:"
    else if (strprefix (SUPP_RES_TAG, buf))
      {
	char * res;
	output (dest, "%s", buf);

	token = GET_TOKEN (buf);
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	res = token2;
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	add_supplied_resource (job, res, token2);
	continue;
      }

#define END_SETUP_TAG	"%%EndSetup"
    else if (strprefix (END_SETUP_TAG, buf))
      {
	if (dest == out)
	  error (1, 0, "`setup' incoherence in output_file");
	dest = out;
	continue;
      }

#define SETUP_TAG 	"%%BeginSetup"
    else if (strprefix (SETUP_TAG, buf))
      {
	dest = job->status->setup;
	continue;
      }

#define FACE_TAG	 "%Face:"
    else if (strprefix (FACE_TAG, buf))
      {
	/* FIXME: We must make a symbol table between fonts and faces. */
	char * fontname;
	enum face_e face;

	token = GET_TOKEN (buf);

	/* Face name -> face */
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	face = string_to_face (token2);
	if (face == No_face)
	  /* TRANS: a face is a virtual `font', for instance Keyword,
	     or Comment_strong, or String are faces. */
	  error_at_line (1, 0, filename, line,
			 _("invalid face `%s'"), quotearg (token2));

	/* What is the corresponding physical font? */
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	fontname = token2;

	/* Bind font to face */
	face_set_font (job, face, fontname);

	/* Font size */
	token2 = GET_LINE_TOKEN (NULL);
	CHECK_TOKEN ();
	output (dest, "  f%s %s scalefont setfont\n",
		fontname, token2);
	continue;
      }

#define FONT_TAG	 "%Font:"
    else if (strprefix (FONT_TAG, buf))
      {
	char * basefontname;
	const char * true_font_name;

	token = GET_TOKEN (buf);

	/* What is the asked font name (before subsitution)?
	 * Register the corresponding font */
	token2 = GET_TOKEN (NULL);
	CHECK_TOKEN ();
	basefontname = token2;
	/* Font size */
	token2 = GET_LINE_TOKEN (NULL);
	CHECK_TOKEN ();

	encoding_add_font_name_used (job->requested_encoding,
				     basefontname);

	/* Register the font that will be used.
	 * We don't want to read its AFM file: no use of the
	 * WX.  Just put it in the DSC storage, that will decide
	 * whether including the font's file, or not */
	true_font_name =
	  encoding_resolve_font_substitute (job,
					    job->requested_encoding,
					    basefontname);
	add_required_font (job, true_font_name);


	output (dest, "  f%s %s scalefont setfont\n", basefontname, token2);
	continue;
      }
#define EXPAND_TAG	 "%Expand:"
    else if (strprefix (EXPAND_TAG, buf))
      {
	char * expansion;

	token = GET_LINE_TOKEN (buf + strlen (EXPAND_TAG));
	expansion = ((char *)
		     expand_user_string (job, FIRST_FILE (job),
					 (const uchar *) "Expand: requirement",
					 (const uchar *) token));
	output (dest, expansion);
	continue;
      }

    output (dest, "%s", buf);
  }

  if (dest != out)
    /* E.g. `%%BeginSetup' with no matching `%%EndSetup' */
    error_at_line (1, 0, filename, line,
		   _("`%s' with no matching `%s'"),
		   SETUP_TAG, END_SETUP_TAG);

  fclose (stream);
  free (filename);
}

/*
 * The derivation of a special kind will be emptied in STREAM
 */
inline static void
underivation (FILE * stream, const struct derivation * derivation)
{
  switch (derivation->type)
    {
    case delayed_routine:
      derivation->delayed_routine (stream,
				   derivation->delayed_routine_arg);
      break;

    case delayed_int:
      fprintf (stream, "%d", *derivation->delayed_int);
      break;

    case delayed_string:
      fprintf (stream, "%s", *derivation->delayed_string);
      break;

    case delayed_chunk:
      output_dump (derivation->delayed_chunk, stream);
      break;

    case nothing:
      break;

    default:
      abort ();
    }
}

/*
 * For debugging
 */
void
output_self_print (struct output * out, FILE * stream)
{
  fprintf (stream, "The derivations:\n");
  da_self_print (out->derivations, stream);
}

/*
 * Empty OUT into STREAM
 */
void
output_dump (struct output * out, FILE * stream)
{
  size_t i;
  char * piece = out->chunk->content;
  struct derivation ** derivations =
    (struct derivation **) out->derivations->content;

  if (msg_test (msg_tool))
    output_report (out, stderr);

  fputs ((char *) piece, stream);
  piece += ustrlen (piece);
  for (i = 0 ; i < out->derivations->len ; i++)
    {
      underivation (stream, derivations [i]);
      piece ++;
      fputs ((char *) piece, stream);
      piece += ustrlen (piece);
    }
}

/*
 * End of the dervertion: send its content on the selected
 * destination
 */
void
undivert (a2ps_job * job)
{
  /* Open the destination */
  a2ps_open_output_stream (job);

  /* Dump the PostScript and close */
  output_dump (job->divertion, job->output_stream->fp);

  /* We have to close stdout to avoid hanging up of pipes */
  /* Note: some day, I should learn about the signals, and be able
   * either to catch or to ignore the SIG_PIPE that happens
   * in the fclose when the piped command exit != 0 */
  a2ps_close_output_stream (job);
}
