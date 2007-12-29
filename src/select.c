/*
 * select.c -- Selection of a style sheet
 * Copyright (c) 1995-99 Akim Demaille, Miguel Santana
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
#include "select.h"
#include "routines.h"
#include "xfnmatch.h"
#include "getshline.h"
#include "path-concat.h"
#include "pathwalk.h"
#include "filtdir.h"
#include "message.h"
#include "quotearg.h"

extern char * style_request;

/*
 * Take priviledged access to job :)
 */
extern a2ps_job * job;
extern struct darray * sheets_map;


/*
 * The user has requested the style sheet him self.
 */
void
set_requested_style (const char * arg)
{
  xstrcpy (style_request, arg);
}

/************************************************************************/
/*			sheets.map handling				*/
/************************************************************************/
/*
 * Association of suffixes rules, and corresponding style sheet
 */
struct pattern_rule
{
  const char * pattern;
  const char * command;
  int on_file_verdict;
  bool insensitive_p;
};

struct pattern_rule *
pattern_rule_new (const char *pattern, int on_file_verdict, bool insensitive_p,
		  const char *command)
{
  NEW (struct pattern_rule, res);

  res->pattern = pattern;
  res->on_file_verdict = on_file_verdict;
  res->command = command;
  res->insensitive_p = insensitive_p;

  return res;
}

static void
pattern_rule_self_print (struct pattern_rule * item, FILE * stream)
{
  fprintf (stream, "%s/%s: %s/%s\n",
	   item->on_file_verdict ? "file" : "name",
	   item->pattern,
	   item->command,
	   item->insensitive_p ? "i" : "");
}

struct darray *
sheets_map_new (void)
{
  return da_new ("Sheets map", 200,
		 da_linear, 20,
		 (da_print_func_t) pattern_rule_self_print, NULL);
}

void
sheets_map_add (const char * pattern, int on_file_verdict, bool insensitive_p,
		const char * key)
{
  da_append (sheets_map,
	     pattern_rule_new (pattern, on_file_verdict, insensitive_p, key));
}


/*
 * Read the sheets.map file
 */
static int
sheets_map_load_main (void)
{
  char * file;

  /* System's */
  file = pw_find_file (job->common.path, "sheets.map", NULL);
  if (!file)
    {
      error (0, errno, _("cannot find file `%s'"), "sheets.map");
      /* sheets.map can not be found: there is no automatic prettyprinting */
      error (0, 0, _("automatic style selection cancelled"));
      return 0;
    }

  sheets_map_load (file);

  free (file);
  return 1;
}

/* escapes the name of a file so that the shell groks it in 'single' q.marks.
   The resulting pointer has to be free()ed when not longer used. */
char *
shell_escape(const char *fn)
{
  size_t len = 0;
  const char *inp;
  char *retval, *outp;
  
  for(inp = fn; *inp; ++inp)
    switch(*inp)
      {
      case '\'': len += 4; break;
      default:   len += 1; break;
      }
  
  outp = retval = malloc(len + 1);
  if(!outp)
    return NULL; /* perhaps one should do better error handling here */
  for(inp = fn; *inp; ++inp)
    switch(*inp)
    {
    case '\'': *outp++ = '\''; *outp++ = '\\'; *outp++ = '\'', *outp++ = '\''; break;
    default:   *outp++ = *inp; break;
    }
  *outp = 0;

  return retval;
}

/* What says file about the type of a file (result is malloc'd).  NULL
  if could not be run.  */

static char *
file_verdict_on (const uchar *filename)
{
  char *cp = NULL, * command;
  char buf [1024];
  FILE * file_out;

  if (IS_EMPTY (job->file_command))
    return NULL;

  filename = shell_escape(filename);
  if(filename == NULL)
    return NULL;
  /* Call file(1) with the correct option */
  command = ALLOCA (char, (4
			   + strlen (job->file_command)
			   + ustrlen (filename)));
  sprintf (command, "%s '%s'", job->file_command, (const char *) filename);
  free(filename);
  message (msg_tool, (stderr, "Reading pipe: `%s'\n", command));
  file_out = popen (command, "r");

  /* Check for failure */
  if (!file_out)
    {
      if (msg_test(msg_tool))
	error (0, errno, _("cannot open a pipe on `%s'"),
	       quotearg (command));
      return NULL;
    }

  /* Get the answer */
  fgets (buf, sizeof (buf), file_out);
  pclose (file_out);
  message (msg_tool, (stderr, "file(1): %s", buf));

  /* File is expected to answer:
     filename: file-answer. */
  cp = buf;
  while (*cp && *cp != ':')
    cp++;
  cp++;
  while (*cp && ((*cp == ' ') || (*cp == '\t')))
    cp++;

  if (*cp)
    {
      if (cp)
	message (msg_tool, (stderr, "File's verdict: %s", cp));
      /* Don't return the `\n'. */
      return xstrndup (cp, strlen (cp) - 1);
    }
  return NULL;
}

/*
 * Get style name from FILENAME, using pattern rules
 * and file(1) rules if USE_FILE.
 */
#define rule(_i_) ((struct pattern_rule *)sheets_map->content[_i_])
const char *
get_command (const uchar *name_to_match, const uchar *name_to_file)
{
  int i;
  char *file_verdict;
  uchar *name_to_match_lc;

  /* We only want to read the sheets map if needed, hence,
   * from here (not needed if the sheet name is given by the
   * user) */
  if (da_is_empty (sheets_map))
    sheets_map_load_main ();


  /* Get file(1)'s verdict, and get a lowercase version of the
     filename.  */
  file_verdict = file_verdict_on (name_to_file);
  if (name_to_match)
    {
      name_to_match_lc = ALLOCA (char, strlen (name_to_match) + 1);
      strcpylc (name_to_match_lc, name_to_match);
    }

  /* We look from bottom up, so that most recently defined rules are
   * honored.  Make sure not to call fnmatch on an empty FILE_VERDICT
   * (thanks to Michael Taeschner <Michael.Taeschner@dlr.de>,
   * Christian Mondrup <scancm@biobase.dk> and Jens Henrik Leonhard
   * Jensen recjhl@mediator.uni-c.dk) */
  /* The loop is split to speed up */
  for (i = sheets_map->len - 1 ; i >= 0 ; i--)
    if (rule(i)->on_file_verdict)
      {
	/* Testing upon file's result */
	if (file_verdict
	    && !fnmatch (rule(i)->pattern, file_verdict, 0))
	  {
	    free (file_verdict);
	    return rule(i)->command;
	  }
      }
    else
      {
	/* Upon file name */
	if (name_to_match
	    && !fnmatch (rule(i)->pattern,
			 (char *) (rule(i)->insensitive_p
				   ? name_to_match_lc : name_to_match),
			 0))
	  {
	    XFREE (file_verdict);
	    return rule(i)->command;
	  }
      }
  XFREE (file_verdict);

  return "plain";
}
