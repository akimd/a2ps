/*
 * main.c -- main loop, and interface with user
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-2000 Akim Demaille, Miguel Santana
 * Copyright (c) 2007 Akim Demaille, Miguel Santana and Masayuki Hatta
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


/************************************************************************/
/*                                                                      */
/*                      I n c l u d e   f i l e s                       */
/*                                                                      */
/************************************************************************/
#include <assert.h>

#include "a2ps.h"
#include "argmatch.h"
#include "confg.h"
#include "options.h"
#include "pathwalk.h"
#include "select.h"
#include "generate.h"
#include "printers.h"
#include "delegate.h"
#include "metaseq.h"
#include "regex.h"
#include "buffer.h"
#include "psgen.h"
#include "prolog.h"
#include "stream.h"
#include "getnum.h"
#include "title.h"
#include "useropt.h"
#include "main.h"
#include "lexps.h"
#include <signal.h>
#include "signame.h"
#include "long-options.h"
#include "version-etc.h"
#include <locale.h>

/* From basename.c */
char *base_name PARAMS ((const char *path));

/************************************************************************/
/*                                                                      */
/*                 G l o b a l   d e f i n i t i o n s                  */
/*                                                                      */
/************************************************************************/
/************************************************************************
 *	The various global behaviors
 */
enum behavior
  {
    b_ps,		/* postscript generator (usual PS converter)    */
    b_guess,		/* do as file(1) does: return the ssh file name */
    b_help,
    b_version,
    b_expand,		/* Expand the strings given as arguments. */
    b_which,		/* Look for the args in the path, and report. */
    b_glob,		/* Same, but with globbing. */
    b_list_options,
    b_list_features,
    b_list_media,
    b_list_style_sheets,
    b_list_html_style_sheets,
    b_list_texinfo_style_sheets,
    b_list_printers,
    b_list_delegations,
    b_list_macro_meta_sequences,
    b_list_encodings,
    b_list_texinfo_encodings,
    b_list_user_options,
    b_list_prologues,
    b_list_texinfo_prologues,
    b_list_ppd
  };

/* Stores the task to execute.  Default: a2ps. */

enum behavior behavior = b_ps;

/* Name under which this program is called.  To understand why it is
   defined twice, see lib/confg.gperf, handling of `Options:'. */

char *program_name;
const char *program_invocation_name;

/* Stores the data of liba2ps.  */

a2ps_job *job = NULL;


/* Syntax table for regex. */

char *re_syntax_table = NULL;

#define RE_SYNTAX_A2PS \
  (/* Allow char classes. */					\
    RE_CHAR_CLASSES						\
  /* Be picky. */						\
  | RE_CONTEXT_INVALID_OPS					\
  /* Allow intervals with `{' and `}', forbid invalid ranges. */\
  | RE_INTERVALS | RE_NO_BK_BRACES | RE_NO_EMPTY_RANGES		\
  /* `(' and `)' are the grouping operators. */			\
  | RE_NO_BK_PARENS						\
  /* `|' is the alternation. */					\
  | RE_NO_BK_VBAR)


/************************************************************************/
/*      Related to the config files                                     */
/************************************************************************/
/*
 * Hash table of the delegations
 */
struct hash_table_s *delegation_table;

/*
 * Content of sheets.map
 */
struct darray *sheets_map = NULL;

/*
 * Hash table of the sheet yet read
 */
struct hash_table_s *style_sheets = NULL;

/************************************************************************/
/*      Related to the options                                          */
/************************************************************************/
/*
 * Delegate files to other applications.
 */
bool delegate_p = true;

/*
 * --toc[=format], generate a table of content
 */
uchar *toc = NULL;

/*
 * -E: style sheet to use. NULL => automated
 */
char *style_request = NULL;

/*
 * -g/--highlight-level: 0, 1 or 2
 */
int highlight_level = 1;

/*
 * --strip=NUM, don't write the comments
 */
int strip_level = 0;

/*
 * --end-of-line=TYPE, specify what are the sequences of chars to
 * interpret as end of line
 */
enum eol_e end_of_line = eol_auto;

/************************************************************************/
/*                             Service routines                         */
/************************************************************************/
/*
 * Unlink all the used files.  Used for atexit
 */
static void
exit_handler (void)
{
  if (job)
    a2ps_job_unlink_tmpfiles (job);
  if (sample_tmpname)
    unlink (sample_tmpname);
}

static RETSIGTYPE
signal_handler (int signum)
{
  /* Error calls exit which calls atexit which removes the files. */
  error (EXIT_FAILURE, 0,
	 _("received signal %d: %s"), signum, strsignal (signum));
}

