/*
 * prolog.h
 * routines for the postscript prologue handling
 *
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
 * $Id: prolog.h,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */

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
