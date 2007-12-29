/*
 * psgen.c
 * routines for the postscript generation
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

/*
 * $Id: psgen.c,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */

#include "a2ps.h"		/* most global variables 		*/
#include "prolog.h"
#include "psgen.h"
#include "routines.h"		/* general interest routines		*/
#include "output.h"		/* Diverted outputs			*/
#include "media.h"
#include "faces.h"
#include "metaseq.h"
#include "message.h"
#include "fonts.h"
#include "dsc.h"
#include "jobs.h"
#include "fjobs.h"
#include "prange.h"

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

/* Is this the first page for the current file?		*/
#define is_first_page	\
    ((job->pages - CURRENT_FILE (job)->first_page) > 1)

#define jdiv	job->divertion

/* return the max of two >-comparable stuff */
#define A2_MAX(X,Y)        (((X) > (Y)) ? (X) : (Y))


/****************************************************************/
/*		Formating help routines				*/
/****************************************************************/
/*
 * Print a char in a form accepted by postscript printers.
 * Returns number of columns used (on the output) to print the char.
 */
static int
ps_escape_char (a2ps_job * job, uchar c, uchar * res)
{
  int len=0;

  /* The number of columns used must be calculated here because of the
   * \ before non-ascii chars, and (, ), and \ itself */

  /* Printable and 7bit clean caracters */
  if (' ' <= c && c < 0177) {
    if (c == '(' || c == ')' || c == '\\')
      USTRCCAT(res, '\\');
    USTRCCAT(res, c);
    return 1;
  }

  /* Printable, but not 7bit clean caracters */
  if (encoding_char_exists (job->encoding, job->status->face, c)
      && ((0177 < c) || (c < 040))) {
    sprintf ((char *)res, "%s\\%o", res, c);
    return 1;
  }

  /* We are dealing with a non-printing character */
  job->status->nonprinting_chars++;

  /* We do this in here for speed.  We could have first escaped, then
    consider the new string as the string to ps-escape. */
  switch (job->unprintable_format) {
  case octal:
    sprintf ((char *) res, "\\\\%03o", c);
    return 4;

  case hexa:
    sprintf ((char *) res, "\\\\x%02x", c);
    return 4;

  case question_mark:
    USTRCCAT (res, '?');
    return 1;

  case space:
    USTRCCAT (res, ' ');
    return 1;

  case caret:
    if (0177 < c) {
      ustrcat(res, "M-");
      len += 2;
      c &= 0177;
    }

    if (c < ' ') {
      USTRCCAT(res, '^');
      len += 2;
      c += '@';
      if (c == '(' || c == ')' || c == '\\')
	USTRCCAT(res, '\\');
      USTRCCAT(res, c);
    } else if (c == 0177) {
      ustrcat(res, "^?");
      len += 2;
    } else {
      if (c == '(' || c == ')' || c == '\\')
	USTRCCAT(res, '\\');
      USTRCCAT(res, c);
      len++;
    }
    return len;

  case Emacs:
    if (0177 < c) {
      ustrcat(res, "M-");
      len += 2;
      c &= 0177;
    }

    if (c < ' ') {
      ustrcat (res, "C-");
      len += 3;
      c += '@';
      if (c == '(' || c == ')' || c == '\\')
	USTRCCAT(res, '\\');
      USTRCCAT(res, c);
    } else if (c == 0177) {
      ustrcat(res, "C-?");
      len += 3;
    } else {
      if (c == '(' || c == ')' || c == '\\')
	USTRCCAT(res, '\\');
      USTRCCAT(res, c);
      len++;
    }
    return len;

  }
  return 0;
}

/*
 * Print a string in a form accepted by postscript printers.
 */
static int
ps_escape_string (a2ps_job * job, uchar * string, uchar * res)
{
  size_t i;
  int delta_column=0;

  for (i = 0 ; i < ustrlen (string) ; i++)
    delta_column += ps_escape_char (job, string[i], res);

  return delta_column;
}

/*
 * Output the formated marker.
 */