/************************************************************************
 * Read the highlighting level
 */
static const char *const highlight_level_args[] =
{
  "none", "off", "0",
  "normal", "light", "1",
  "heavy", "symbols", "2",
  0
};

static const int highlight_level_types[] =
{
  0, 0, 0,
  1, 1, 1,
  2, 2, 2
};

/*
 * Return the highlight_level value
 */
static int
get_highlight_level (const char *option, const char *arg)
{
  ARGMATCH_ASSERT (highlight_level_args, highlight_level_types);
  return XARGCASEMATCH (option, arg,
			highlight_level_args, highlight_level_types);
}

static char *
highlight_level_to_string (int level)
{
  switch (level)
    {
    case 2:
      /* TRANS: highlighting level = heavy (2/2) */
      return _("heavy");

    case 1:
      /* TRANS: highlighting level = normal (1/2) */
      return _("normal");

    case 0:
      /* TRANS: highlighting level = none (0/2) */
      return _("none");
    }
  return NULL;			/* For -Wall */
}

/************************************************************************
 * Read the --list argument
 */
static const char *const behavior_args[] =
{
  "defaults", "options", "settings",
  "features", "plugins",
  "delegations",
  "encodings", "charsets",
  "variables", "macro-meta-sequences",
  "media",
  "printers", "outputs",
  "style-sheets", "languages",
  "user-options", "shortcuts",
  "prologues",
  "texinfo-style-sheets", "ssh-texi",
  "html-style-sheets", "ssh-html",
  "texinfo-encodings", "edf-texi",
  "texinfo-prologues", "pro-texi",
  "ppd",
  "version", "release",
  "help", "usage",
  "expand",
  "which", "find",
  "glob",
  "ps",
  NULL
};

static const enum behavior behavior_types[] =
{
  b_list_options, b_list_options, b_list_options,
  b_list_features, b_list_features,
  b_list_delegations,
  b_list_encodings, b_list_encodings,
  b_list_macro_meta_sequences, b_list_macro_meta_sequences,
  b_list_media,
  b_list_printers, b_list_printers,
  b_list_style_sheets, b_list_style_sheets,
  b_list_user_options, b_list_user_options,
  b_list_prologues,
  b_list_texinfo_style_sheets, b_list_texinfo_style_sheets,
  b_list_html_style_sheets, b_list_html_style_sheets,
  b_list_texinfo_encodings, b_list_texinfo_encodings,
  b_list_texinfo_prologues, b_list_texinfo_prologues,
  b_list_ppd,
  b_version, b_version,
  b_help, b_help,
  b_expand,
  b_which, b_which,
  b_glob,
  b_ps
};

/************************************************************************/
/*                              Interface                               */
/************************************************************************/

/*------------------------------------------------------------------.
| Print information depending on the installation.  It is also used |
| to store useful information in the output to help us debuging the |
| users :).                                                         |
`------------------------------------------------------------------*/

