/*
 * psstat.h
 *
 * Recording information about the PostScript jobs
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
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
 * $Id: psstat.h,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */
#ifndef _PSSTAT_H_
#define _PSSTAT_H_

#include "encoding.h"
#include "output.h"
#include "confg.h"
#include "media.h"
#include "darray.h"
#include "gen.h"
#include "hashtab.h"
typedef struct hash_table_s hash_table;


/* Types of duplexing. */
enum duplex_e
{
  simplex, duplex, tumble
};

/*
 * Information that should be kept private to the postscript
 * generating engine of a2ps
 */
struct ps_status
{
  /* Set once at the beginning */
  unsigned int columnsperline;	/* Characters per output line 		*/
  unsigned long wxperline;	/* Points per output line		*/
  int linesperpage;		/* Lines per page 			*/
  float title_bar_height;	/* Height of the bar for v. pages title */
  int title_font_size;		/* Font size for the main title		*/
  uchar * magic_number;		/* The very first line of a PS file	*/
  uchar * page_label_format;	/* User string for %%Page: (here)	*/

  struct encoding * opened_encoding;/* Current encoding dict opened */

  /* Get enriched over the process */
  int page_are_ordered;		/* It might not be the case because of
				 * the delegations			*/
  hash_table * needed_resources;/* such as fonts etc.			*/
  hash_table * supplied_resources;/* such as fonts etc.			*/
  hash_table * colors;		/* such as fonts etc.			*/
  hash_table * pagedevice;	/* for PS setpagedevice			*/
  hash_table * statusdict;	/* For PS statusdict definitions	*/
  struct output * setup;	/* Files read may have sth to put in the
				 * set up part of the ps file		*/

  /* Changes all the time and need to be reset between files */
  uchar ** page_label;		/* %%Page (this part) 1			*/
  int start_page;
  int start_line;
  int line_continued;
  int is_in_cut;		/* Do we have to skip the incoming text	*/
  enum face_e face;			/* Current face				*/
  int face_declared;	/* Does the ps knows the current font?	*/
  int nonprinting_chars;
  int chars;			/* Number of nonprinting and total chars */
  int line;			/* Line number (in current page) 	*/
  unsigned int column;		/* Column number in chars 		*/
  unsigned long wx;		/* Column number in pts (in current line) */
};

/*
 * Dealing with the structure
 */
struct ps_status * new_ps_status PARAMS ((void));
void ps_status_free PARAMS ((struct ps_status * status));
void initialize_ps_status PARAMS ((struct ps_status * status));

/*
 * Dealing with its content
 */
/*
 * setpagedevice
 */
void output_pagedevice PARAMS ((struct a2ps_job * job));
void pagedevice_dump PARAMS ((FILE *stream, struct a2ps_job * job));
void dump_requirements PARAMS ((FILE * stream, struct a2ps_job * job));
void setpagedevice PARAMS ((struct a2ps_job * job,
			    const char * key, const char * value));
void delpagedevice PARAMS ((struct a2ps_job * job,
			    const char * key));
void list_pagedevice PARAMS ((struct a2ps_job * job, FILE * stream));

/*
 * statusdict
 */
void output_statusdict PARAMS ((struct a2ps_job * job));
void setstatusdict PARAMS ((struct a2ps_job * job,
			    const char * key, const char * value, int def));
void delstatusdict PARAMS ((struct a2ps_job * job,
			    const char * key));
void list_statusdict PARAMS ((struct a2ps_job * job, FILE * stream));

#endif
