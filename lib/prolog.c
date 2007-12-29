/*
 * prolog.c
 * routines for the postscript generation
 *
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
 * $Id: prolog.c,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */

#include "a2ps.h"		/* most global variables 		*/
#include "prolog.h"
#include "routines.h"		/* general interest routines		*/
#include "output.h"		/* Diverted outputs			*/
#include "media.h"
#include "message.h"
#include "dsc.h"
#include "jobs.h"
#include "fonts.h"
#include "metaseq.h"
#include "psgen.h"
#include "pathwalk.h"
#include "getshline.h"
#include "document.h"

/* FIXME: Should not be here */
#define PS_PROLOGUE_SUFFIX ".pro"

/* If none null, called at the end of the ps comments, when debugging */
a2ps_job_stream_hook ps_comment_hook = NULL;

/* Important.

   I must say I'm a bit lost in the complexity that results of mixing
   several encodings, and the possibility to avoid printing some pages
   (--pages).  This causes many problems in trying to get a good and
   up-to-date value of job->encoding.

   I have to go and see the code some other day.  Now it seems to
   work, but it is a bit miraculous, and it uses... 4 _four_ variables
   to remember the encoding used.  I think three is OK.  Four is not.

   If somebody feels like cleaning this up, he is really welcome.
 */

#define PRINT_HEADER					\
	(!IS_EMPTY(job->header))

#define PRINT_FOOTER					\
	(!IS_EMPTY(job->footer)				\
	 || !IS_EMPTY(job->left_footer)			\
	 || !IS_EMPTY(job->right_footer))

#define PRINT_TITLE					\
	(!IS_EMPTY(job->center_title)			\
	 || !IS_EMPTY(job->left_title)			\
	 || !IS_EMPTY(job->right_title))

/* Width in characters for line prefix	*/
#define prefix_size	(job->numbering ? 5 : 0)

#define jdiv	job->divertion

/* return the max of two >-comparable stuff */
#define A2_MAX(X,Y)        (((X) > (Y)) ? (X) : (Y))


/****************************************************************/
/* Reporting the known prologues   				*/
/****************************************************************/
/*
 * List the prologues
 */
void
prologues_list_short (a2ps_job * job, FILE * stream)
{
  fputs (_("Known Prologues"), stream);
  putc ('\n', stream);
  pw_lister_on_suffix (stream, job->common.path, PS_PROLOGUE_SUFFIX);
}

/*
 * Print a prologue signature
 */
static inline void
prologue_print_signature (struct a2ps_job * job,
			  const char * prologue_name, FILE * stream,
			  const char * name_format,
			  documentation_fn_t documentation_fn)
{
  FILE * fp;
  char * filename;
  char * buf = NULL;
  int done = false;
  int firstline = 0, lastline = 0;
  size_t bufsiz = 0;
  char buf2[BUFSIZ];

  fprintf (stream, name_format, prologue_name);

  filename = xpw_find_file (job->common.path, prologue_name, PS_PROLOGUE_SUFFIX);
  fp = xrfopen (filename);

  /* Dump rest of file. */
  while ((getshline_numbered (&firstline, &lastline,
			      &buf, &bufsiz, fp) != -1)
	 && !done)
    {
#define DOC_TAG "Documentation"
#define END_DOC_TAG "EndDocumentation"
      if (strprefix (DOC_TAG, buf))
	{
	  /* We don't use getshline, because we do want the
	   * ``empty'' lines to be kept: they separate the
	   * paragraphs */
	  while (fgets (buf2, sizeof(buf2), fp)
		 && !strprefix (END_DOC_TAG, buf2))
	    {
	      if (strlen (buf2) < sizeof (buf2))
		lastline++;
	      (*documentation_fn) ((uchar *) buf2, "%s", stream);
	    }
	  if (!strprefix (END_DOC_TAG, buf2))
	    error (1, 0, filename, firstline,
		   _("missing argument for `%s'"), "`Documentation'");
	  done = true;
	}
    }
  putc ('\n', stream);

  free (buf);
  fclose (fp);
  free (filename);
}