static void
list_options (struct a2ps_job *a_job, FILE *stream)
{
#if 0
  /* This is just so that gettext knows I use those two strings. */
  static char *yes = N_("yes");
  static char *no  = N_("no");
#endif

#define bool_to_string(bool) ((bool) ? _("yes") : _("no"))
  uchar buf[256], buf2[256];
  const char *cp = NULL;
  uchar *ucp = NULL;

  /* Title of --list-options (%s%s is `a2ps' `version' */
  title (stream, '=', true,
	 _("Configuration status of %s %s\n"),
	 program_invocation_name, VERSION);
  putc ('\n', stream);

  title (stream, '-', false, _("Sheets:\n"));
  fprintf (stream, _("\
  medium          = %s%s, %s\n\
  page layout     = %d x %d, %s\n\
  borders         = %s\n\
  file alignment  = %s\n\
  interior margin = %d\n"),
	   a_job->medium->name,
	   (a2ps_medium_libpaper_p (job, job->medium_request)
	    ? " (libpaper)" : ""),
	   (a_job->orientation == portrait) ? _("portrait") : _("landscape"),
	   a_job->columns, a_job->rows,
	   madir_to_string (a_job->madir),
	   bool_to_string (a_job->border),
	   file_align_to_string (a_job->file_align),
	   a_job->margin);
  putc ('\n', stream);

  if (a_job->columns_requested > 0)
    sprintf ((char *) buf, _("%d characters per line"),
	     a_job->columns_requested);
  else if (a_job->lines_requested > 0)
    sprintf ((char *) buf, _("%d lines per page"),
	     a_job->lines_requested);
  else
    sprintf ((char *) buf, _("font size is %gpt"), a_job->fontsize);

  switch (a_job->numbering)
    {
    case 0:
      ustrcpy (buf2, _("no"));
      break;
    case 1:
      /* number line: each line */
      ustrcpy (buf2, _("each line"));
      break;
    default:
      /* number line: each %d line */
      sprintf ((char *) buf2, _("each %d lines"), a_job->numbering);
    }

  title (stream, '-', false, _("Virtual pages:\n"));
  fprintf (stream, _("\
  number lines         = %s\n\
  format               = %s\n\
  tabulation size      = %d\n\
  non printable format = %s\n"),
	   buf2,
	   buf,
	   a_job->tabsize,
	   unprintable_format_to_string (a_job->unprintable_format));
  putc ('\n', stream);

  title (stream, '-', false, _("Headers:\n"));
  fprintf (stream, _("\
  header       = %s\n\
  left footer  = %s\n\
  footer       = %s\n\
  right footer = %s\n\
  left title   = %s\n\
  center title = %s\n\
  right title  = %s\n\
  under lay    = %s\n"),
	   UNNULL (a_job->header),
	   UNNULL (a_job->left_footer),
	   UNNULL (a_job->footer),
	   UNNULL (a_job->right_footer),
	   UNNULL (a_job->left_title),
	   UNNULL (a_job->center_title),
	   UNNULL (a_job->right_title),
	   UNNULL (a_job->water));
  putc ('\n', stream);

  title (stream, '-', false, _("Input:\n"));
  fprintf (stream, _("\
  truncate lines = %s\n\
  interpret      = %s\n\
  end of line    = %s\n\
  encoding       = %s\n\
  document title = %s\n\
  prologue       = %s\n\
  print anyway   = %s\n\
  delegating     = %s\n"),
	   bool_to_string (!a_job->folding),
	   bool_to_string (a_job->interpret),
	   eol_to_string (end_of_line),
	   encoding_get_name (a_job->requested_encoding),
	   a_job->title,
	   a_job->prolog,
	   bool_to_string (a_job->print_binaries),
	   bool_to_string (delegate_p));
  putc ('\n', stream);

  /*
   * Pretty printing
   */
  if (IS_EMPTY (style_request))
    /* TRANS: a2ps -E --list=options.  Warning, this answer is also
       used for the PPD file.  Make it compatible with both.  */
    ustrcpy (buf, _("selected automatically"));
  else
    ustrcpy (buf, style_request);
  title (stream, '-', false, _("Pretty-printing:\n"));
  fprintf (stream, _("\
  style sheet     = %s\n\
  highlight level = %s\n\
  strip level     = %d\n"),
	   buf,
	   highlight_level_to_string (highlight_level),
	   strip_level);
  putc ('\n', stream);

  /*
   * Information on where will go the output
   */

  /* Make a nice message to tell where the output is sent */
  ucp = a2ps_flag_destination_to_string (a_job);

  /* Make a nice message to tell what version control is used */
  switch (a_job->backup_type)
    {
    case none:
      cp = _("never make backups");
      break;

    case simple:
      cp = _("simple backups of every file");
      break;

    case numbered_existing:
      /* appears in a2ps --version-=existing --list=defaults */
      cp = _("numbered backups of files already numbered,\n\
                            and simple of others");
      break;

    case numbered:
      cp = _("numbered backups of every file");
      break;
    }

  title (stream, '-', false, _("Output:\n"));
  fprintf (stream, _("\
  destination     = %s\n\
  version control = %s\n\
  backup suffix   = %s\n"),
	   ucp, cp, simple_backup_suffix);
  putc ('\n', stream);
  free (ucp);

  /*
   * PostScript report.
   * TRANS: to be aligned with `page prefeed ='
   */
  cp = a2ps_printers_request_ppdkey_get (a_job->printers);
  title (stream, '-', false, _("PostScript:\n"));
  fprintf (stream, _("\
  magic number              = %s\n\
  Printer Description (PPD) = %s\n\
  default PPD               = %s\n\
  page label format         = %s\n\
  number of copies          = %d\n\
  sides per sheet           = %s\n\
  page device definitions   = "),
	   a_job->status->magic_number,
	   cp ? cp : _("selected automatically"),
	   a2ps_printers_default_ppdkey_get (a_job->printers),
	   a_job->status->page_label_format,
	   a_job->copies,
	   (a_job->duplex == simplex
	    ? "Simplex"
	    : (a_job->duplex == duplex
	       ? "Duplex"
	       : "DuplexTumble")));

  list_pagedevice (a_job, stream);
  fprintf (stream, _("\
  statusdict definitions    = "));
  list_statusdict (a_job, stream);
  fprintf (stream, _("\
  page prefeed              = %s\n"),
	   bool_to_string (a_job->page_prefeed));
  putc ('\n', stream);


  /*
   * Internal Details
   */
  title (stream, '-', false, _("Internals:\n"));
  fprintf (stream, _("\
  verbosity level     = %d\n\
  file command        = %s\n\
  library path        = \n"),
	   msg_verbosity,
	   UNNULL (a_job->file_command));
  pw_fprintf_path (stream, "\t%s\n", a_job->common.path);
}


