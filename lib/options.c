/*
 * options.c
 *
 * Read and understanding everything about the options
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
 * $Id: options.c,v 1.1.1.1.2.2 2007/12/29 01:58:21 mhatta Exp $
 */


/************************************************************************/
/*									*/
/*			I n c l u d e   f i l e s			*/
/*                                                                      */
/************************************************************************/
#include <assert.h>

#include "a2ps.h"
#include "routines.h"
#include "options.h"
#include "argmatch.h"
#include "getopt.h"
#include "caret.h"
#include "message.h"
#include "jobs.h"
#include "useropt.h"
#include "prange.h"
#include "metaseq.h"
#include "madir.h"
#include "getnum.h"
#include "argv.h"
#include "quotearg.h"
#include "filalign.h"

#define MAN_LINES               66	/* no lines for a man */
extern char *program_name;
extern const char *program_invocation_name;

/*
 * Hooks used
 */
option_hook handle_option_hook = NULL;



static struct option const long_options[] =
{
  /* Tasks */
  {"help",		no_argument,		0, 'h'},
  {"guess",		no_argument,		0, 138},
  {"list", 		required_argument,	0, 145},
  {"version", 		no_argument,		0, 'V'},
  /* these ones are a courtesy for those who are used to enscript */
  {"list-options", 	no_argument,		0, 139},
  {"list-media", 	no_argument,		0, 161},
  {"list-style-sheets",	no_argument,		0, 162},
  {"help-languages",	no_argument,		0, 162},
  {"help-pretty-print",	no_argument,		0, 162},

  /* Global */
  {"macro-meta-sequence",required_argument,	0, 'D'},
  {"variable",		required_argument,	0, 'D'},
  {"define",		required_argument,	0, 'D'},
  {"user-option",	required_argument,	0, '='},

  /* Pretty Print */
  {"pretty-print",	optional_argument,	0, 'E'},

  /* PostScript */
  {"ppd",		optional_argument,	0, 163},
  {"prologue",		required_argument,	0, 134},
  {"include",		required_argument,	0, 134},
  {"sides", 		required_argument,	0, 's'},
  {"statusdict",	required_argument,	0, 164},
  {"setpagedevice",	required_argument,      0, 'S'},

  /* Not sorted yet */
  {"columns",		required_argument,	0, 132},
  {"rows",		required_argument,	0, 133},
  {"compact",		required_argument,      0, 'A'}, /* -A */
  {"file-align",	required_argument,      0, 'A'}, /* -A */
  {"header",		optional_argument,	0, 'b'},
  {"no-header", 	no_argument, 		0, 'B'},
  {"truncate-lines",	required_argument,	0, 151}, /* -c */
  {"line-numbers",	required_argument,      0, 152}, /* -C */

  {"font-size",		required_argument, 	0, 'f'},

  /* Obsolete, replaced by the following */
  {"graphic-symbols", 	required_argument, 	0, 154}, /* -g */
  {"highlight-level", 	required_argument, 	0, 173},

  {"interpret", 	required_argument,	0, 155}, /* -i */
  {"end-of-line", 	required_argument,	0, 169},
  {"borders", 		required_argument, 	0, 156}, /* -j */
  {"page-prefeed",	no_argument,		0, 'k'},
  {"no-page-prefeed",	no_argument,		0, 'K'},
  {"lines-per-page",	required_argument,	0, 'L'},
  {"chars-per-line",	required_argument,	0, 'l'},
  {"catman",		no_argument,		0, 'm'},
  {"medium", 		required_argument,	0, 'M'},
  /* Convenience for enscript users */
  {"media", 		required_argument,	0, 'M'},
  {"copies", 		required_argument,	0, 'n'},
  {"output", 		required_argument,	0, 'o'},
  {"printer", 		optional_argument,	0, 'P'},
  {"quiet", 		no_argument,		0, 'q'},
  {"silent",		no_argument,		0, 'q'},
  {"landscape", 	no_argument,		0, 'r'},
  {"portrait", 		no_argument,		0, 'R'},
  {"title", 		optional_argument,	0, 't'},
  {"tabsize", 		required_argument,	0, 'T'},
  {"underlay", 		required_argument,	0, 'u'},
  {"verbose", 		optional_argument,	0, 'v'},
  {"encoding", 		required_argument,	0, 'X'},


  {"non-printable-format",required_argument,	0, 135},
  {"print-anyway",	required_argument,	0, 136},