/*
 * List the prologues together with there description
 */
static inline void
prologues_list_long_internal (a2ps_job * job, FILE * stream,
			      const char * name_format,
			      documentation_fn_t documentation_fn)
{
  struct darray * entries;
  size_t i;

  entries = pw_glob_on_suffix (job->common.path, PS_PROLOGUE_SUFFIX);

  for (i = 0 ; i < entries->len ; i++)
    prologue_print_signature (job, entries->content[i], stream,
			      name_format,
			      documentation_fn);

  da_free (entries, (da_map_func_t) free);
}

void
prologues_list_long (a2ps_job * job, FILE * stream)
{
  fputs (_("Known Prologues"), stream);
  putc ('\n', stream);

  prologues_list_long_internal (job, stream,
				"Prologue \"%s\":\n",
				documentation_print_plain);
}

void
prologues_list_texinfo (a2ps_job * job, FILE * stream)
{
  fputs ("@table @samp\n", stream);
  prologues_list_long_internal (job, stream,
				"@item %s\n", documentation_print_texinfo);
  fputs ("@end table\n", stream);
}


/*
 * We want to optimize the number of fonts defined in the
 * postscript: we may run into limitcheck on poor printers
 * The optimization is not very strong: we consider that
 * if say fce is used in a face, then it will be produced
 * in _every_ encoding.  A more parsimonous scheme may be
 * used, but I'm afraid to slow down the whole stuff because
 * of the test required.
 */
static void
dump_encodings (FILE * stream, a2ps_job * job)
{
  /* If the encoding is the main encoding (that of the
   * headers etc.), dump what is required for the headers */
  output_dump (job->ps_encodings, stream);
}

/****************************************************************/
/*		Printing a document				*/
/****************************************************************/
static void
dump_prolog_comments (FILE * stream, struct a2ps_job * job)
{
  uchar * cp;
  /*
   * Fixme: Put all this is output_first_line?
   */
  fputs ((char *) job->status->magic_number, stream);
  putc ('\n', stream);

  cp = expand_user_string (job, FIRST_FILE (job),
			   (uchar *) "Document title", job->title);
  fprintf (stream, "%%%%Title: %s\n", cp);

  cp = expand_user_string (job, FIRST_FILE (job),
			  (uchar *) "User Name", (const uchar *) "%N");
  fprintf (stream, "%%%%For: %s\n", cp);
  fprintf (stream, "%%%%Creator: %s version %s\n", PACKAGE, VERSION);
  fprintf (stream, "%%%%CreationDate: %s", asctime(&job->run_tm));

  fprintf (stream, "%%%%BoundingBox: %d %d %d %d\n",
	   job->medium->llx, job->medium->lly,
	   job->medium->urx, job->medium->ury);
  fprintf (stream, "%%%%DocumentData: Clean7Bit\n");
  fprintf (stream, "%%%%Orientation: %s\n",
	   (job->orientation == landscape) ? "Landscape" : "Portrait");
  fprintf (stream, "%%%%Pages: %d\n", job->sheets);
  if (job->status->page_are_ordered)
    fprintf (stream, "%%%%PageOrder: Ascend\n");
  else
    fprintf (stream, "%%%%PageOrder: Special\n");
  fprintf (stream, "%%%%DocumentMedia: %s %d %d 0 () ()\n",
	   job->medium->name,
	   job->medium->w, job->medium->h);

  dump_needed_resources (stream, job);
  dump_process_color (stream, job);
  dump_requirements (stream, job);
  dump_supplied_resources (stream, job);
  fputs ("%%EndComments\n", stream);

  /* This is mostly useful wrt users who are not able to say exactely
     what they did.  */
  if (ps_comment_hook)
    ps_comment_hook (job, stream);
}

/*
 * Print the PostScript prolog.
 */