/*------------------------------------------------------------------.
| This is used in psgen to push into the PostScript the report of   |
| the state of a2ps when it produced the file.  I'm tired of        |
| fighting with users who don't really say everything on the state  |
| of their a2ps when something goes wrong.  Now I can ask a ps file |
| generated with --debug, and got everything I need.                |
`------------------------------------------------------------------*/

static void
spy_user (struct a2ps_job *a_job, FILE * stream)
{
#define PREFIX "% "
  FILE *spy;
  char *spyname;
  char buf[BUFSIZ];

  /* Use one of the temp file names so that cleanup can be correctly
     done. */
  tempname_ensure (job->tmp_filenames[0]);
  spyname = job->tmp_filenames[0];
  spy = fopen (spyname, "w");
  if (!spy)
    error (1, errno, _("cannot open file `%s'"), quotearg (spyname));


  /* Well, this is the information I've been fighting with some users
     to get them exact...  I hate doing that, but I need to save part
     of my time.  */
  fputs ("SPY-BEGIN\n", spy);
  fputs ((char *) expand_user_string (job, CURRENT_FILE (job),
				      (const uchar *) "Debugging info",
				      (const uchar *) "%V was called with #!$|| |\n\n"),
	 spy);

  list_options (a_job, spy);
  putc ('\n', spy);
  macro_meta_sequences_list_long (a_job, spy);
  fputs ("SPY-END\n", spy);

  /* Yes, I know, there are certainly better means.  Just teach them
     to me...  */
  fclose (spy);
  fopen (spyname, "r");
  if (!spy)
    error (1, errno, _("cannot open file `%s'"), quotearg (spyname));

  while (fgets (buf, sizeof (buf), spy))
    {
      fputs (PREFIX, stream);
      fputs (buf, stream);
    }
  fputs (PREFIX, stream);
  putc ('\n', stream);
  fclose (spy);
  unlink (spyname);
}


/*--------------------------------------------------.
| Print information depending on the installation.  |
`--------------------------------------------------*/

static void
list_features (struct a2ps_job *a_job, FILE * stream)
{
  /* Known languages */
  list_style_sheets_short (stream);
  putc ('\n', stream);

  /* Known char sets */
  list_encodings_short (a_job, stream);
  putc ('\n', stream);

  /* Known media */
  list_media_short (a_job, stream);
  putc ('\n', stream);

  /* Known prologues */
  prologues_list_short (a_job, stream);
  putc ('\n', stream);

  /* Known PPD files */
  a2ps_ppd_list_short (a_job, stream);
  putc ('\n', stream);

  /* Known "printers" */
  a2ps_printers_list_short (a_job, stream);
  putc ('\n', stream);

  /* Known "delegates" */
  delegations_list_short (delegation_table, stream);
  putc ('\n', stream);

  /* Known user options */
  user_options_list_short (a_job, stream);
  putc ('\n', stream);

  /* Macro meta seq. */
  macro_meta_sequences_list_short (a_job, stream);
}

/*------------------------.
| Print a usage message.  |
`------------------------*/

#define sfputs(String)	fputs (String, stream)
#define sputc(Char)	putc (Char, stream)