static void
output_marker (a2ps_job * job, const char * kind, uchar * marker)
{
  uchar *cp, buf[256];
  int i;

  cp = expand_user_string (job, CURRENT_FILE (job),
			   (const uchar *) kind, marker);

  for (i = 0 ; cp[i] ; i++) {
    switch (cp[i]) {
    case FILE_LAST_PAGE:
      output_delayed_int (jdiv, &(CURRENT_FILE (job)->last_page));
      break;
    case FILE_LAST_SHEET:
      output_delayed_int (jdiv, &(CURRENT_FILE (job)->last_sheet));
      break;
    case FILE_NB_PAGES:
      output_delayed_int (jdiv, &(CURRENT_FILE (job)->pages));
      break;
    case FILE_NB_SHEETS:
      output_delayed_int (jdiv, &(CURRENT_FILE (job)->sheets));
      break;
    case FILE_NB_LINES:
      output_delayed_int (jdiv, &(CURRENT_FILE (job)->lines));
      break;
    case JOB_NB_PAGES:
      output_delayed_int (jdiv, &job->pages);
      break;
    case JOB_NB_SHEETS:
      output_delayed_int (jdiv, &job->sheets);
      break;
    case JOB_NB_FILES:
      output_delayed_int (jdiv, &job->total_files);
      break;
    default:
      *buf = '\0';
      ps_escape_char (job, cp[i], buf);
      output (jdiv, (char *) buf);
      break;
    }
  }
}

/****************************************************************/
/*		Dealing with the encodings			*/
/****************************************************************/
/*
 * Change the current encoding
 */
void
ps_set_encoding (a2ps_job * job, struct encoding * enc)
{
  set_encoding (job, enc);
  job->status->opened_encoding = enc;
  output (jdiv, "%sdict begin\n", encoding_get_key (enc));
}

void
ps_end_encoding (a2ps_job * job)
{
  if (job->status->opened_encoding)
    output (jdiv, "end %% of %sdict\n",
	    encoding_get_key (job->status->opened_encoding));
  set_encoding (job, NULL);
  job->status->opened_encoding = NULL;
}

/*
 * Switch of encoding while we are _not_ in the body.  E.g., we
 * are dealing with the headers.
 *
 * We want to save the previous encoding, because, say we
 * were printing -X latin1, we have switched to latin2 dynamically.
 * Right now, we need to change page,
 * so we are using back latin1, but we must remember of latin2
 * for the following page.
 */
static void
ps_internal_switch_encoding (a2ps_job * job, struct encoding * enc)
{
  if (job->encoding != enc) {
    /* If there are no dictionary opened yet, it is because we are
     * changing of dict outside the text body (e.g. even before
     * the first output char).  In this case, do not open a
     * ps dict: it is not time yet.
     */
    if (job->status->opened_encoding) {
      ps_end_encoding (job);
      ps_set_encoding (job, enc);
    } else {
      ps_end_encoding (job);
      set_encoding (job, enc);
    }
    /* Make sure to re-declare the font */
    job->status->face_declared = false;
  }
}

/*
 * Switch the encoding during the print process (ie. from
 * within the body of the text)
 * We have to take care of closing the line currently being printed
 * and restore the state, i.e. opening a string to print.
 */
void
ps_switch_encoding (a2ps_job * job, struct encoding * enc)
{
  if (job->encoding != enc)
    {
      if (!job->status->start_line) {
	if (!job->status->face_declared)
	  output (jdiv, ") %s\n", face_eo_ps (job->status->face));
	else
	  output (jdiv, ") S\n");
      }
      ps_internal_switch_encoding (job, enc);
      job->saved_encoding = enc;
      if (!job->status->start_line)
	output_char (jdiv, '(');
    }
}

static void
ps_push_encoding (a2ps_job * job, struct encoding * enc)
{
  job->saved_encoding = job->encoding;
  ps_internal_switch_encoding (job, enc);
}

static void
ps_pop_encoding (a2ps_job * job)
{
  ps_internal_switch_encoding (job, job->saved_encoding);
}

/****************************************************************/
/*		Printing a physical page			*/
/****************************************************************/
/*
 * Print the prolog necessary for printing each physical page.
 * Adobe convention for page independence is enforced through this routine.
 * Note that is may called for a second time on a single sheet if two
 * different files are printed on the same sheet.
 */
