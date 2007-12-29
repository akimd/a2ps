/*
 * faces.h
 *
 * definition of the faces used by a2ps
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: faces.c,v 1.1.1.1.2.1 2007/12/29 01:58:17 mhatta Exp $
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
#include "faces.h"
#include "routines.h"
#include "jobs.h"

extern const char * program_name;

/* List of the faces that must be checked */
enum face_e base_faces [] = {
  Plain,
  Keyword,
  Keyword_strong,
  Label,
  Label_strong,
  String,
  Symbol,
  Error,
  Comment,
  Comment_strong,
  No_face
};

struct face_and_name {
  const char * name;
  enum face_e face;
};

static const struct face_and_name face_and_name [] = {
  { "NoFace", No_face },
  { "Plain", Plain },
  { "Keyword", Keyword },
  { "Keyword_strong", Keyword_strong },
  { "Label", Label },
  { "Label_strong", Label_strong },
  { "String", String },
  { "Symbol", Symbol },
  { "Error", Error },
  { "Comment", Comment },
  { "Comment_strong", Comment_strong },
  { NULL, No_face }
};

enum face_e
string_to_face (const char * string)
{
  int i;

  for (i = 0 ; face_and_name[i].name ; i++)
    if (strequ (string, face_and_name[i].name))
      return face_and_name[i].face;
  
  return No_face;
}

static inline const char *
_face_to_string (enum face_e face)
{
  int i;
  
  for (i = 0 ; face_and_name[i].name ; i++)
    if (face == face_and_name[i].face)
      return face_and_name[i].name;
  
  return "UnknownFace";
}

const char *
face_to_string (enum face_e face)
{
  return _face_to_string (face);
}

/*
 * Report the face
 */
void
face_self_print (enum face_e face, FILE * stream)
{
  fputs (_face_to_string (face), stream);
}

/* convertion from a font to the postscript function */
const char *
face_eo_ps (enum face_e face)
{
  switch (face) {
  case Plain:
    return "p";
  case Symbol:
    return "sy";
  case Keyword:
    return "k";
  case Keyword_strong:
    return "K";
  case Comment:
    return "c";
  case Comment_strong:
    return "C";
  case Label:
    return "l";
  case Label_strong:
    return "L";
  case String:
    return "str";
  case Error:
    return "e";
  default:
    error (1, 0, "face_eo_ps: %d", face);
  }
  return 0; /* For -Wall */
}

/* Check that the face_eo_font knowldge is complete */
void
check_face_eo_font (struct a2ps_job * job)
{
  int i;
  int err = false;

  for (i = 0 ; base_faces [i] != No_face ; i++)
    if (!job->face_eo_font [base_faces [i]]) {
      err = true;
      fprintf (stderr, "%s: face `", program_name);
      face_self_print (base_faces [i], stderr);
      fprintf (stderr, "' (%d) has no specified font\n", base_faces [i]);
    }
  if (err)
    error (1, 0, _("incomplete knowledge of faces"));
}

/*
 * Return true if the faces are known, false otherwise
 */
int
face_eo_font_is_set (struct a2ps_job * job)
{
  enum face_e f;
  int res = true;

  for (f = First_face ; f <= Last_face ; f++)
    if (!job->face_eo_font [f])
      res = false;

  return res;
}

/* Initialize the face_font_to_font array to non acceptable values */
void
init_face_eo_font (struct a2ps_job * job)
{
  enum face_e f;
  for (f = First_face ; f <= Last_face ; f++)
    job->face_eo_font [f] = NULL;
}

void
face_eo_font_free (char * face_eo_font [NB_FACES])
{
  enum face_e f;
  for (f = First_face ; f <= Last_face ; f++)
    /* Xfree because it may not have been initialized if no printing
     * was done (e.g. --help) */
    XFREE (face_eo_font [f]);
}

void
face_set_font (struct a2ps_job * job, enum face_e face, const char * font_name)
{
  XFREE (job->face_eo_font [face]);
  job->face_eo_font [face] = xstrdup (font_name);
}
