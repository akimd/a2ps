/*
 * main.h -- Common definitions for a2ps the program
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
 * $Id: main.h,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "system.h"
#include "darray.h"
#include "hashtab.h"
#include "message.h"
#include "quotearg.h"

#include "jobs.h"
#include "fjobs.h"
#include "generate.h"
#include "psgen.h"
#include "sshread.h"
#include "read.h"
#include "routines.h"
#include "select.h"
#include "delegate.h"
#include "gen.h"
#include "printers.h"
#include "metaseq.h"

/* Defined in main.c. */

extern struct hash_table_s * files;

/* --stdin=NAME name of the file given by stdin */
extern uchar * stdin_filename;

extern struct a2ps_job * job;

/* Shall we delegate */
extern bool delegate_p;

/* Highlighting model */
extern int highlight_level;

/* Requested style sheet */
extern char *style_request;

/* Type of eol. */
extern enum eol_e end_of_line;

/* Defined in generate.c. */

/* The file in which samples of files are put. */
extern char *sample_tmpname;
#endif /* !defined(_MAIN_H_) */