static void
page_begin (a2ps_job * job)
{
  job->virtual = 1;

  if (print_page (job, job->pages))
    job->sheets++;

  /* The number of pages has not been updated yet: add 1 */
  CURRENT_FILE (job)->top_page = CURRENT_FILE (job)->pages + 1;
  /*
   * Getting ready to delay the page label
   * This means to build a new string handler, store it, and fill it later
   * We do this because one would like to have all the information on
   * what is on the current page before making the page label.
   */
  if (!output_is_to_void (jdiv))
    {
      output (jdiv, "%%%%Page: (");
      job->status->page_label = XMALLOC (uchar *, 1);
      output_delayed_string (jdiv, job->status->page_label);
      output (jdiv, ") %d\n", job->sheets);
    }

  /* Reinitialize state variables for each new sheet */
  output (jdiv, "%%%%BeginPageSetup\n");
  output (jdiv, "/pagesave save def\n");

  /* Shift front side sheets */
  if (job->margin
      && (job->duplex == simplex || (job->sheets % 2)))
    output (jdiv, "%d 0 translate\n", job->margin);

  if (job->orientation == landscape)
    output (jdiv, "sh 0 translate 90 rotate\n");

  output (jdiv, "%%%%EndPageSetup\n");

  if (job->debug)
    output (jdiv, "\
%% Display the bounding box\n\
  gsave\n\
    llx lly moveto\n\
    2 setlinewidth\n\
    0.9 setgray\n\
    urx lly lineto\n\
    urx ury lineto\n\
    llx ury lineto\n\
    closepath stroke\n\
  grestore\n\n");

  /* Set the encoding */
  ps_set_encoding (job, job->requested_encoding);

  /* water marks (only once per sheet) */
  if (!IS_EMPTY(job->water))
    {
      output_char (jdiv, '(');
      output_marker (job, "water mark", job->water);
      output (jdiv,
	      ") %4.2f water\n",
	      ((float) atan2 ((double) job->medium->w - job->margin,
			      (double) job->medium->h)
	       / 3.14159265 * 180));
    }

  /* Move to the lower left point of the drawable area */
  output (jdiv, "gsave\n");
  output (jdiv, "llx lly %d add translate\n",
	  PRINT_FOOTER * HEADERS_H);
  /* Set the encoding */
  ps_internal_switch_encoding (job, job->saved_encoding);
}

/* The job on the page is over: puts the headers and footers,
 * then print the physical page.
 */
static void
page_end (a2ps_job * job)
{
  /* The page label has been divertered (through a handler).
   * Fill that handler with the correct page label value
   */
  *(job->status->page_label) =
    xustrdup (expand_user_string (job, CURRENT_FILE (job),
				  (const uchar *) "Page label",
				  job->status->page_label_format));

  output (jdiv, "grestore\n");

  /* All the headers should be written using the requested encoding */
  ps_push_encoding (job, job->requested_encoding);

  /* Print the right header */
  if (PRINT_HEADER) {
    output_char (jdiv, '(');
    output_marker (job, "right header", job->header);
    output (jdiv, ") rhead\n");
  }

  /* Print the center footer.
   * Use dynamic markers in priority
   */
  if (PRINT_FOOTER) {
    if (!IS_EMPTY(job->footer)) {
      output_char (jdiv, '(');
      output_marker (job, "center footer", job->footer);
      output (jdiv, ") ");
    } else
      output (jdiv, "() ");

    /* Print the right footer */
    if (!IS_EMPTY(job->right_footer)) {
      output_char (jdiv, '(');
      output_marker (job, "right footer", job->right_footer);
      output (jdiv, ") ");
    } else
      output (jdiv, "() ");

    /* Print the left footer */
    if (!IS_EMPTY(job->left_footer)) {
      output_char (jdiv, '(');
      output_marker (job, "left footer", job->left_footer);
      output (jdiv, ") ");
    }
    else
      output (jdiv, "() ");
    output (jdiv, "footer\n");
  }

  /* Close the current encoding */
  ps_end_encoding (job);

  output (jdiv, "pagesave restore\n");
  output (jdiv, "showpage\n");

  job->virtual = 0;
}

/****************************************************************/
/*		Printing a virtual page				*/
/****************************************************************/
/*
 * Prints page header and page border and
 * initializes printing of the file lines.
 */
static void
virtual_begin (a2ps_job * job)
{
  job->pages++;

  if (print_page (job, job->pages)) {
    output_to_void (jdiv, false);
    job->virtual++;
  } else {
    output_to_void (jdiv, true);
  }

  /* We test to one, since it has just been incremented above */
  if (job->virtual == 1)
    page_begin (job);

  if (!job->encoding)
    ps_set_encoding (job, job->saved_encoding);

  file_job_synchronize_pages (job);
  file_job_synchronize_sheets (job);

  output (jdiv, "/v %d store\n", job->virtual - 1);
  output (jdiv, "/x0 x v get %f add sx cw mul add store\n",
	  SIDE_MARGIN_RATIO * job->fontsize * 0.6);
  output (jdiv, "/y0 y v get bfs %s sub store\n",
	  PRINT_TITLE ? "th add" : "");
  output (jdiv, "x0 y0 moveto\n");
}

