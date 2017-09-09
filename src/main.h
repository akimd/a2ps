/* main.h - main loop, and interface with user
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
extern unsigned char * stdin_filename;

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