  {"center-title",	optional_argument,	0, 149},
  {"left-title",	optional_argument,	0, 140},
  {"right-title",	optional_argument,	0, 141},
  {"left-footer",	optional_argument,	0, 142},
  {"footer",		optional_argument,	0, 143},
  {"right-footer",	optional_argument,	0, 144},
  {"stdin",		required_argument,	0, 166},

  {"margin",		optional_argument,	0, 147},
  {"strip-level",	required_argument,	0, 148},
  {"major",		required_argument,	0, 157},
  {"version-control",	required_argument,	0, 158},
  {"suffix",		required_argument,	0, 159},

  {"debug",	 	no_argument,		0, 146},

  {"delegate",	 	required_argument,	0, 160},
  /* Courtesy for enscript */
  {"pass-through", 	required_argument,	0, 160},

  {"toc",		optional_argument,	0, 167},
  {"pages", 		optional_argument,	0, 'a'},

  {"which", 		no_argument,		0, 137},
  {"glob", 		no_argument,		0, 150},


  /* Free: 165, 168, 170, 171, 172 Next to use is: 174 */
  {NULL, 0, 0, 0}
};

#define OPT_STRING \
"123456789=:A:a:b::BcCdD:E::f:gGhijkKl:L:mM:n:o:P:qrRs:S:t::T:u::v::VX:Z"

/************************************************************************/
/*				arguments				*/
/************************************************************************/
/************************************************************************/
/* Helping routines for the options' arguments				*/
/************************************************************************/
/*
 * What about the bools
 */
static const char *const bool_args[] =
{
  "yes", "on", "1",
  "no", "off", "0",
  0
};

static const bool bool_types[] =
{
  true, true, true,
  false, false, false
};

/*
 * Return the bool value
 */
bool
a2ps_get_bool (const char *option, const char *arg)
{
  ARGMATCH_ASSERT (bool_args, bool_types);
  return XARGCASEMATCH (option, arg, bool_args, bool_types);
}


/* --non-printable-format */
static const char *const non_printable_args[] =
{
  "octal",
  "hexa",
  "emacs",
  "questionmark",
  "space", "white", "blank",
  "caret",
  0
};

static const enum unprintable_format non_printable_types[] =
{
  octal,
  hexa,
  Emacs,
  question_mark,
  space, space, space,
  caret
};

/* --sides: Duplex, Simplex, Tumble.
   I don't want `DuplexTumble' and `DuplexNoTumble' because it makes
   too many characters of ambiguity.  This way, the user can say `-sd'
   to mean `-sduplex'. */
static const char *const duplex_args[] =
{
  "simplex", "1", "none",
  "duplex", "2",
  "tumble",
  0
};

static const enum duplex_e duplex_types[] =
{
  simplex, simplex, simplex,
  duplex, duplex,
  tumble
};


/************************************************************************/
/*	Handle the options						*/
/************************************************************************/
/* There is a difficult task which is expand the user options.
   Because one cannot make recursive calls to getopt, because it is
   better not to try to patch getopt, we do it in two steps.

   First call getopt to expand the options in pairs (option, argument).

   Second process these pairs of (option, argument).

   Since during the second step we are out of getopt, it is then safe
   to call getopt again, so there are no special problem in the
   handling of user options, even if user options use user options.

   This simple algorithm was suggested by Tristan Gingold. */

/* A link list of (option, argument) */
struct opt_optarg
{
  int option;
  char *optarg;
  struct opt_optarg *next;
};