/*
 * Adds a sheet number to the page (footnote) and prints the formatted
 * page (physical impression). Activated at the end of each source page.
 */
static void
virtual_end (a2ps_job * job)
{
  /*
   *	Print the titles with the option-given encoding
   */
  /* Draw the header and its content */
  if (PRINT_TITLE) {
    ps_push_encoding (job, job->requested_encoding);
    if (!IS_EMPTY(job->center_title)) {
      output_char (jdiv, '(');
      output_marker (job, "center title", job->center_title);
      output (jdiv, ") ");
    } else
      output (jdiv, "() ");

    if (!IS_EMPTY(job->right_title)) {
      output_char (jdiv, '(');
      output_marker (job, "right title", job->right_title);
      output (jdiv, ") ");
    } else
      output (jdiv, "() ");

    if (!IS_EMPTY(job->left_title)) {
      output_char (jdiv, '(');
      output_marker (job, "left title", job->left_title);
      output (jdiv, ") ");
    } else
      output (jdiv, "() ");

    output (jdiv, "title\n");
    ps_pop_encoding (job);
  }

  if (job->border)
    output (jdiv, "border\n");

  if (job->virtual == (job->columns * job->rows))
    page_end (job);

  job->status->line = 0;
}

/* Issue an empty virtual (used for file alignment). */

static void
virtual_empty_output (a2ps_job *job)
{
  /* Let's use a hidden option which lets choose between an empty
     virtual (with headers etc.), or nothing printed at all. */
  if (macro_meta_sequence_get (job, VAR_OPT_VIRTUAL_FORCE))
    {
      virtual_begin (job);
      virtual_end (job);
    }
  else
    {
      /* Just increase the virtual number, that's enough. */
      job->pages++;
      job->virtual++;
      if (job->virtual == (job->columns * job->rows))
	page_end (job);
    }
}

/*
 * Flush the (physical) page, ready for new page
 * Used
 * -- at the end of the whole job
 * -- at the end of a page
 * -- when a fresh page is required (eg, when after an a2ps
 *    generated ps page arrives a delegated ps file)
 */
void
page_flush (a2ps_job * job)
{
  /* Everything that follows _must_ be dumped */
  output_to_void (job->divertion, false);

  /* If the sheet has not been printed, flush it */
  if (job->virtual != 0)
    page_end (job);
}

/* Output an empty page (used for instance to align files.
   No page should be waiting (issue flushes before). */

static void
page_empty_output (a2ps_job *job)
{
  job->sheets++;
  file_job_synchronize_sheets (job);
  output (jdiv, "%%%%Page: (*) %d\n", job->sheets);
  output (jdiv, "%% Empty Page\n");
  output (jdiv, "showpage\n");
}

static void
sheet_flush (a2ps_job *job)
{
  page_flush (job);

  /* Need an empty back side? */
  if ((job->duplex == duplex || job->duplex == tumble)
      && (job->sheets % 2) != 0)
    page_empty_output (job);
}

/*
 * Make sure this a new sheet.
 * No DSC is done on the new page.  This is used when delegated
 * ps file is inlined.
 */
void
require_fresh_page (a2ps_job * job)
{
  /* If this is not a blank sheet, end it */
  if (job->virtual != 0)
    /* The clean up _must_ be done */
    page_flush (job);
}

/****************************************************************/
/*		Service routines				*/
/****************************************************************/
/*
 * Test if we have a binary file.
 *
 * Printing binary files is not very useful. We stop printing
 * if we detect one of these files. Our heuristic to detect them:
 * if 40% characters are non-printing characters,
 * the file is a binary file. (40% is taken from the case of a2ps istself).
 */
static void
check_binary_file (a2ps_job * job)
{
  if (job->status->chars > 120)
    {
      if (!job->print_binaries
	  && (job->status->nonprinting_chars*100 / job->status->chars) >= 40)
	error (1, 0, _("`%s' is a binary file, printing aborted"),
	       CURRENT_FILE (job)->name);
    }
}

