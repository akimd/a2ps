/*
 * metaseq.c
 *
 * Handling of the meta sequences
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: metaseq.c,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
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
 * $Id: metaseq.c,v 1.1.1.1.2.1 2007/12/29 01:58:20 mhatta Exp $
 */

#include "a2ps.h"
#include "routines.h"
#include "jobs.h"
#include "fjobs.h"
#include "stpncpy.h"
#include "message.h"
#include "metaseq.h"
#include "xobstack.h"
#include "pair_ht.h"
#include "prange.h"
#include "title.h"

#define KEY_FORBIDDEN_CHARS ":(){}"

/************************************************************************/
/* Handling the macro meta sequences					*/
/************************************************************************/
/*
 * Creation
 */
struct pair_htable *
macro_meta_sequence_table_new (void)
{
  return pair_table_new ();
}

/*
 * Destruction
 */
void
macro_meta_sequence_table_free (struct pair_htable * table)
{
  pair_table_free (table);
}

/*
 * Check if is a valid name, and add.
 * (Note, strdup is done so that no memory is shared with key and value)
 *
 */
bool
macro_meta_sequence_add (struct a2ps_job * job,
			 const char * key, const char * value)
{
  if (strpbrk (key, KEY_FORBIDDEN_CHARS))
    return false;

  /* We want to remove any white space before the command */
  pair_add (job->macro_meta_sequences,
	    key, value + strspn (value, "\t "));
  return true;
}

void
macro_meta_sequence_delete (struct a2ps_job * job, const char * key)
{
  pair_delete (job->macro_meta_sequences, key);
}

char *
macro_meta_sequence_get (struct a2ps_job * job, const char * key)
{
  return pair_get (job->macro_meta_sequences, key);
}

void
macro_meta_sequences_list_short (struct a2ps_job * job, FILE * stream)
{
  /* TRANS: Variables (formely called `macro meta sequences', eeeaerk)
     are things such as #(psnup) which is substituted to a bigger strings,
     e.g. -#v #?q|-q|| #?j|-d|| #?r||-c| -w#w -h#h */
  fprintf (stream, _("Known Variables"));
  putc ('\n', stream);
  pair_table_list_short (job->macro_meta_sequences, stream);
}

void
macro_meta_sequences_list_long (struct a2ps_job * job,
				FILE * stream)
{
  title (stream, '=', true, _("Known Variables"));
  putc ('\n', stream);
  pair_table_list_long (job->macro_meta_sequences, stream);
}

/************************************************************************/
/* Expansion of a user string						*/
/************************************************************************/
/*
 * Help macros for expand_user_string ()
 */

#define APPEND_CH(ch)					\
  do {							\
    int a;						\
    if (width && justification < 0)			\
      obstack_1grow (user_string_stack, ch);		\
    for (a = 0; a < (int) width - 1; a++)		\
      obstack_1grow (user_string_stack, padding);	\
    if (!width || justification > 0)			\
      obstack_1grow (user_string_stack, ch);		\
  } while (0)

#define APPEND_STR(str)						\
  do {								\
    size_t len = ustrlen (str);					\
    size_t nspace;						\
								\
    nspace = (len > width) ? 0 : (width - len);			\
								\
    if (width && justification > 0)				\
      for (; nspace; nspace--)					\
        obstack_1grow (user_string_stack, padding);		\
								\
    obstack_grow (user_string_stack, str, len);			\
								\
    if (width && justification < 0)				\
      for (; nspace; nspace--)					\
        obstack_1grow (user_string_stack, padding);		\
  } while (0)

/*
 * We can't use strtok, that would skip the empty fields
 */
#define SPLIT(to,sep,esc,cat)						\
   do {									\
     to = next ;							\
     next = ustrchr (next, sep);					\
     if (!next)								\
       error (1, 0, _("%s: missing `%c' for %s%c escape"),		\
	      context_name, sep, esc, cat);				\
     *next++ = '\0';							\
   } while (0)

/*
 * An enumeration can be limited by the width.
 * If width = 0, no limit.
 * If width > 0, upper limit.
 * If width < 0, limit to n - width objects
 */
#define limit_by_width(_num_)		\
     (((width > 0)			\
       && (justification > 0)		\
       && (width < _num_)) 		\
      ? width				\
      : ((width > 0)			\
	  && (justification < 0)	\
	  && (width <= _num_) 		\
	 ? (_num_ - width)		\
	 : _num_))

#define fjob(_array_,_num_)	\
	((struct file_job *) _array_->content [_num_])


/*
 * Using the data in JOB, and in the current FILE data descriptor,
 * expand the possilbity escaped STR in the current USER_STRING_STACK.
 * Use CONTEXT_NAME as a mean to report more understandable error/logs.
 */