static void
usage (int status)
{
  /* Currently, there seem to be no use in being able to use another
     stream than STDOUT.  */
  FILE *stream = stdout;

  fprintf (stream, _("\
Usage: %s [OPTION]... [FILE]...\n\
\n\
Convert FILE(s) or standard input to PostScript.  By default, the output\n\
is sent to the default printer.  An output file may be specified with -o.\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
Long options marked with * require a yes/no argument, corresponding\n\
short options stand for `yes'.\n"),
	   program_invocation_name);

  /*
   * Does not print, and exits with success
   */
  sputc ('\n');
  sfputs (_("Tasks:\n"));
  sfputs (_("\
  --version        display version\n\
  --help           display this help\n\
  --guess          report guessed types of FILES\n\
  --which          report the full path of library files named FILES\n\
  --glob           report the full path of library files matching FILES\n\
  --list=defaults  display default settings and parameters\n\
  --list=TOPIC     detailed list on TOPIC (delegations, encodings, features,\n\
                   variables, media, ppd, printers, prologues, style-sheets,\n\
                   user-options)\n"));
  sputc ('\n');
  sfputs (_("\
After having performed the task, exit successfully.  Detailed lists may\n\
provide additional help on specific features.\n"));

  /*
   * Applies to the whole behavior
   */
  sputc ('\n');
  sfputs (_("Global:\n"));
  sfputs (_("\
  -q, --quiet, --silent      be really quiet\n\
  -v, --verbose[=LEVEL]      set verbosity on, or to LEVEL\n\
  -=, --user-option=OPTION   use the user defined shortcut OPTION\n\
      --debug                enable debugging features\n\
  -D, --define=KEY[:VALUE]   unset variable KEY or set to VALUE\n"));

  sputc ('\n');
  sfputs (_("Sheets:\n"));
  sfputs (_("\
  -M, --medium=NAME      use output medium NAME\n\
  -r, --landscape        print in landscape mode\n\
  -R, --portrait         print in portrait mode\n\
      --columns=NUM      number of columns per sheet\n\
      --rows=NUM         number of rows per sheet\n\
      --major=DIRECTION  first fill (DIRECTION=) rows, or columns\n\
  -1, -2, ..., -9        predefined font sizes and layouts for 1.. 9 virtuals\n\
  -A, --file-align=MODE  align separate files according to MODE (fill, rank\n\
                         page, sheet, or a number)\n\
  -j, --borders*         print borders around columns\n\
      --margin[=NUM]     define an interior margin of size NUM\n"));
  sputc ('\n');
  sfputs (_("\
The options -1.. -9 affect several primitive parameters to set up predefined\n\
layouts with 80 columns.  Therefore the order matters: `-R -f40 -2' is\n\
equivalent to `-2'.  To modify the layout, use `-2Rf40', or compose primitive\n\
options (`--columns', `--font-size' etc.).\n"));

  sputc ('\n');
  sfputs (_("Virtual pages:\n"));
  sfputs (_("\
      --line-numbers=NUM     precede each NUM lines with its line number\n\
  -C                         alias for --line-numbers=5\n\
  -f, --font-size=SIZE       use font SIZE (float) for the body text\n\
  -L, --lines-per-page=NUM   scale the font to print NUM lines per virtual\n\
  -l, --chars-per-line=NUM   scale the font to print NUM columns per virtual\n\
  -m, --catman               process FILE as a man page (same as -L66)\n\
  -T, --tabsize=NUM          set tabulator size to NUM\n\
  --non-printable-format=FMT specify how non-printable chars are printed\n"));

  sputc ('\n');
  sfputs (_("Headings:\n"));
  /* xgettext:no-c-format */
  sfputs (_("\
  -B, --no-header        no page headers at all\n\
  -b, --header[=TEXT]    set page header\n\
  -u, --underlay[=TEXT]  print TEXT under every page\n\
  --center-title[=TEXT]  set page title to TITLE\n\
  --left-title[=TEXT]    set left and right page title to TEXT\n\
  --right-title[=TEXT]\n\
  --left-footer[=TEXT]   set sheet footers to TEXT\n\
  --footer[=TEXT]\n\
  --right-footer[=TEXT]\n"));
  sputc ('\n');
  sfputs (_("\
The TEXTs may use special escapes.\n"));

  sputc ('\n');
  sfputs (_("Input:\n"));
  sfputs (_("\
  -a, --pages[=RANGE]        select the pages to print\n\
  -c, --truncate-lines*      cut long lines\n\
  -i, --interpret*           interpret tab, bs and ff chars\n\
      --end-of-line=TYPE     specify the eol char (TYPE: r, n, nr, rn, any)\n\
  -X, --encoding=NAME        use input encoding NAME\n\
  -t, --title=NAME           set the name of the job\n\
      --stdin=NAME           set the name of the input file stdin\n\
      --print-anyway*        force binary printing\n\
  -Z, --delegate*            delegate files to another application\n\
      --toc[=TEXT]           generate a table of content\n"));
  sputc ('\n');
  sfputs (_("\
When delegations are enabled, a2ps may use other applications to handle the\n\
processing of files that should not be printed as raw information, e.g., HTML\n\
PostScript, PDF etc.\n"));

  sputc ('\n');
  sfputs (_("Pretty-printing:\n"));
  sfputs (_("\
  -E, --pretty-print[=LANG]  enable pretty-printing (set style to LANG)\n\
  --highlight-level=LEVEL    set pretty printing highlight LEVEL\n\
                             LEVEL can be none, normal or heavy\n\
  -g                         alias for --highlight-level=heavy\n\
  --strip-level=NUM          level of comments stripping\n"));

  sputc ('\n');
  sfputs (_("Output:\n"));
  sfputs (_("\
  -o, --output=FILE          leave output to file FILE.  If FILE is `-',\n\
                             leave output to stdout.\n\
  --version-control=WORD     override the usual version control\n\
  --suffix=SUFFIX            override the usual backup suffix\n\
  -P, --printer=NAME         send output to printer NAME\n\
  -d                         send output to the default printer\n\
                             (this is the default behavior)\n"));

  sputc ('\n');
  sfputs (_("PostScript:\n"));
  sfputs (_("\
      --prologue=FILE        include FILE.pro as PostScript prologue\n\
      --ppd[=KEY]            automatic PPD selection or set to KEY\n\
  -n, --copies=NUM           print NUM copies of each page\n\
  -s, --sides=MODE           set the duplex MODE (`1' or `simplex',\n\
                             `2' or `duplex', `tumble')\n\
  -S, --setpagedevice=K[:V]  pass a page device definition to output\n\
      --statusdict=K[:[:]V]  pass a statusdict definition to the output\n\
  -k, --page-prefeed         enable page prefeed\n\
  -K, --no-page-prefeed      disable page prefeed\n"));


  /* A short documentation. */
  sputc ('\n');
  sfputs (_("\
By default a2ps is tuned to do what you want to, so trust it.  To pretty\n\
print the content of the `src' directory and a table of content, and send the\n\
result to the printer `lw',\n\
\n\
    $ a2ps -P lw --toc src/*\n\
\n\
To process the files `sample.ps' and `sample.html' and display the result,\n\
\n\
    $ a2ps -P display sample.ps sample.html\n\
\n\
To process a mailbox in 4 up,\n\
\n\
    $ a2ps -=mail -4 mailbox\n\
\n\
To print as a booklet on the default printer, which is Duplex capable,\n\
\n\
    $ a2ps -=book paper.dvi.gz -d\n"));

  /* Finally, some addresses. */
  sputc ('\n');
  sfputs (_("\
News, updates and documentation: visit http://www.gnu.org/software/a2ps/.\n"));
  sfputs (_("Report bugs to <bug-a2ps@gnu.org>.\n"));

  exit (status);
}


/*----------------------------------------------------------------.
| Handle the options that a2ps understands (not liba2ps) Return 1 |
| for success                                                     |
`----------------------------------------------------------------*/

static int
handle_a2ps_option (int option, char *optional_arg)
{
  switch (option)
    {
    case 'E':			/* --pretty-print select language */
      xstrcpy (style_request, optional_arg);
      break;

    case 'g':			/* Symbol translation */
      highlight_level = 2;
      break;

    case 154:			/* Symbol translation */
      highlight_level =
	a2ps_get_bool ("--graphic-symbols", optional_arg);
      break;

    case 173:			/* Level of high lighting */
      highlight_level =
	get_highlight_level ("--highlight-level", optional_arg);
      break;

    case 'h':			/* --help */
      behavior = b_help;
      break;

    case 'V':			/* version and configuration info */
      behavior = b_version;
      break;

    case 'Z':			/* --delegate */
      delegate_p = true;
      break;

    case 138:
      behavior = b_guess;
      break;

    case 137:
      behavior = b_which;
      break;

    case 150:
      behavior = b_glob;
      break;

    case 139:
      behavior = b_list_options;
      break;

    case 145:
      ARGMATCH_ASSERT (behavior_args, behavior_types);
      behavior = XARGCASEMATCH ("--list", optional_arg,
				behavior_args, behavior_types);
      break;

    case 148:			/* --strip-level */
      strip_level =
	get_integer_in_range ("--strip-level", optional_arg,
			      0, 3, range_min_max);
      break;

    case 160:			/* --delegate=BOOL */
      delegate_p = a2ps_get_bool ("--delegate", optional_arg);
      break;

    case 161:			/* --list-media */
      behavior = b_list_media;
      break;

    case 162:			/* --list-style-sheets  */
      behavior = b_list_style_sheets;
      break;

    case 167:			/* --toc[=toc format]           */
      /* If no argument is given, use #{toc}. */
      xustrcpy (toc, optional_arg ? optional_arg : "#{toc}");
      break;

    case 169:			/* --end-of-line=TYPE           */
      end_of_line = option_string_to_eol ("--end-of-line", optional_arg);
      break;

    default:
      return 0;
    }
  return 1;
}

/************************************************************************/
/*                      Main routine for this program.                  */
/************************************************************************/
int
main (int argc, char *argv[])
{
  int argn;

  /* Architecture specific initialization. */
#ifdef __EMX__
  /* Wildcard expansion for OS/2 */
  _wildcard (&argc, &argv);
#endif

  /* Name under which this program was called. */
  program_name = base_name (argv[0]);
  program_invocation_name = xstrdup (program_name);
  version_etc_copyright = N_("\
Copyright (c) 1988-1993 Miguel Santana\n\
Copyright (c) 1995-2000 Akim Demaille, Miguel Santana\n\
Copyright (c) 2007- Akim Demaille, Miguel Santana and Masayuki Hatta");

  /* Set the NLS on */
  setlocale (LC_TIME, "");
#ifdef HAVE_LC_MESSAGES
  setlocale (LC_MESSAGES, "");
#endif
  setlocale (LC_CTYPE, "");

  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  /* People don't want initializations when they just request a --help
     or --version. */
  parse_long_options (argc, argv,
		      NULL, GNU_PACKAGE, VERSION,
		      "Akim Demaille, Miguel Santana",
		      usage);

  /* Catch the exits and signals to cleanup the mess.

     We do it now, though there is no tmp files before long, because I
     find it beautiful to see `received signal blah blah' even if it
     is even before a2ps could make a move.  */
  atexit (exit_handler);
  signame_init ();
#define signal_set(Sig, Handler)		\
 do {						\
   if (signal (Sig, Handler) == SIG_IGN)	\
     signal (Sig, SIG_IGN);			\
 } while (0)
  /* There are warnings on Solaris.  This is due to their definition
     of SIG_IGN as `(void (*)())1'.  Please ignore ;) */
  signal_set (SIGINT, signal_handler);
#ifdef SIGHUP
  signal_set (SIGHUP, signal_handler);
#endif
  signal_set (SIGTERM, signal_handler);
#ifdef SIGPIPE
  signal (SIGPIPE, signal_handler);
#endif

  /* Hooks for reading the config files */
  delegation_hook = add_delegation;

  /* Hook when reading the options */
  handle_option_hook = handle_a2ps_option;

  /* Prepare to receive in the hash table of the delegations */
  delegation_table = delegation_table_new ();

  /* Set the syntax that has be chosen for regexp */
  re_set_syntax (RE_SYNTAX_A2PS);

  /* The spine of a2ps */
  job = a2ps_job_new ();

  /* System's config.    */
  a2_read_sys_config (job);

  /* Personal config.: only when installed, because there may be
     incompatibilities between config files versions. */
  if (!getenv ("NO_HOME_CONF"))
    /* Home's */
    a2_read_config (job,
		    macro_meta_sequence_get (job, VAR_USER_HOME),
		    ".a2ps/a2psrc");

  /* Local config. */
  a2_read_config (job, ".", ".a2psrc");

  /* Prepare the sheets map structure and the hash table that receives
     yet read sheets. */
  sheets_map = sheets_map_new ();
  style_sheets = new_style_sheets ();

  /* Process the command line options. */
  argn = a2ps_handle_options (job, argc, argv);

  /* Once a2ps.cfg is read, finish the building of a2ps_job */
  a2ps_job_finalize (job);

  /* If we are debugging, then install a hook called after having
     generated the PostScript comments. */
  if (job->debug)
    ps_comment_hook = spy_user;

  /* Attach the arguments to the JOB */
  job->argv = argv;
  job->argc = argc;

  switch (behavior)
    {
    case b_guess:
      /* Act like file(1) does: report guessed ssh key */
      if (argn < argc)
	for (; argn < argc; argn++)
	  guess ((uchar *) argv[argn]);
      else
	/* A guess is asked upon stdin */
	guess (UNULL);
      break;

      /* FIXME: for expand, which, and glob, should we give an error
         when no arguments are given? */

    case b_expand:
      /* Expand the strings given as arguments. */
      for (; argn < argc; argn++)
	{
	  fputs (expand_user_string (job, FIRST_FILE (job),
				     "--list=expand", (uchar *) argv[argn]),
		 stdout);
	  putc ('\n', stdout);
	}
      break;

    case b_which:
      /* Look for the arguments in the library, and report the full
         paths. */
      for (; argn < argc; argn++)
	{
	  char *cp;
	  cp = pw_find_file (job->common.path, (uchar *) argv[argn], NULL);
	  if (cp)
	    {
	      fputs (cp, stdout);
	      putc ('\n', stdout);
	    }
	}
      break;

    case b_glob:
      /* Glob the arguments in the library, and report the full
         paths. */
      for (; argn < argc; argn++)
	pw_glob_print (job->common.path, (uchar *) argv[argn], stdout);
      break;

    case b_version:
      version_etc (stdout, NULL, GNU_PACKAGE, VERSION,
		   "Akim Demaille, Miguel Santana");
      break;

    case b_help:
      usage (0);
      break;

    case b_list_options:
      list_options (job, stdout);
      break;

    case b_list_features:
      list_features (job, stdout);
      break;

    case b_list_media:
      list_media_long (job, stdout);
      break;

    case b_list_style_sheets:
      list_style_sheets_long (stdout);
      break;

    case b_list_html_style_sheets:
      /* This is done to ease the update of a2ps' web page */
      list_style_sheets_html (stdout);
      break;

    case b_list_texinfo_style_sheets:
      /* This is done to ease the update of a2ps' Texinfo doc */
      list_style_sheets_texinfo (stdout);
      break;

    case b_list_printers:
      a2ps_printers_list_long (job, stdout);
      break;

    case b_list_delegations:
      delegations_list_long (delegation_table, stdout);
      break;

    case b_list_macro_meta_sequences:
      macro_meta_sequences_list_long (job, stdout);
      break;

    case b_list_encodings:
      list_encodings_long (job, stdout);
      break;

    case b_list_texinfo_encodings:
      list_texinfo_encodings_long (job, stdout);
      break;

    case b_list_user_options:
      user_options_list_long (job, stdout);
      break;

    case b_list_prologues:
      prologues_list_long (job, stdout);
      break;

    case b_list_texinfo_prologues:
      prologues_list_texinfo (job, stdout);
      break;

    case b_list_ppd:
      /* Report PPD files     */
      a2ps_ppd_list_long (job, stdout);
      break;

      /*
       * Text to PostScript generator
       */
    case b_ps:
      {
	/* Count the number of jobs done */
	int delegated_jobs = 0, native_jobs = 0;

	a2ps_open_output_session (job);

	if (argn == argc)	/* Print stdin */
	  print (UNULL, &native_jobs, &delegated_jobs);
	else			/* Print following files */
	  for (; argn < argc; argn++)
	    print ((uchar *) argv[argn], &native_jobs, &delegated_jobs);

	if (!IS_EMPTY (toc))
	  print_toc ((uchar *) _("Table of Content"), toc, &native_jobs);

	if ((native_jobs == 0) && (delegated_jobs == 1))
	  {
	    /* a2ps has only been used to delegate a single job.
	     * Hence its prologue is superfluous */
	    /* FIXME: if there were other files but which failed,
	     * then there is _no_reason_ that the file we're interested
	     * in is this one!
	     * To this end, we need to put more information in file_job
	     * on how its processing went. */
	    
	    struct file_job * file_job;
	    size_t len;
	    
	    /* 'delegation_tmpname' is necessary not null else it is a
	       failed job and we ignore it */
	    file_job = CURRENT_FILE (job);
	    len = job->jobs->len;
	    while (!file_job->delegation_tmpname)
	      {
		len--;
		file_job = job->jobs->content[len - 1];
	      }
	    
	    a2ps_open_output_stream (job);
	    pslex_dump (job->output_stream->fp, file_job->delegation_tmpname);
	    unlink (file_job->delegation_tmpname);
	    a2ps_close_output_stream (job);
	    msg_job_pages_printed (job);
	  }
	else if (native_jobs || delegated_jobs)
	  {
	    /* The whole stuff is needed */
	    a2ps_close_output_session (job);
	    msg_job_pages_printed (job);
	  }
	else
	  {
	    /* Nothing has been printed.
	     * Don't close the job, so that nothing is sent to the printer,
	     * not even the PS prologue */
	    msg_nothing_printed ();
	  }
      }
      break;

    default:
      /* A case has not been recognized. */
      abort ();
    }

  a2ps_job_free (job);
  job = NULL;

  return (EXIT_SUCCESS);
}
