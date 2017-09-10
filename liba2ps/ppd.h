/* ppd.h - PostScript Printer Description files
   Copyright 1988-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef PPD_H_
# define PPD_H_

struct a2ps_job;
struct string_htable;

int font_is_ps_known (struct a2ps_job * job, const char * name);

struct ppd
{
  char * key;			/* Name of the file	*/
  char * modelname;
  char * nickname;
  struct string_htable * fonts;	/* List of the know fonts */
};

/*
 * The font part
 */
void ppd_font_add (struct ppd * pdd, const char * fontname);
int ppd_font_known_p (struct ppd * pdd, const char * fontname);

/*
 * The whole struct
 */
struct ppd * ppd_new (void);
void ppd_free (struct ppd * pdd);
struct ppd * _a2ps_ppd_get (char * const * path, const char * key);

/*
 * Report them
 */
void _a2ps_ppd_list_short (char * const * path, FILE * stream);
void _a2ps_ppd_list_long (char * const * path, FILE * stream);

/* From parsepdd.y */

struct ppd * a2ps_ppd_parse
   (const char * filename, char * const *path);

#endif /* !defined(PPD_H_) */