static void
grow_user_string_obstack (struct obstack * user_string_stack,
			  struct a2ps_job * job,
			  struct file_job * file,
			  const uchar * context_name,
			  const uchar * str)
{
  uchar * cp, * cp2;
  size_t i = 0, j;
  uchar padding = ' ' ;	/* Char used to complete %20 (usually ` ' or `.' */
  uchar buf[512], buf2[512], buf3[256];
  size_t width = 0;
  int justification = 1;

  /* Format string. */
  for (i = 0; str[i] != '\0'; i++)
    {
      int type;

      type = str[i];
      if (type == '%' || type == '$' || type == '#' || type == '\\') {
	i++;
	width = 0;
	justification = 1;
	padding = ' ';

	/* Get optional width and justification. */
	if (str[i] == '-') {
	  i++;
	  justification = -1;
	  if (!ISDIGIT ((int) str[i]))
	    padding = str[i++];
	}
	if (str[i] == '+') {
	  i++;
	  justification = 1;
	  if (!ISDIGIT ((int) str[i]))
	    padding = str[i++];
	}
	while (ISDIGIT ((int) str[i]))
	  width = width * 10 + str[i++] - '0';

	/* Handle escapes. */
	switch (type) {
	  /*
	   *		#
	   *		 #
	   *		  #
	   *		   #
	   *		    #
	   *		     #
	   *		      #
	   *
	   * Only escapes
	   */
	case '\\':
	  switch (str[i]) {
	  case 'f':	/* `\f' character \f */
	    APPEND_CH ('\f');
	    break;

	  case 'n':	/* `\n' character \n */
	    APPEND_CH ('\n');
	    break;

	  default:
	    APPEND_CH (str [i]);
	    break;
	  }
	  break;

	  /*
	   *		###   #
	   *		# #  #
	   *		### #
	   *		   #
	   *		  # ###
	   *		 #  # #
	   *		#   ###
	   *
	   * Related to the whole context
	   */
	case '%':
	  /* General state related %-escapes. */
	  switch (str[i]) {
	  case '%':	/* `%%' character `%' */
	    APPEND_CH ('%');
	    break;

	  case '#':	/* `%#': total number of files */
	    APPEND_CH (JOB_NB_FILES);
	    break;

	  case 'a':	/* `%a' NLS'ed `printed by USERNAME */
	    sprintf ((char *) buf2,
		     _("Printed by %s"),
		     macro_meta_sequence_get (job, VAR_USER_NAME));
	    APPEND_STR (buf2);
	    break;

	  case 'A':  /* `%A' NLS'ed `printed by USERNAME from MACHINE */
	    cp = macro_meta_sequence_get (job, VAR_USER_NAME);
	    cp2 = macro_meta_sequence_get (job, VAR_USER_HOST);
	    if (cp2)
	      sprintf ((char *) buf3,
		       _("Printed by %s from %s"), cp, cp2);
	    else
	      sprintf ((char *) buf3, _("Printed by %s"), cp);
	    APPEND_STR (buf3);
	    break;

	  case 'c':	/* `%c' trailing component of pwd. */
	    cp = (uchar *) xgetcwd ();
	    if (!cp)
	      error (1, errno,
		     _("cannot get current working directory"));
	    cp2 = ustrrchr (cp, DIRECTORY_SEPARATOR);
	    if (cp2)
	      cp2++;
	    else
	      cp2 = cp;
	    APPEND_STR (cp2);
	    XFREE (cp);
	    break;
	  case 'C':	/* `%C' runtime in `hh:mm:ss' format */
	    sprintf ((char *)buf, "%d:%02d:%02d", job->run_tm.tm_hour,
		     job->run_tm.tm_min, job->run_tm.tm_sec);
	    APPEND_STR (buf);
	    break;

	  case 'd':	/* `%d' current working directory */
	    cp = (uchar *) xgetcwd ();
	    if (!cp)
	      error (1, errno,
		     _("cannot get current working directory"));
	    APPEND_STR (cp);
	    XFREE (cp);
	    break;

	  case 'D':
	    if (str[i + 1] == '{')
	      {
		/* `%D{}' format run date with strftime() */
		for (j = 0, i += 2;
		     j < sizeof (buf2) && str[i] && str[i] != '}';
		     i++, j++)
		  buf2[j] = str[i];
		if (str[i] != '}')
		  error (1, 0, _("%s: too long argument for %s escape"),
			 context_name, "%D{}");

		buf2[j] = '\0';
		strftime ((char *) buf, sizeof (buf),
			  (char *) buf2, &job->run_tm);
	      }
	    else
	      {
		/* `%D' run date in `yy-mm-dd' format */
		sprintf ((char *)buf, "%02d-%02d-%02d",
			 job->run_tm.tm_year % 100,
			 job->run_tm.tm_mon + 1,
			 job->run_tm.tm_mday);
	      }
	    APPEND_STR (buf);
	    break;

	  case 'e':	/* `%e' run date in localized short format */
	    strftime ((char *) buf, sizeof (buf),
		      /* Translators: please make a short date format
		       * according to the std form in your language, using
		       * the standard strftime(3) */
		      (_("%b %d, %y")), &job->run_tm);
	    APPEND_STR (buf);
	    break;

	  case 'E':	/* `%E' run date in localized long format */
	    /* Translators: please make a long date format
	     * according to the std form in your language, using
	     * the standard strftime (3) */
	    strftime ((char *) buf, sizeof (buf),
		      (_("%A %B %d, %Y")), &job->run_tm);
	    APPEND_STR (buf);
	    break;

	  case 'F':	/* `%F' run date in `dd.mm.yyyy' format */
	    sprintf ((char *)buf, "%d.%d.%d",
		     job->run_tm.tm_mday,
		     job->run_tm.tm_mon + 1,
		     job->run_tm.tm_year+1900);
	    APPEND_STR (buf);
	    break;

	  case 'm':	/* `%m' the hostname up to the first `.' */
	    cp = macro_meta_sequence_get (job, VAR_USER_HOST);
	    cp2 = ALLOCA (uchar, strlen (cp) + 1);
	    strcpy (cp2, cp);
	    cp = ustrchr (cp2, '.');
	    if (cp)
	      *cp = '\0';
	    APPEND_STR (cp2);
	    break;

	  case 'M':	/* `%M' the full hostname */
	    APPEND_STR (macro_meta_sequence_get (job, VAR_USER_HOST));
	    break;

	  case 'n':	/* `%n' user's login */
	    APPEND_STR (macro_meta_sequence_get (job, VAR_USER_LOGIN));
	    break;

	  case 'N':	/* `%N' user's name */
	    APPEND_STR (macro_meta_sequence_get (job, VAR_USER_NAME));
	    break;

	  case 'p':	/* `%p' related to the pages of the job */
	    switch (str [++i]) {
	    case '.':	/* `%p.' current page number */
	      sprintf ((char *)buf, "%d", job->pages);
	      APPEND_STR (buf);
	      break;

	    case '#':	/* `%p#' total number of pages */
	      APPEND_CH (JOB_NB_PAGES);
	      break;

	    default:
	      error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		     context_name, "%p", str [i], str [i]);
	      break;
	    }
	    break;

	  case 'q':	/* `%q' localized `Page %d' */
	    sprintf ((char *)buf, _("Page %d"), job->pages);
	    APPEND_STR (buf);
	    break;

	  case 'Q':	/* `%Q' localized `Page %d/%c' */
	    sprintf ((char *)buf, _("Page %d/%c"),
		     job->pages, JOB_NB_PAGES);
	    APPEND_STR (buf);
	    break;

	  case 's':	/* `%s' related to the sheets of the job */
	    switch (str [++i]) {
	    case '.':	/* `%s.' current sheet number */
	      sprintf ((char *)buf, "%d", job->sheets);
	      APPEND_STR (buf);
	      break;

	    case '#':	/* `%s#' total number of sheets */
	      APPEND_CH (JOB_NB_SHEETS);
	      break;

	    default:
	      error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		     context_name, "%s", str [i], str [i]);
	      break;
	    }
	    break;

	  case 't':	/* `%t' runtime in 12-hour am/pm format */
	    sprintf ((char *)buf, "%d:%02d%s",
		     job->run_tm.tm_hour > 12
		     ? job->run_tm.tm_hour - 12 : job->run_tm.tm_hour,
		     job->run_tm.tm_min,
		     job->run_tm.tm_hour > 12 ? "pm" : "am");
	    APPEND_STR (buf);
	    break;

	  case 'T':	/* `%T' runtime in 24-hour format */
	    sprintf ((char *)buf, "%d:%02d",
		     job->run_tm.tm_hour, job->run_tm.tm_min);
	    APPEND_STR (buf);
	    break;

	  case '*':	/* `%*' runtime in 24-hour format with secs */
	    sprintf ((char *)buf, "%d:%02d:%02d",
		     job->run_tm.tm_hour,
		     job->run_tm.tm_min,
		     job->run_tm.tm_sec);
	    APPEND_STR (buf);
	    break;

	  case 'V':	/* `%V': name & version of this program */
	    sprintf ((char *) buf, "%s %s", PACKAGE, VERSION);
	    APPEND_STR (buf);
	    break;

	  case 'W':	/* `%W' run date in `mm/dd/yy' format */
	    sprintf ((char *)buf, "%02d/%02d/%02d",
		     job->run_tm.tm_mon + 1,
		     job->run_tm.tm_mday,
		     job->run_tm.tm_year % 100);
	    APPEND_STR (buf);
	    break;

	  default:
	    error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		   context_name, "%", str[i], str[i]);
	    break;

	  }
	  break;

	  /*
	   *		 #####
	   *		#  #  #
	   *		#  #
	   *		 #####
	   *		   #  #
	   *		#  #  #
	   *		 #####
	   *
	   * Related to the curent file
	   */
	case '$':
	  /* Input file related $-escapes. */
	  switch (str[i]) {
	  case '$':	/* `$$' character `$' */
	    APPEND_CH ('$');
	    break;

	  case '*':	/* `$*' modif time in 24-hour format with secs */
	    sprintf ((char *)buf, "%d:%02d:%02d",
		     file->mod_tm.tm_hour,
		     file->mod_tm.tm_min,
		     file->mod_tm.tm_sec);
	    APPEND_STR (buf);
	    break;

	  case '(':	/* $(ENVVAR) FIXME: Some day, remove in favor of ${} */
	    for (j = 0, i++;
		 str[i] && str[i] != ')' && j < sizeof (buf) - 1;
		 i++)
	      buf[j++] = str[i];

	    if (str[i] == '\0')
	      error (1, 0,  _("%s: missing `%c' for %s%c escape"),
		     context_name, ')', "$(", ')');
	    if (str[i] != ')')
	      error (1, 0, _("%s: too long argument for %s escape"),
		     context_name, "$()");
	    buf[j] = '\0';

	    cp = (uchar *) getenv ((char *)buf);
	    if (cp)
	      APPEND_STR (cp);
	    break;

	  case '{':	/* ${ENVVAR} or ${ENVVAR:-word} or  ${ENVVAR:+word} */
	    cp2 = UNULL;
	    for (j = 0 , i++ ; str[i] != '}' && j < sizeof (buf) - 1 ; i++)
	      switch (str [i]) {
	      case '\0':
		error (1, 0,  _("%s: missing `%c' for %s%c escape"),
		       context_name, '}', "${", '}');
		break;

	      case ':':		/* End of the name of the envvar,
				 * start getting the word */
		buf[j++] = '\0';
		cp2 = buf + j;
		break;

	      default:
		buf[j++] = str[i];
		break;
	      }
	    if (str[i] != '}')
	      error (1, 0, _("%s: too long argument for %s escape"),
		     context_name, "${}");
	    buf[j] = '\0';

	    /* Get the value of the env var */
	    cp = (uchar *) getenv ((char *)buf);
	    if (IS_EMPTY (cp2))
	      {
		/* No word specified */
		if (cp)
		  APPEND_STR (cp);
	      }
	    else
	      {
		/* A word is specified.  See how it should be used */
		switch (*cp2) {
		case '-':	/* if envvar defined then value else word */
		  if (!IS_EMPTY (cp))
		    APPEND_STR (cp);
		  else
		    APPEND_STR (cp2 + 1);
		  break;

		case '+':	/* if defined, then word */
		  if (!IS_EMPTY (cp))
		    APPEND_STR (cp2 + 1);
		  break;

		default:
		  error (1, 0,
			 _("%s: invalid separator `%s%c' for `%s' escape"),
			 context_name, ":", *cp2, "${}");
		}
	      }
	    break;

	  case '[':	/* `$[]' command line options */
	    if (!ISDIGIT ((int) str[i]))
	      error (1, 0,  _("%s: invalid argument for %s%c escape"),
		     context_name, "$[", ']');
	    {
	      size_t value = 0;
	      while (ISDIGIT ((int) str[i]))
		value = value * 10 + str[i++] - '0';
	      if (str[i] == '\0')
		error (1, 0,  _("%s: missing `%c' for %s%c escape"),
		       context_name, ']', "$[", ']');
	      if (str[i] != ']')
		error (1, 0,  _("%s: invalid argument for %s%c escape"),
		       context_name, "$[", ']');

	      if (value < job->argc)
		APPEND_STR (job->argv [value]);
	    }
	    break;

	  case '#':	/* `$#': input file number */
	    sprintf ((char *)buf, "%d", file->num);
	    APPEND_STR (buf);
	    break;

	  case 'C':	/* `$C' modtime in `hh:mm:ss' format */
	    sprintf ((char *)buf, "%d:%02d:%02d",
		     file->mod_tm.tm_hour,
		     file->mod_tm.tm_min,
		     file->mod_tm.tm_sec);
	    APPEND_STR (buf);
	    break;

	  case 'd':	/* `$d' directory part of the current file */
	    cp = ustrrchr (file->name, DIRECTORY_SEPARATOR);
	    if (cp) {
	      ustrncpy (buf, file->name, cp - file->name);
	      buf [cp - file->name] = '\0';
	      APPEND_STR (buf);
	    } else {
	      APPEND_CH ('.');
	    }
	    break;

	  case 'D':
	    if (str[i + 1] == '{')
	      {
		/* `$D{}' format modification date with strftime() */
		for (j = 0, i += 2;
		     j < sizeof (buf2) && str[i] && str[i] != '}';
		     i++, j++)
		  buf2[j] = str[i];
		if (str[i] != '}')
		  error (1, 0, _("%s: too long argument for %s escape"),
			 context_name, "$D{}");

		buf2[j] = '\0';
		strftime ((char *) buf, sizeof (buf),
			  (char *) buf2, &(file->mod_tm));
	      }
	    else
	      {
		/* `$D' mod date in `yy-mm-dd' format */
		sprintf ((char *)buf, "%02d-%02d-%02d",
			 file->mod_tm.tm_year % 100,
			 file->mod_tm.tm_mon + 1,
			 file->mod_tm.tm_mday);
	      }
	    APPEND_STR (buf);
	    break;

	  case 'e':	/* `$e' mod date in localized short format */
	    /* Translators: please make a short date format
	     * according to the std form in your language, using
	     * GNU strftime(3) */
	    strftime ((char *) buf, sizeof (buf),
		      (_("%b %d, %y")), &(file->mod_tm));
	    APPEND_STR (buf);
	    break;

	  case 'E':	/* `$E' mod date in localized long format */
	    strftime ((char *) buf, sizeof (buf),
		      /* Translators: please make a long date format
		       * according to the std form in your language, using
		       * GNU strftime(3) */
		      (_("%A %B %d, %Y")), &(file->mod_tm));
	    APPEND_STR (buf);
	    break;

	  case 'f':	/* `$f' full file name	*/
	    APPEND_STR (file->name);
	    break;

	  case 'F':	/* `$F' run date in `dd.mm.yyyy' format */
	    sprintf ((char *)buf, "%d.%d.%d",
		     file->mod_tm.tm_mday,
		     file->mod_tm.tm_mon + 1,
		     file->mod_tm.tm_year+1900);
	    APPEND_STR (buf);
	    break;

	  case 'l':	/* `$l' related to the lines of the file */
	    switch (str [++i]) {
	    case '^':	/* $l^ top most line in the current page */
	      sprintf ((char *)buf, "%d", file->top_line);
	      APPEND_STR (buf);
	      break;

	    case '.':	/* `$l.' current line */
	      sprintf ((char *)buf, "%d", file->lines - 1);
	      APPEND_STR (buf);
	      break;

	    case '#':		/* `$l#' number of lines in this file */
	      if (file != CURRENT_FILE (job)) {
		/* This file is finised, we do know its real number of lines */
		sprintf ((char *)buf, "%d", file->lines);
		APPEND_STR (buf);
	      } else {
		/* It is not know: delay it to the end of the job */
		APPEND_CH (FILE_NB_LINES);
	      }
	      break;

	    default:
	      error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		     context_name, "$l", str [i], str [i]);
	      break;
	    }
	    break;

	  case 'N': /* `$N' input file name without suffix nor
		       directory. */
	    /* First, skip dirname */
	    cp = ustrrchr (file->name, DIRECTORY_SEPARATOR);
	    if (cp == NULL)
	      cp =file->name;
	    else
	      cp ++;

	    /* Then, until the last dot */
	    cp2 = ustrrchr (cp, '.');
	    if (cp2) {
	      ustrncpy (buf, cp, cp2 - cp);
	      buf [cp2 - cp] = '\0';
	      APPEND_STR (buf);
	    } else
	      APPEND_STR (cp);
	    break;

	  case 'n':	/* `$n' input file name without directory */
	    cp = ustrrchr (file->name, DIRECTORY_SEPARATOR);
	    if (cp == NULL)
	      cp = file->name;
	    else
	      cp ++;
	    APPEND_STR (cp);
	    break;

	  case 'p':	/* `$p' related to the pages of the file */
	    switch (str [++i]) {
	    case '^':	/* `$p^' first page number of this file
			 * appearing in the current sheet */
	      sprintf ((char *)buf, "%d", file->top_page);
	      APPEND_STR (buf);
	      break;

	    case '-':	/* `$p-' interval of the pages of the current file
			 * appearing in the current sheet */
	      if (file->top_page == file->pages)
		sprintf ((char *)buf, "%d", file->top_page);
	      else
		sprintf ((char *)buf, "%d-%d", file->top_page, file->pages);
	      APPEND_STR (buf);
	      break;

	    case '<':	/* `$p<' first page number for this file */
	      sprintf ((char *)buf, "%d", file->first_page);
	      APPEND_STR (buf);
	      break;

	    case '.':	/* `$p.' current page number */
	      sprintf ((char *)buf, "%d", file->pages);
	      APPEND_STR (buf);
	      break;

	    case '>':	/* `$p>' last page number for this file */
	      if (file != CURRENT_FILE (job)) {
		/* This file is finised, we do know its last page */
		sprintf ((char *)buf, "%d", file->last_page);
		APPEND_STR (buf);
	      } else {
		/* It is not know: delay it to the end of the job */
		APPEND_CH (FILE_LAST_PAGE);
	      }
	      break;

	    case '#':	/* `$p#' total number of pages */
	      if (file != CURRENT_FILE (job)) {
		/* This file is finised, we do know its real number of pages */
		sprintf ((char *)buf, "%d", file->pages);
		APPEND_STR (buf);
	      } else {
		/* It is not know: delay it to the end of the job */
		APPEND_CH (FILE_NB_PAGES);
	      }
	      break;

	    default:
	      error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		     context_name, "$p", str [i], str [i]);
	      break;
	    }
	    break;

	  case 'q':	/* `$q' localized `Page $p' */
	    sprintf ((char *)buf, _("Page %d"), file->pages);
	    APPEND_STR (buf);
	    break;

	  case 'Q':	/* `$Q' localized `Page $p/$P' */
	    if (file != CURRENT_FILE (job))
	      /* This file is finised, we do know its real number of pages */
	      sprintf ((char *) buf, _("Page %d/%d"),
		       file->pages, file->pages);
	    else
	      /* It is not know: delay it to the end of the job */
	      sprintf ((char *) buf, _("Page %d/%c"),
		       file->pages,
		       FILE_NB_PAGES);
	    APPEND_STR (buf);
	    break;

	  case 's':	/* `$s' related to the sheets of the file */
	    switch (str [++i]) {
	    case '<':	/* `$s<' first sheet for this file */
	      sprintf ((char *)buf, "%d", file->first_sheet);
	      APPEND_STR (buf);
	      break;

	    case '.':	/* `$s.' current sheet number */
	      sprintf ((char *)buf, "%d", file->sheets);
	      APPEND_STR (buf);
	      break;

	    case '>':	/* `$s>' last sheet for this file */
	      if (file != CURRENT_FILE (job)) {
		/* This file is finised, we do know its last sheet */
		sprintf ((char *)buf, "%d", file->last_sheet);
		APPEND_STR (buf);
	      } else {
		/* It is not know: delay it to the end of the job */
		APPEND_CH (FILE_LAST_SHEET);
	      }
	      break;

	    case '#':	/* `$s#' total number of sheets */
	      if (file != CURRENT_FILE (job)) {
		/* This file is finised, we know its number of sheets */
		sprintf ((char *)buf, "%d", file->sheets);
		APPEND_STR (buf);
	      } else {
		/* It is not know: delay it to the end of the job */
		APPEND_CH (FILE_NB_SHEETS);
	      }
	      break;

	    default:
	      error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		     context_name, "$s", str [i], str [i]);
	      break;
	    }
	    break;

	  case 't':
	    switch (str[i + 1]) {
	    case '1':	/* `$t1' first marker grabbed from file */
	      i++;
	      APPEND_STR (job->tag1);
	      break;

	    case '2':	/* `$t2' second marker grabbed from file */
	      i++;
	      APPEND_STR (job->tag2);
	      break;

	    case '3':	/* `$t3' third marker grabbed from file */
	      i++;
	      APPEND_STR (job->tag3);
	      break;

	    case '4':	/* `$t4' fourth marker grabbed from file */
	      i++;
	      APPEND_STR (job->tag4);
	      break;

	    default:	/* `$t' runtime in 12-hour am/pm format */
	      sprintf ((char *)buf, "%d:%02d%s",
		       (file->mod_tm.tm_hour > 12
			?file->mod_tm.tm_hour-12
			:file->mod_tm.tm_hour),
		       file->mod_tm.tm_min,
		       file->mod_tm.tm_hour > 12 ? "pm" : "am");
	      APPEND_STR (buf);
	    }
	    break;

	  case 'T':	/* `$T' runtime in 24-hour format */
	    sprintf ((char *)buf, "%d:%02d",
		     file->mod_tm.tm_hour,
		     file->mod_tm.tm_min);
	    APPEND_STR (buf);
	    break;

	  case 'W':	/* `$W' run date in `mm/dd/yy' format */
	    sprintf ((char *)buf, "%02d/%02d/%02d",
		     file->mod_tm.tm_mon + 1,
		     file->mod_tm.tm_mday,
		     file->mod_tm.tm_year % 100);
	    APPEND_STR (buf);
	    break;

	  default:
	    error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		   context_name, "$", str[i], str[i]);
	    break;
	  }
	  break;

	  /*
	   *		  # #
	   *		  # #
	   *		#######
	   *		  # #
	   *		#######
	   *		  # #
	   *		  # #
	   */
	case '#':
	  switch (str[i]) {
	  case '#':	/* `##' character `#' */
	    APPEND_CH ('#');
	    break;

	  case '(':	/* #(macro meta sequence)  */
	    /* FIXME: Some day should disapear in favor of #{} */
	    for (j = 0, i++;
		 str[i] && str[i] != ')' && j < sizeof (buf) - 1;
		 i++)
	      buf[j++] = str[i];

	    if (str[i] == '\0')
	      error (1, 0, _("%s: missing `%c' for %s%c escape"),
		     context_name, ')', "#(", ')');
	    if (str[i] != ')')
	      error (1, 0, _("%s: too long argument for %s escape"),
		     context_name, "#()");
	    buf[j] = '\0';

	    cp = (uchar *) macro_meta_sequence_get (job,
						    (char *) buf);
	    if (cp)
	      grow_user_string_obstack (user_string_stack,
					job, file,
					context_name, cp);
	    break;


	  case '{':	/* #{macro} or #{macro:-word} or ${macro:+word} */
	    cp2 = UNULL;
	    for (j = 0 , i++ ; str[i] != '}' && j < sizeof (buf) - 1 ; i++)
	      switch (str [i]) {
	      case '\0':
		error (1, 0,  _("%s: missing `%c' for %s%c escape"),
		       context_name, '}', "#{", '}');
		break;

	      case ':':		/* End of the name of the macro,
				 * start getting the word */
		buf[j++] = '\0';
		cp2 = buf + j;
		break;

	      default:
		buf[j++] = str[i];
		break;
	      }
	    if (str[i] != '}')
	      error (1, 0, _("%s: too long argument for %s escape"),
		     context_name, "#{}");
	    buf[j] = '\0';

	    /* Get the value of the macro */
	    cp = (uchar *) macro_meta_sequence_get (job, (char *) buf);
	    if (IS_EMPTY (cp2))
	      {
		/* No word specified */
		if (cp)
		  grow_user_string_obstack (user_string_stack,
					    job, file,
					    context_name, cp);
	      }
	    else
	      {
		/* A word is specified.  See how it should be used */
		switch (*cp2) {
		case '-':	/* if macro defined value else word */
		  if (!IS_EMPTY (cp))
		    grow_user_string_obstack (user_string_stack,
					      job, file,
					      context_name, cp);
		  else
		    APPEND_STR (cp2 + 1);
		  break;

		case '+':	/* if macro defined, word */
		  if (!IS_EMPTY (cp))
		    APPEND_STR (cp2 + 1);
		  break;

		default:
		  error (1, 0,
			 _("%s: invalid separator `%s%c' for `%s' escape"),
			 context_name, ":", *cp2, "#{}");
		}
	      }
	    break;

	  case '.':	/* `#.' the usual extension for
			 * current output language */
	    APPEND_STR ("ps");
	    break;

	  case '?':	/* `#?' if-then meta sequence */
	    {
	      int test = 0;
	      uchar cond, sep;
	      uchar * if_true, * if_false;
	      uchar * next;

	      cond = str[++i];
	      sep = str[++i];
	      next = xustrdup(str + ++i);

	      SPLIT (if_true, sep, "#?", cond);
	      SPLIT (if_false, sep, "#?", cond);
	      i += next - if_true - 1;

	      switch (cond) {
	      case '1':	/* `#?1' Is the tag1 not empty? */
		test = !IS_EMPTY(job->tag1);
		break;
	      case '2':	/* `#?2' Is the tag2 not empty? */
		test = !IS_EMPTY(job->tag2);
		break;
	      case '3':	/* `#?3' Is the tag3 not empty? */
		test = !IS_EMPTY(job->tag3);
		break;
	      case '4':	/* `#?4' Is the tag4 not empty? */
		test = !IS_EMPTY(job->tag4);
		break;

	      case 'd': /* `#?d' Double sided printing		*/
		test = job->duplex == duplex || job->duplex == tumble;
		break;

  	      case 'j': /* `#?j' Bordering is asked (-j)		*/
 		test = job->border;
  		break;

 	      case 'l': /* `#?l' in landscape */
 		test = job->orientation == landscape;
 		break;

	      case 'o':	/* `#?o' Only one virtual page per page (-1) */
		test = ((job->rows * job->columns) == 1);
		break;

	      case 'p':	/* `#?p' A page range is specified		*/
		/* FIXME: May depend of other things (odd etc) */
		test = page_range_applies_above (job->page_range, job->pages);
		break;

	      case 'q':	/* `#?q' in quiet mode */
		test = msg_verbosity == 0;
		break;

	      case 'r': /* `#?r' madir = row		*/
		test = job->madir == madir_rows;
		break;

	      case 'V':	/* `#?V' verbose mode */
		test = msg_test (msg_tool);
		break;

	      case 'v': /* `#?v' on a verso side */
		test = job->sheets & 0x1;
		break;

	      default:
		error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		       context_name, "#?", cond, cond);
		break;
	      }
	      /*
	       * One might think there are problem in recursing
	       * grow_user_string_obstack, because of the static
	       * obstack.  It is true in general, but not
	       * for this precise case, where the obstack
	       * while keep on growing in the same
	       * direction
	       */
	      if (test)
		grow_user_string_obstack (user_string_stack,
					  job, file,
					  context_name, if_true);
	      else
		grow_user_string_obstack (user_string_stack,
					  job, file,
					  context_name, if_false);
	      free (if_true);
	    }
	    break;

	  case '!':	/* `#!' a enumeration of a category		*/
	    {
	      uchar category, sep;
	      uchar * in, * between;
	      uchar * next;

	      category = str[++i];
	      sep = str[++i];
	      next = xustrdup(str + ++i);

	      SPLIT (in, sep, "#!", category);
	      SPLIT (between, sep, "#!", category);
	      i += next - in - 1;

	      switch (category) {
	      case '$':		/* `#!$': enumeration of the arguments */
		{
		  size_t fnum, fmax;
		  fmax = limit_by_width (job->argc);
		  for (fnum = 0 ; fnum < fmax ; fnum++) {
		    APPEND_STR (job->argv [fnum]);
		    if (fnum < fmax - 1)
		      grow_user_string_obstack (user_string_stack,
						job,
						fjob(job->jobs, fnum),
						context_name, between);
		  }
		}
		break;

	      case 'f':		/* `#!f': enumeration of the input files */
		{
		  size_t fnum, fmax;
		  fmax = limit_by_width (job->jobs->len);
		  for (fnum = 0 ; fnum < fmax ; fnum++) {
		    grow_user_string_obstack (user_string_stack,
					      job,
					      fjob(job->jobs, fnum),
					      context_name, in);
		    if (fnum < fmax - 1)
		      grow_user_string_obstack (user_string_stack,
						job,
						fjob(job->jobs, fnum),
						context_name, between);
		  }
		}
		break;

	      case 'F':		/* `#!F': enumeration of the input files
				 * in alpha order */
		{
		  size_t fnum, fmax;
		  struct darray * ordered;

		  /* Make a ordered clone of the jobs array */
		  ordered = da_clone (job->jobs);
		  ordered->cmp = (da_cmp_func_t) file_name_cmp;
		  da_qsort (ordered);
		  fmax = limit_by_width (job->jobs->len);
		  for (fnum = 0 ; fnum < fmax ; fnum++) {
		    grow_user_string_obstack (user_string_stack,
					      job,
					      fjob (ordered, fnum),
					      context_name, in);
		    if (fnum < fmax - 1)
		      grow_user_string_obstack (user_string_stack,
						job,
						fjob (ordered, fnum),
						context_name, between);
		  }
		  da_erase (ordered);
		}
		break;

	      case 's':		/* `#!s': enumeration of the input files
				 * appearing in the current sheets */
		{
		  size_t fnum, fmax;
		  struct darray * selected;

		  /* Make a ordered clone of the jobs array */
		  selected = da_clone (job->jobs);

		  /* Make the selection:
		   * Only the files before this sheet are known,
		   * so just test on the last page number */
		  fnum = 0 ;
		  while (fnum < selected->len) {
		    if (fjob (selected, fnum)->last_sheet < job->sheets)
		      da_remove_at (selected, fnum, NULL);
		    else
		      fnum++;
		  }

		  fmax = limit_by_width (selected->len);
		  for (fnum = 0 ; fnum < fmax ; fnum++) {
		    grow_user_string_obstack (user_string_stack,
					      job,
					      fjob (selected, fnum),
					      context_name, in);
		    if (fnum < fmax - 1)
		      grow_user_string_obstack (user_string_stack,
						job,
						fjob (selected, fnum),
						context_name, between);
		  }
		  da_erase (selected);
		}
		break;

	      default:
		error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		       context_name, "#!", category, category);
		break;
	      }
	      free (in);
	    }
	    break;

	  case 'f':	/* `#f0' to `#f9': temporary file names */
	    {
	      int k = str [++i] - '0';
	      if (k < 0 || 9 < k)
		error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		       context_name, "#f", str [i], str [i]);
	      tempname_ensure (job->tmp_filenames [k]);
	      APPEND_STR (job->tmp_filenames [k]);
	    }
	    break;

	  case 'h':	/* `#h' medium height in PS points		*/
	    sprintf ((char *) buf, "%d", job->medium->h);
	    APPEND_STR (buf);
	    break;

	  case 'o':	/* `#o' name of destination, before evaluation */
	    APPEND_STR (a2ps_printers_flag_output_name_get (job->printers));
	    break;

	  case 'O':	/* `#O' name of destination, after evaluation */
	    if (a2ps_printers_flag_output_is_printer_get (job->printers))
	      grow_user_string_obstack
		(user_string_stack, job, file,
		 (const uchar *) _("output command"),
		 (const uchar *) a2ps_printers_flag_output_name_get(job->printers));
	    else
	      APPEND_STR (a2ps_printers_flag_output_name_get (job->printers));
	    break;

	  case 'p':	/* `#p' page range of what remains to be printed.
			 * E.g. with a2ps -a2,4-, then #p on page 3 gives 2- */
	    page_range_to_buffer (job->page_range, buf, job->pages);
	    APPEND_STR (buf);
	    break;

	  case 'v':	/* `#v' number of virtual pages */
	    sprintf ((char *) buf, "%d", job->rows * job->columns);
	    APPEND_STR (buf);
	    break;

	  case 'w':	/* `#w' medium width in PS points		*/
	    sprintf ((char *) buf, "%d", job->medium->w);
	    APPEND_STR (buf);
	    break;

	  default:
	    error (1, 0, _("%s: unknown `%s' escape `%c' (%d)"),
		   context_name, "#", str[i], str[i]);
	    break;
	  }
	  break;
	}
	/* Reset width so the else-arm goes ok at the next round. */
	width = 0;
	justification = 1;
      }
      else
	APPEND_CH (str[i]);
    }
}


/* The exported function.
   GIGO principle: if STR is NULL, output too.  */

uchar *
expand_user_string (struct a2ps_job * job,
		    struct file_job * file,
		    const uchar * context_name,
		    const uchar * str)
{
  static int first_time = 1;
  static struct obstack user_string_stack;

  uchar * res;

  if (first_time)
    {
      first_time = 0;
      obstack_init (&user_string_stack);
    }

  if (!str)
    return NULL;

  message (msg_meta,
	   (stderr, "Expanding of %s user string (`%s')\n",
	    context_name, str));

  grow_user_string_obstack (&user_string_stack,
			    job, file, context_name, str);

  obstack_1grow (&user_string_stack, '\0');
  res = (uchar *) obstack_finish (&user_string_stack);
  obstack_free (&user_string_stack, res);

  message (msg_meta,
	   (stderr, "Expansion of %s (`%s') is `%s'\n",
	    context_name, str, res));
  return res;
}
