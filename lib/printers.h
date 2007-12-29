/*
 * printers.h - Information about the printers (named outputs)
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

#ifndef PRINTERS_H_
#define PRINTERS_H_
#include "a2ps.h"
#include "common.h"

/*

  The purpose of this file is to keep the information we have on the
  printers.  The central hash table gets its info from the reading of
  the configuration files (config.h mainly).  And its main results is
  to open the output stream, either on a file or a pipe.

*/

/*
 * The whole printers world
 * (There defs, the PPD handling etc)
 */
struct a2ps_job;
struct a2ps_printers_s;

/* Listing printers and PPD. */
void a2ps_printers_list_short PARAMS ((struct a2ps_job * job, FILE * stream));
void a2ps_printers_list_long PARAMS ((struct a2ps_job * job, FILE * stream));
void a2ps_ppd_list_short PARAMS ((struct a2ps_job * job, FILE * stream));
void a2ps_ppd_list_long PARAMS ((struct a2ps_job * job, FILE * stream));
/*
 * Output streams
 */
void a2ps_open_output_stream PARAMS ((struct a2ps_job * job));
void a2ps_close_output_stream PARAMS ((struct a2ps_job * job));

uchar *a2ps_flag_destination_to_string
    PARAMS ((struct a2ps_job * job));
uchar *a2ps_destination_to_string
    PARAMS ((struct a2ps_job * job));

/* Create the mem of the printers module */
struct a2ps_printers_s *a2ps_printers_new
   PARAMS ((struct a2ps_common_s * common));

/* Release the mem used by a PRINTERS module The module is freed */
void a2ps_printers_free PARAMS ((struct a2ps_printers_s * printers));

/* Finalize the printers module */
void a2ps_printers_finalize PARAMS ((struct a2ps_printers_s * printers));

/* Accessing some of the fields */
const char * a2ps_printers_default_ppdkey_get
   PARAMS ((struct a2ps_printers_s * printers));

void a2ps_printers_default_ppdkey_set
   PARAMS ((struct a2ps_printers_s * printers,
	      const char * key));

const char * a2ps_printers_request_ppdkey_get
   PARAMS ((struct a2ps_printers_s * printers));

void a2ps_printers_request_ppdkey_set
   PARAMS ((struct a2ps_printers_s * printers,
	    const char * key));

bool a2ps_printers_add
   PARAMS ((struct a2ps_printers_s * printers,
	    const char * key, char * definition));

int a2ps_printers_font_known_p
   PARAMS ((struct a2ps_printers_s * printers,
	    const char * name));


/*
 * Outputs
 */
void a2ps_printers_flag_output_set
    PARAMS ((struct a2ps_printers_s * printers,
	     const char * output_name, bool is_printer));
const char * a2ps_printers_flag_output_name_get
    PARAMS ((struct a2ps_printers_s * printers));
bool a2ps_printers_flag_output_is_printer_get
    PARAMS ((struct a2ps_printers_s * printers));

#endif /* !defined (PRINTERS_H_) */