static inline void
end_of_line (struct a2ps_job * job)
{
  if (!job->status->face_declared) {
    output (jdiv, ") %s n\n", face_eo_ps (job->status->face));
    job->status->face_declared = true;
  } else
    output (jdiv, ") N\n");
  job->status->line++;
  job->status->column = 0;
  job->status->wx = 0;
}

#define page_full	\
	(job->status->line >= job->status->linesperpage)

#define line_full (job->status->wx > job->status->wxperline)

/*
 * Fold a line too long.
 */
static inline void
fold_line (struct a2ps_job * job, enum face_e new_face)
{
  job->lines_folded++;
  end_of_line (job);
  if (page_full)
    {
      virtual_end (job);
      virtual_begin (job);
      job->status->face_declared = false ;
    }
  if (job->numbering)
    {
      output (jdiv, "0 T (");
    }
  else
    {
      /* This is a new line, hence we can consider that there is no
	 need to close the current font: just consider it is the new
	 font, but not declared. */
      output_char (jdiv, '(');
      job->status->face_declared &= (job->status->face
				     == new_face);
      job->status->face = new_face;
  }
}

/*
 * Print a char
 */
void
ps_print_char (a2ps_job * job, int c, enum face_e new_face)
{
  /*
   * Preprocessing (before printing):
   * - TABs expansion (see interpret option)
   * - FF and BS interpretation
   * - replace non printable characters by a space or a char sequence
   *   like:
   *     ^X for ascii codes < 0x20 (X = [@, A, B, ...])
   *     ^? for del char
   *     M-c for ascii codes > 0x3f
   * - prefix parents and backslash ['(', ')', '\'] by backslash
   *   (escape character in postcript)
   */
  if (job->status->is_in_cut
      && (c != '\f' )
      /* FIXME: May be some day, using a more flexible scheme
       * would be good
      && (c != encodings[job->encoding].new_line))*/
      && (c != '\n'))
    return;
  job->status->is_in_cut = false;

  /* Start a new line ? */
  if (job->status->start_line)
    {
      if (job->status->start_page)
	{
	  /* only if there is something to print! */
	  virtual_begin (job);
	  job->status->start_page = false ;
	  /* This is the first line of a new page, hence we need (page
	   * independance) to repeat the current font */
	  job->status->face = new_face;
	  job->status->face_declared = false;

	  if (job->numbering)
	    {
	      if (CURRENT_FILE (job)->lines % job->numbering == 0)
		output (jdiv, "(%d) # (", CURRENT_FILE (job)->lines);
	      else
		output (jdiv, "0 T (");
	    }
	  else
	    output_char (jdiv, '(');
	}
      else
	{
	  /* This is a new line, but not the first in the page */
	  if (job->numbering)
	    {
	      if (CURRENT_FILE (job)->lines % job->numbering == 0)
		output (jdiv, "(%d) # (", CURRENT_FILE (job)->lines);
	      else
		output (jdiv, "0 T (");
	    }
	  else
	    {
	      /* This is a new line, hence we can consider that there is
		 no need to close the current font: just consider it is
		 the new font, but not declared. */
	      output_char (jdiv, '(');
	      /* Why the hell did I do this? */
	      /* FIXME: This is suppresed because of the changes of encoding */
	      job->status->face_declared = (job->status->face_declared
					    && (job->status->face == new_face));
	      job->status->face = new_face;
	    }
	}
      job->status->start_line = false;
    }

  /*
   * Interpret each character
   *
   *  One of the tricky stuff is that we want to avoid uncessary
   * Changes of font.  For instance, I see no point in
   * Updating the font right before a \n.
   */
  switch (c) {
  case '\f':  		/* Form feed */
    if (!job->interpret)
      goto print;

    /* Close current line */
    if (!job->status->start_line)
      {
	end_of_line (job);
	job->status->start_line = true;
      }
    /* start a new page ? */
    if (job->status->start_page)
      {
	virtual_begin (job);
      }
    /* Close current page and begin another */
    virtual_end (job);
    job->status->start_page = true;
    break;

  case '\n':
#if 0
  /* Now the primary eol is \n.  It is up to a2ps-prog to change the
   * \r or \n\r to \n
   * The program  */
  case '\r':		/* One of these is just a plain character */
    if (c != encodings[job->encoding].new_line)
      goto print;
#endif
    (CURRENT_FILE (job))->lines++;
    job->status->start_line = true;
    end_of_line (job);

    if (page_full) {
      virtual_end (job);
      job->status->start_page = true ;
    }
    break;

  case '\t':
    if (!job->interpret)
      goto print;

    /* Is this a new font? */
    if (job->status->face != new_face) {
      if (!job->status->face_declared)
	output (jdiv, ") %s\n(", face_eo_ps (job->status->face));
      else
	output (jdiv, ") S\n(");
      job->status->face = new_face;
      job->status->face_declared = false;
    }

    /* Tabs are interpreted  but we want to go to the same
     * column as if the font were Courier
     */
    job->status->column =
      (A2_MAX(job->status->wx / COURIER_WX, job->status->column)
       / job->tabsize + 1) * job->tabsize;
    job->status->wx = job->status->column * COURIER_WX;
    if (line_full) {
      if (job->folding) {
	fold_line (job, new_face);
      } else {
	job->status->is_in_cut = true;
	return;
      }
    }
    /* Make sure that the font is given */
    if (!job->status->face_declared) {
      output (jdiv, ") %s", face_eo_ps(job->status->face));
      job->status->face_declared = true;
    } else
      output (jdiv, ") S");
    output (jdiv, " %d T (", job->status->column);
    break;
  print:
  default:
    {
      uchar buf[256];
      int nchars;
      *buf = '\0';

  /* Is this a new font? */
  if (job->status->face != new_face) {
    if (!job->status->face_declared)
      output (jdiv, ") %s\n(", face_eo_ps (job->status->face));
    else
      output (jdiv, ") S\n(");
    job->status->face = new_face;
    job->status->face_declared = false;
  }

      nchars = ps_escape_char (job, c, buf);
      job->status->wx += char_WX (job, c);
      job->status->column += nchars;
      if (line_full) {
	if (job->folding) {
	  fold_line (job, new_face);
	  job->status->column = nchars;
	  job->status->wx = char_WX (job, c);
	} else {
	  job->status->is_in_cut = true;
	  return;
	}
      }
      output (jdiv, "%s", buf);
      job->status->chars++;
    }
    break;
  }
}