static void
output_prolog (a2ps_job * job)
{
  /* Comments */
  output_delayed_routine (jdiv,
			  (delayed_routine_t) dump_prolog_comments,
			  (void *) job);

  /* If we are in debug mode, download a PS error handler */
  if (job->debug)
    output_file (jdiv, job, "ehandler", ".ps");

  /* a2ps dict, (needed for good eps files) */
  output (jdiv, "/a2psdict 200 dict def\n");
  output (jdiv, "a2psdict begin\n");

  /* Prolog */
  output (jdiv, "%%%%BeginProlog\n");

  output_file (jdiv, job, job->prolog, ".pro");
  output (jdiv, "%%%%EndProlog\n");
}

/*
 * Print the PostScript document setup part.
 */
static void
output_document_setup (a2ps_job * job)
{
  int i, j;

  /* Set up */
  output (jdiv, "%%%%BeginSetup\n");

  /* At this point, everything should be known about the faces */
  check_face_eo_font (job);
  /* At this point, yet we know the value of the
   * the fonts used for each face.  job->requested_encoding is
   * the only encoding which does know its faces_wx yet */
  encoding_build_faces_wx (job, job->requested_encoding);

  /* Encodings used */
  output_delayed_routine (jdiv,
			  (delayed_routine_t) dump_encodings,
			  (void *) job);

  /* Fonts used */
  output_delayed_routine (jdiv,
			  (delayed_routine_t) dump_fonts,
			  (void *) job);

  /* Complete the prolog with static variables */
  output (jdiv, "%% Initialize page description variables.\n");
  switch (job->orientation)
    {
    case portrait:
      /* FIXME: prendre en compte les marges */
      output (jdiv, "/sh %d def\n", job->medium->h);
      output (jdiv, "/sw %d def\n", job->medium->w);
      output (jdiv, "/llx %d def\n", job->medium->llx);
      output (jdiv, "/urx %d def\n", job->medium->urx - job->margin);
      output (jdiv, "/ury %d def\n", job->medium->ury);
      output (jdiv, "/lly %d def\n", job->medium->lly);
      break;

    case landscape:
      output (jdiv, "/sh %d def\n", job->medium->w);
      output (jdiv, "/sw %d def\n", job->medium->h);
      output (jdiv, "/llx %d def\n", job->medium->lly);
      output (jdiv, "/urx %d def\n", job->medium->ury);
      output (jdiv, "/ury %d def\n",
	      job->medium->w - job->medium->llx);
      output (jdiv, "/lly %d def\n",
	      job->medium->w - job->medium->urx + job->margin);
      break;
    }

  /* Misceleanous PostScript variables */
  output (jdiv, "/#copies %d def\n", job->copies);

  /* Page prefeed */
  if (job->page_prefeed)
    output (jdiv, "true page_prefeed\n");

  /* statusdict definitions */
  output_statusdict (job);

  /* Page device definitions */
  output_pagedevice (job);

  /* Header size */
  output (jdiv, "/th %f def\n", job->status->title_bar_height);

  /* General format */
  /* Font sizes */
  output (jdiv, "/fnfs %d def\n", job->status->title_font_size);
  output (jdiv, "/bfs %f def\n", job->fontsize);
  output (jdiv, "/cw %f def\n", job->fontsize * 0.6);  /* char width */
  output (jdiv, "\n");

  /* Are diverted:
   * 1. setup of the fonts
   * 2. %%(Begin|End)Setup parts read in the various ps files. */
  output_delayed_routine (jdiv,
			  (delayed_routine_t) dump_encodings_setup,
			  (void *) job);

  output_delayed_routine (jdiv,
			  (delayed_routine_t) dump_setup,
			  (void *) job);

  /* Sorry for the length of the name... */
  output_delayed_routine (jdiv,
			  (delayed_routine_t) font_info_table_dump_special_font_setup,
			  (void *) job);


  output (jdiv, "/hm fnfs 0.25 mul def\n");

  /* Page attributes */
  output (jdiv, "/pw\n");
  output (jdiv, "   cw %f mul\n",
	  (float) job->status->columnsperline + 2 * SIDE_MARGIN_RATIO);
  output (jdiv, "def\n");
  output (jdiv, "/ph\n");
  output (jdiv, "   %f th add\n",
	  (job->status->linesperpage + BOTTOM_MARGIN_RATIO) * job->fontsize);
  output (jdiv, "def\n");
  if (job->columns > 1)
    output (jdiv, "/pmw urx llx sub pw %d mul sub %d div def\n",
	    job->columns, job->columns - 1);
  else
    output (jdiv, "/pmw 0 def\n");
  if (job->rows > 1)
    output (jdiv, "/pmh ury lly sub ph %d mul sub %d sub %d div def\n",
	    job->rows,
	    (PRINT_HEADER + PRINT_FOOTER) * HEADERS_H,
	    job->rows - 1);
  else
    output (jdiv, "/pmh 0 def\n");
  output (jdiv, "/v 0 def\n");

  switch (job->madir) {
  case madir_rows:
    output (jdiv, "/x [\n");
    for (j = 1 ; j <= job->rows ; j++) {
      output (jdiv, "  0\n");
      for (i = 2 ; i <= job->columns ; i++)
	output (jdiv, "  dup pmw add pw add\n");
    }
    output (jdiv, "] def\n");

    output (jdiv, "/y [\n");
    for (j = job->rows ; 1 <= j ; j--) {
      output (jdiv, "  pmh ph add %d mul ph add\n", j - 1);
      for (i = 2 ; i <= job->columns ; i++)
	output (jdiv, "  dup\n");
    }
    output (jdiv, "] def\n");
    break;

  case madir_columns:
    output (jdiv, "/x [\n");
    for (i = 1 ; i <= job->columns ; i++) {
      output (jdiv, "  pmw pw add %d mul\n", i - 1);
      for (j = 2 ; j <= job->rows ; j++) {
	output (jdiv, "  dup\n");
      }
    }
    output (jdiv, "] def\n");

    output (jdiv, "/y [\n");
    for (i = 1 ; i <= job->columns ; i++)
      for (j = job->rows ; j >= 1 ; j--) {
	output (jdiv, "  pmh ph add %d mul ph add\n", j - 1);
      }
    output (jdiv, "] def\n");
    break;
  default:
    error (1, 0, "output_document_setup: bad switch");
    break;
  }

  output (jdiv, "/scx sw 2 div def\n");
  output (jdiv, "/scy sh 2 div def\n");

  output (jdiv, "/snx urx def\n");
  output (jdiv, "/sny lly 2 add def\n");
  output (jdiv, "/dx llx def\n");
  output (jdiv, "/dy sny def\n");
  output (jdiv, "/fnx scx def\n");
  output (jdiv, "/fny dy def\n");
  output (jdiv, "/lx snx def\n");
  output (jdiv, "/ly ury fnfs 0.8 mul sub def\n");
  output (jdiv, "/sx %d def\n", prefix_size);
  output (jdiv, "/tab %d def\n", job->tabsize);
  output (jdiv, "/x0 0 def\n");
  output (jdiv, "/y0 0 def\n");

  /* Close prolog */
  output (jdiv, "%%%%EndSetup\n\n");
}

