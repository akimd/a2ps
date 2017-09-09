/* prolog.h - routines for postscript prologue handling
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

#ifndef _PROLOG_H_
#define _PROLOG_H_

#define PORTRAIT_HEADER		20
#define LANDSCAPE_HEADER	15
#define PAGE_MARGIN 12		/* space between virtual pages		*/
#define HEADERS_H   12		/* Space for header/footer		*/

/* Space between lowest line in the text, and bottom of the frame */
#define SIDE_MARGIN_RATIO	0.7
#define BOTTOM_MARGIN_RATIO	0.7

struct a2ps_job;

typedef void (*a2ps_job_stream_hook) PARAMS ((struct a2ps_job * job,
					      FILE * stream));

/* A hook called when --debug, at the end of the ps comments */
extern a2ps_job_stream_hook ps_comment_hook;

void ps_begin PARAMS ((struct a2ps_job * job));
void ps_end PARAMS ((struct a2ps_job * job));

void ps_end_file PARAMS ((struct a2ps_job * job));

/*
 * List of the prologue
 */
void prologues_list_short PARAMS ((struct a2ps_job * job, FILE * stream));
void prologues_list_long PARAMS ((struct a2ps_job * job, FILE * stream));
void prologues_list_texinfo PARAMS ((struct a2ps_job * job, FILE * stream));
#endif /* !defined(_PROLOGUE_H) */