/*
 * Print the content of a C string \0 terminated
 */
void
ps_print_string (a2ps_job * job, uchar * string, enum face_e face)
{
  while (*string)
    ps_print_char (job, *(string++), face);
}

/*
 * Print the N first chars in BUFFER
 */
void
ps_print_buffer (a2ps_job * job,
		 const uchar * buffer,
		 size_t start, size_t end,
		 enum face_e face)
{
  size_t i;
  for (i = start ; i < end ; i++)
    ps_print_char (job, buffer [i], face);
}

/* Handling the input sessions in an output session, i.e., typically
   the files. */

/* A new file will be printed. */

void
ps_begin_file (a2ps_job *job)
{
  /* Reinitialize the ps status */
  initialize_ps_status (job->status);

  /* Alignment is not needed for the first file. */
  if (job->jobs->len == 0)
    return;

  switch (job->file_align)
    {
    case file_align_virtual:
      /* Nothing to do. */
      break;

    case file_align_rank:
      /* Issue virtual until we are in a new rank. */
      {
	int alignment = job->madir == madir_rows ? job->columns : job->rows;
	while (job->pages % alignment != 0)
	  virtual_empty_output (job);
      }
      break;

    case file_align_page:
      /* End the page if needed. */
      page_flush (job);
      break;

    case file_align_sheet:
      /* End the sheet if needed. */
      sheet_flush (job);
      break;

    default:
      /* job->file_align is a number.  We must issue as many pages as
         needed to have a page number which is a multiple of
         FILE_ALIGN plus one. */
      page_flush (job);
      while ((job->sheets) % job->file_align != 0)
	page_empty_output (job);
      break;
    }
}

void
ps_end_file (a2ps_job *job)
{
  /* If a line was not finished, close it.
   * Typically, no \n at end of file */
  if (!job->status->start_line)
    {
      if (!job->status->face_declared)
	output (jdiv, ") %s\n", face_eo_ps (job->status->face));
      else
	output (jdiv, ") S\n");
    }
  if (!job->status->start_page)
    virtual_end (job);

  /* Set the number of pages/sheets printed */
  file_job_synchronize_pages (job);
  file_job_synchronize_sheets (job);

  /* If we don't want to print binaries, complain and quit */
  check_binary_file (job);
}
