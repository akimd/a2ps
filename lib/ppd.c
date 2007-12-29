/*
 * ppd.c
 *
 * PostScript Printer Description files
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
 * $Id: ppd.c,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
 */

#include "a2ps.h"
#include "ppd.h"
#include "jobs.h"
#include "routines.h"
#include "pathwalk.h"
#include "printers.h"
#include "darray.h"
#include "str_ht.h"
#include "title.h"
#include "lister.h"

#define PPD_SUFFIX ".ppd"

/************************************************************************/
/*	Handling the fonts						*/
/************************************************************************/
void
ppd_font_add (struct ppd * ppd, const char * fontname)
{
  string_htable_add (ppd->fonts, fontname);
}

int
ppd_font_known_p (struct ppd * ppd, const char * fontname)
{
  return (string_htable_get (ppd->fonts, fontname) != NULL);
}

/************************************************************************/
/*	Handling the PPD structure					*/
/************************************************************************/
struct ppd *
ppd_new (void)
{
  NEW (struct ppd, res);
  res->fonts = string_htable_new ();
  res->key = NULL;
  res->modelname = NULL;
  res->nickname = NULL;
  return res;
}

void
ppd_free (struct ppd * ppd)
{
  if (ppd) {
    string_htable_free (ppd->fonts);
    XFREE (ppd->key);
    XFREE (ppd->modelname);
    XFREE (ppd->nickname);
    free (ppd);
  }
}

/*
 * Produce a nice report of PPD
 */
static void
ppd_self_print (struct ppd * ppd, FILE * stream)
{
/* It must include the white spaces put before */
#define TAB_MAX	76
  char * cp;
  char ** fonts;

  if (ppd->nickname)
    cp = ppd->nickname;
  else if (ppd->modelname)
    cp = ppd->modelname;
  else
    cp = ppd->key;
  title (stream, '-', true, "%s (%s)", cp, ppd->key);

  /* Report the known fonts */
  fonts = string_htable_dump_sorted (ppd->fonts);
  fputs (_("Known Fonts"), stream);
  if (!*fonts)
    /* TRANS: This `none' is an answer to `List of known fonts: None' */
    fputs (_("\n  None.\n"), stream);
  else
    {
      putc ('\n', stream);
      lister_fprint_separated (NULL, stream,
			       (void **) fonts, (size_t) -1,
			       (lister_width_t) strlen,
			       (lister_print_t) fputs);
    }
  free (fonts);
}

/*
 * Look for the ppd file corresponding to KEY, in the PATH and return
 * its content
 */
struct ppd *
_a2ps_ppd_get (char * const * path, const char * key)
{
  struct ppd * res;
  char * ppd_filepath;

  ppd_filepath = xpw_find_file (path, key, ".ppd");
  res = a2ps_ppd_parse (ppd_filepath, path);
  res->key = xstrdup (key);
  free (ppd_filepath);
  return res;
}

/****************************************************************/
/* Reporting the known PPD	   				*/
/****************************************************************/
/*
 * List the PPD names
 */
void
_a2ps_ppd_list_short (char * const * path, FILE * stream)
{
  fputs (_("Known PostScript Printer Descriptions"), stream);
  putc ('\n', stream);
  pw_lister_on_suffix (stream, path, PPD_SUFFIX);
}

/*
 * List the PPD with their content
 */
void
_a2ps_ppd_list_long (char * const * path, FILE * stream)
{
  struct darray * entries;
  size_t i;

  entries = pw_glob_on_suffix (path, PPD_SUFFIX);

  title (stream, '=', true, _("Known PostScript Printer Descriptions"));
  putc ('\n', stream);

  for (i = 0 ; i < entries->len ; i++)
    {
      ppd_self_print (_a2ps_ppd_get (path, entries->content[i]), stream);
      putc ('\n', stream);
    }

  da_free (entries, (da_map_func_t) free);
}
