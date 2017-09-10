/* psstat.h - recording information about the PostScript process
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
  unsigned char * magic_number;		/* The very first line of a PS file	*/
  unsigned char * page_label_format;	/* User string for %%Page: (here)	*/

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
  unsigned char ** page_label;		/* %%Page (this part) 1			*/
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
struct ps_status * new_ps_status (void);
void ps_status_free (struct ps_status * status);
void initialize_ps_status (struct ps_status * status);

/*
 * Dealing with its content
 */
/*
 * setpagedevice
 */
void output_pagedevice (struct a2ps_job * job);
void pagedevice_dump (FILE *stream, struct a2ps_job * job);
void dump_requirements (FILE * stream, struct a2ps_job * job);
void setpagedevice (struct a2ps_job * job,
			    const char * key, const char * value);
void delpagedevice (struct a2ps_job * job,
			    const char * key);
void list_pagedevice (struct a2ps_job * job, FILE * stream);

/*
 * statusdict
 */
void output_statusdict (struct a2ps_job * job);
void setstatusdict (struct a2ps_job * job,
			    const char * key, const char * value, int def);
void delstatusdict (struct a2ps_job * job,
			    const char * key);
void list_statusdict (struct a2ps_job * job, FILE * stream);

#endif