static void
end_document (a2ps_job * job)
{
  page_flush (job);
  output (jdiv, "\n%%%%Trailer\n");

  if (job->page_prefeed)
    output (jdiv, "false page_prefeed\n");

  /* Close a2ps dict. */
  output (jdiv, "end\n");
  output (jdiv, "%%%%EOF\n");
}
/************************************************************************
 * 			Routines called by the main loop		*
 ************************************************************************/
/*
 * Called by the main loop to initialize postscript printing
 */
void
ps_begin (a2ps_job * job)
{
  float area_w = 0.0, area_h = 0.0;	/* Dimension of drawable area	*/
  float printing_h, printing_w;  	/* Body size of virtual pages	*/
  struct medium * medium = job->medium;

  /* Postcript prolog printing */
  /* This will allow us to read information from the prologs,
   * such as the size they'd like to have to print the headers,
   * before we actually calculate the parameters */
  output_prolog (job);

  job->virtual = 0;

  switch (job->orientation) {
  case portrait:
    area_h = (medium->ury - medium->lly
	      /* Room for header and footer */
	      - (PRINT_HEADER + PRINT_FOOTER) * HEADERS_H);
    area_w = (medium->urx - medium->llx
	      - job->margin);
    break;
  case landscape:
    area_w = (medium->ury - medium->lly);
    area_h = (medium->urx - medium->llx
	      /* Room for header and footer */
	      - (PRINT_HEADER + PRINT_FOOTER) * HEADERS_H
	      - job->margin);
    break;
  }

  /* Initialize variables related to the header */
  if (!PRINT_TITLE) {
    job->status->title_font_size = 11;
    job->status->title_bar_height = 0.0;
  } else if (job->columns * job->rows > 1) {
    job->status->title_font_size = 11;
    job->status->title_bar_height = LANDSCAPE_HEADER;
  } else {
    job->status->title_font_size = 15;
    job->status->title_bar_height = PORTRAIT_HEADER;
  }

  /* Area inside the frame of a virtual page */
  printing_h = ((area_h
		 /* room for title */
		 - job->rows * job->status->title_bar_height
		 /* Space between the virtual pages */
		 - ((job->rows > 1) ? PAGE_MARGIN : 0))
		/ job->rows);
  printing_w = ((area_w
		 /* Space between the virtual pages */
		 - ((job->columns > 1) ? PAGE_MARGIN : 0))
		/ job->columns);

  /*
   * Determine the font size according to (decreasing priority):
   * 1. --font=@<size>
   * 2. --columns-per-page,
   * 3. --lines-per-page,
   * 4. default values
   */
  /* width = 0.6 font size */
  if (job->columns_requested != 0) {
    job->fontsize = ((printing_w /
		      (job->columns_requested + prefix_size +
		       2 * SIDE_MARGIN_RATIO))
		     / 0.6);
  } else if (job->lines_requested != 0) {
    job->fontsize = (printing_h /
		     (job->lines_requested + BOTTOM_MARGIN_RATIO));
  } else if (job->fontsize == 0.0)
    job->fontsize = ((job->orientation == landscape) ? 6.8
		     : (job->columns * job->rows > 1) ? 6.4 : 9.0);

  /* fontsize is OK.  Calculate the other variables */
  job->status->linesperpage =
    (int) ((printing_h / job->fontsize) - BOTTOM_MARGIN_RATIO);
  job->status->columnsperline =
    (int) ((printing_w / (job->fontsize * 0.6)) - 2 * SIDE_MARGIN_RATIO);

  if (job->columns_requested > 0)
    job->status->columnsperline = job->columns_requested + prefix_size;
  else if (job->lines_requested > 0)
    job->status->linesperpage = job->lines_requested;

  if (job->status->linesperpage <= 0 || job->status->columnsperline <= 0)
    error (1, 0, _("font %f too big"), job->fontsize);

  job->status->wxperline =
    (job->status->columnsperline - prefix_size) * COURIER_WX;

  output_document_setup (job);

  /* Set the encoding.  Usualy it is done in begin_sheet, but
   * begin_sheet may not be called if, for instance, the first page is
   * skip (by --pages).  We must guarantee at least the first encoding
   *
   * FIXME: This does not work.  For instance if encoding=ascii,
   * and the first thing that the doc does is the switch to
   * latin1.  Then, there is an encoding closed that was not opened in the ps.
   */
/*  set_encoding (job, job->requested_encoding);*/
  job->saved_encoding = job->requested_encoding;
}

/*
 * Finish the poscript printing
 */
void
ps_end (a2ps_job * job)
{
/* Make sure that the output is really processed.  It
   * could have been closed by a page range selection, but
   * the trailer must be dumped */
  output_to_void (job->divertion, false);

  /* Close the postscript file */
  end_document (job);

  /* Undivert */
  undivert (job);
}