int
a2ps_handle_options (a2ps_job * job, int argc, char *argv[])
{
  struct opt_optarg *opt_optarg_head = NULL;
  struct opt_optarg *opt_optarg = NULL;
  int res;

  /* Reset optind so that getopt is reinitialized. */
  optind = 0;

  while (1)
    {
      /* getopt_long stores the index of the option here. */
      int option_index = 0;
      int option;
      struct opt_optarg *new;

      option = getopt_long (argc, argv,
			    OPT_STRING, long_options, &option_index);

      if (option == '?')
	{
	  /* Error message is done by getopt */
	  fprintf (stderr,
		   _("Try `%s --help' for more information.\n"),
		   program_invocation_name);
	  exit (EXIT_FAILURE);
	}

      /* Store the pair. */
      new = ALLOCA (struct opt_optarg, 1);
      new->option = option;
      new->optarg = optarg;
      new->next = NULL;
      if (!opt_optarg)
	{
	  opt_optarg_head = new;
	  opt_optarg = opt_optarg_head;
	}
      else
	{
	  opt_optarg->next = new;
	  opt_optarg = opt_optarg->next;
	}

      /* Exit at first non option. */
      if (option == EOF)
        break;
    }

  /* When processing the options, getopt can be called (e.g., user
     options), so the global var optind (which specifies the rank of
     the first non option in argv) *will* be changed.  We want to
     return its original value, not the one which can be defined here
     after. */
  res = optind;

  /* Now process the options. */
  for (opt_optarg = opt_optarg_head ;
       opt_optarg->option != EOF ;
       opt_optarg = opt_optarg->next)
    {
      optarg = opt_optarg->optarg;

      /*
       * First the option is passed to the application, in case
       * the application wants to redefine some options
       */
      if (handle_option_hook
	  && handle_option_hook (opt_optarg->option, optarg))
	/* The option has been handled, continue processing */
	continue;

      /* The option has not been recognized by the application,
       * let the lib handle it */
      switch (opt_optarg->option) {
#define define_nup(_col_,_row_,_orient_)	\
	job->columns = _col_;		\
	job->rows = _row_;		\
	job->orientation = _orient_;	\
	job->columns_requested = 80;	\
	job->lines_requested = 0;	\
	job->madir = madir_rows;

      case '1':				/* 1 logical page per sheet */
	define_nup (1, 1,  portrait);
	break;

      case '2':				/* twin pages */
	define_nup (2, 1,  landscape);
	break;

      case '3':				/* 3 virtual pages */
	define_nup (3, 1,  landscape);
	break;

      case '4':				/* 4 virtual pages */
	define_nup (2, 2,  portrait);
	break;

      case '5':				/* 5 virtual pages */
	define_nup (5, 1,  landscape);
	break;

      case '6':				/* 6 virtual pages */
	define_nup (3, 2,  landscape);
	break;

      case '7':				/* 7 virtual pages */
	define_nup (7, 1,  landscape);
	break;

      case '8':				/* 8 virtual pages */
	define_nup (4, 2,  landscape);
	break;

      case '9':				/* 9 virtual pages */
	define_nup (3, 3,  portrait);
	break;

      case '=':				/* A user option */
	a2ps_handle_string_options (job, user_option_get (job, optarg));
	break;

      case 'a':				/* --pages= pages to print */
	a2ps_page_range_set_string (job, optarg);
	break;

      case 'A':				/* --file-align=MODE. */
	job->file_align = file_align_argmatch ("--file-align", optarg);
	break;

      case 'b':				/* sheet header value */
	xustrcpy (job->header, optarg);
	break;

      case 'B':				/* No headers at all */
	/* Free them if they were allocated */
	XFREE (job->header);
	XFREE (job->left_footer);
	XFREE (job->footer);
	XFREE (job->right_footer);
	XFREE (job->left_title);
	XFREE (job->center_title);
	XFREE (job->right_title);
	XFREE (job->water);

	job->header = UNULL;
	job->left_footer = UNULL;
	job->footer = UNULL;
	job->right_footer = UNULL;
	job->left_title = UNULL;
	job->center_title = UNULL;
	job->right_title = UNULL;
	job->water = UNULL;
	break;

      case 'c':				/* cut lines too large */
	job->folding = false;
	break;

      case 151:				/* cut lines too large */
	job->folding = (a2ps_get_bool ("--truncate-lines", optarg)
			? false : true);
	break;

      case 'C':				/* line numbering */
	job->numbering = 5; /* Default is 5 by 5 */
	break;

      case 152:				/* --line-number[=INTERVAL] */
	/* Alex would like a default of 1. */
	job->numbering = (optarg
			  ? get_integer_in_range ("--line-numbers", optarg,
						  0, 0, range_min)
			  : 1);
	break;

      case 'd':				/* fork a process to print */
	a2ps_printers_flag_output_set (job->printers, NULL, true);
	break;

      case 'S': 			/* -S, --setpagedevice */
	{
	  char *value;
	  value = optarg + strcspn (optarg, " \t:=");
	  /* If `-SFeature:' or `-SFeature' then remove Feature,
	     otherwise (-SFeature:value) pass to libppd */
	  if (*value)
	    {
	      *value = '\0';
	      value ++;
	    }
	  if (*value)
	    setpagedevice (job, optarg, value);
	  else
	    delpagedevice (job, optarg);
	}
      break;

      case 'f':
	{
	  char * cp;
	  /* This is for compatibility with the previous scheme */
	  cp = strchr (optarg, '@');
	  if (cp)
	    cp ++;
	  else
	    cp = optarg;
	  /* A font size is given */
	  job->fontsize = get_length ("--font-size", cp,
				      0.0, 0.0, "pt", range_min_strict);
	  job->columns_requested = 0;
	  job->lines_requested = 0;
	}
      break;

      case 'i':				/* interpret control chars */
	job->interpret = true;
	break;

      case 155:				/* interpret control chars */
	job->interpret = a2ps_get_bool ("--interpret", optarg);
	break;

      case 'j':				/* surrounding border */
	job->border = true;
	break;

      case 156:				/* surrounding border */
	job->border = a2ps_get_bool ("--border", optarg);
	break;

      case 'k':				/* page prefeed */
	job->page_prefeed = true;
	break;

      case 'K':				/* no page prefeed */
	job->page_prefeed = false;
	break;

      case 'l':
	/* set columns per line, useful for most cases */
	job->columns_requested =
	  get_integer_in_range ("--chars-per-line", optarg, 1, 0, range_min);
	job->lines_requested = 0;
	break;

      case 'L':
	/* set lines per page.  Useful with preformatted files. Scaling is
	 * automatically done when necessary.  */
	job->lines_requested =
	  get_integer_in_range ("--lines-per-page", optarg, 1, 0, range_min);
	/* Unset value given to columns-per-page, so that this one
	 * is not hidden */
	job->columns_requested = 0;
	break;

      case 'm':				/* Process file as a man */
	job->lines_requested = MAN_LINES;
	job->columns_requested = 0;
	break;

      case 'M':                 		/* select a medium */
	xstrcpy (job->medium_request, optarg);
	break;

      case 'n':				/* n copies */
	job->copies =
	  get_integer_in_range ("--copies", optarg, 1, 0, range_min);
	break;

      case 'o':			/* output goes into a file */
	a2ps_printers_flag_output_set (job->printers, optarg, false);
	break;

      case 'P':					/* fork a process to print */
	a2ps_printers_flag_output_set (job->printers, optarg, true);
	break;

      case 'q':			       /* don't say anything but errors */
	/* Respect the envvar A2PS_VERBOSITY. */
	if (!getenv ("A2PS_VERBOSITY"))
	  msg_verbosity = msg_null;
	break;

      case 'r':
	job->orientation = landscape;  		/* landscape format */
	break;

      case 'R':
	job->orientation = portrait;  		/* portrait format */
	break;

      case 's':
	/* I'd like to have this test be run at compile time, but how? */
	ARGMATCH_ASSERT (duplex_args, duplex_types);
	job->duplex = XARGCASEMATCH ("--sides", optarg,
				     duplex_args, duplex_types);
	switch (job->duplex)
	  {
	  case simplex:
	    delpagedevice (job, "Duplex");
	    break;

	  case duplex:
	    setpagedevice (job, "Duplex", "true");
	    setpagedevice (job, "Tumble", "false");
	    break;

	  case tumble:
	    setpagedevice (job, "Duplex", "true");
	    setpagedevice (job, "Tumble", "true");
	    break;
	  }
	break;

      case 164:				/* statusdict definitions */
	{
	  char *value;
	  value = strchr (optarg, ':');
	  if (IS_EMPTY (value))
	    {
	      delstatusdict (job, optarg);
	    }
	  else
	    {
	      *value = '\0';
	      value ++;
	      if (*value == ':')
		setstatusdict (job, optarg, value + 1, true);
	      else
		setstatusdict (job, optarg, value, false);
	    }
	}
	break;

      case 't':				/* Job title		*/
	xustrcpy (job->title, optarg);
	break;

      case 'T':
	job->tabsize =
	  get_integer_in_range ("--tabsize", optarg, 1, 0, range_min);
	break;

      case 'u':				/* water mark (under lay) */
	xustrcpy (job->water, optarg);
	break;

      case 'v':					    /* verbosity */
	/* If the user specified the verbosity level by envvar,
	   don't override it. */
	if (!getenv ("A2PS_VERBOSITY"))
	    {
	      if (!IS_EMPTY(optarg))
		{
		  msg_verbosity =
		    msg_verbosity_argmatch ("--verbose", optarg);
		}
	      else
		msg_verbosity = msg_report1;
	    }
	break;

      case 'X': 			/* change the encoding scheme */
	/* Since there can be -X in the config files, and because
	 * the encoding.map has not been read yet (because to read
	 * encoding.map, one has to know the lib path, and to know
	 * the lib path, one has to read the config files...), we
	 * can't store the requested encoding as an encoding.
	 *
	 * Nevertheless, if encoding.map has been read. which means
	 * that this is actually a real command line option,
	 * do store the correct encoding */
	XFREE (job->requested_encoding_name);
	job->requested_encoding_name = xstrdup (optarg);
	break;

      case 132:				/* Number of columns */
	job->columns =
	  get_integer_in_range ("--columns", optarg, 1, 0, range_min);
	break;

      case 133:				/* Number of rows */
	job->rows =
	  get_integer_in_range ("--rows", optarg, 1, 0, range_min);
	break;

      case 134:				/* --include ps prologue */
	xstrcpy (job->prolog, optarg);
	break;

      case 135:				/* --non-printable-format */
	ARGMATCH_ASSERT (non_printable_args, non_printable_types);
	job->unprintable_format =
	  XARGCASEMATCH ("--non-printable", optarg,
			 non_printable_args, non_printable_types);
	break;

      case 136:				/* --print-anyway=bool */
	job->print_binaries = a2ps_get_bool ("--print-anyway", optarg);
	break;

      case 'D':				/* --define=key:value */
	{
	  char *value;
	  value = optarg + strcspn (optarg, " \t:=");
	  /* A value is given if there is a separator.  The value can
	     then be empty (`--var=foo=' i.e. foo = "") which is not
	     the same thing as --var=foo (i.e., undefine the var foo) */
	  if (*value)
	    *value++ = '\0';
	  else
	    value = NULL;
	  if (value)
	    {
	      if (!macro_meta_sequence_add (job, optarg, value))
		error (1, 0,
			       _("invalid variable identifier `%s'"),
			       quotearg (optarg));
	    }
	  else
	    {
	      macro_meta_sequence_delete (job, optarg);
	    }
	}
	break;

      case 140:
	xustrcpy (job->left_title, optarg);
	break;

      case 141:
	xustrcpy (job->right_title, optarg);
	break;

      case 149:
	xustrcpy (job->center_title, optarg);
	break;

      case 142:
	xustrcpy (job->left_footer, optarg);
	break;

      case 143:
	xustrcpy (job->footer, optarg);
	break;

      case 144:
	xustrcpy (job->right_footer, optarg);
	break;

      case 146:				/* --debug */
	job->debug = true;
	break;

      case 147:				/* --margin */
	if (optarg)
	  job->margin =
	    get_integer_in_range ("--margin", optarg, 0, 0, range_min);
	else
	  job->margin = 12;
	break;

      case 157: 			/* --major= */
	job->madir = madir_argmatch ("--major", optarg);
	break;

      case 158:				/* --version-control */
	job->backup_type = get_version ("--version-control", optarg);
	break;

      case 159:				/* --suffix	*/
	simple_backup_suffix = xstrdup (optarg);
	break;

      case 163:				/* --ppd[=FILE] */
	a2ps_printers_request_ppdkey_set (job->printers, optarg);
	break;

      case 166:			/* Set the name of file give by stdin */
	xustrcpy (job->stdin_filename, optarg);
	break;

      case '?':				/* Unknown option */
	/* Should never happen, since getopt was called in the previous
	   loop. */
	abort ();
	break;
      }
    }

  return res;
}

/* Process STRING as if it were given on the command line, i.e.  make
   it an argument vector, while respecting shell-escape and
   shell-grouping mechanics (" and '), and them run
   a2ps_handle_options onto the arguement vector.

   Return optind, i.e., the index of the the first non option argument.
   But I doubt arguments are allowed in user options.
*/
int
a2ps_handle_string_options (a2ps_job * job, const char * string)
{
  int argc;
  char **argv;
  int res;

  if (string == NULL)
    return 0;

  message (msg_opt, (stderr, "handle_string_options(%s)", string));

  /* Build the arguments vector, and add program name */
  argc = 1;
  argv = buildargv_argc (string, &argc);
  argv[0] = program_name;

  if (msg_test (msg_opt))
    {
      int i;
      for (i = 0; i < argc; i++)
        fprintf (stderr, "   %3d = `%s'\n", i, argv[i]);
    }

  /* Process options. */
  res = a2ps_handle_options (job, argc, argv);

  /* Cleanup. */
  freeargv_from (argv, 1);

  return res;
}
