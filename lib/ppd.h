/*
 * ppd.h
 *
 * PostScript Printer Description files
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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

/*
 * $Id: ppd.h,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */
#ifndef PPD_H_
# define PPD_H_

struct a2ps_job;
struct string_htable;

int font_is_ps_known PARAMS ((struct a2ps_job * job, const char * name));

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
void ppd_font_add PARAMS ((struct ppd * pdd, const char * fontname));
int ppd_font_known_p PARAMS ((struct ppd * pdd, const char * fontname));

/*
 * The whole struct
 */
struct ppd * ppd_new PARAMS ((void));
void ppd_free PARAMS ((struct ppd * pdd));
struct ppd * _a2ps_ppd_get PARAMS ((char * const * path, const char * key));

/*
 * Report them
 */
void _a2ps_ppd_list_short PARAMS ((char * const * path, FILE * stream));
void _a2ps_ppd_list_long PARAMS ((char * const * path, FILE * stream));

/* From parsepdd.y */

struct ppd * a2ps_ppd_parse
   PARAMS ((const char * filename, char * const *path));

#endif /* !defined(PPD_H_) */
