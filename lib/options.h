/*
 * options.h
 *
 * Read and understanding everything about the options
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

#ifndef OPTIONS_H_
# define OPTIONS_H_

struct a2ps_job;

/*
 * Type of the functions for options hooks
 */
typedef int (* option_hook) PARAMS ((int opt, char * arg));

extern option_hook handle_option_hook;

/* Handle arguments */
bool a2ps_get_bool
  PARAMS ((const char * option, const char * arg));

/* Handle options.  Returns the indice of the first element of ARGV
   which is not an option. */

int a2ps_handle_options
   PARAMS ((struct a2ps_job * job, int argc, char *argv[]));

int a2ps_handle_string_options
   PARAMS ((struct a2ps_job * job, const char *string));

#endif /* !OPTIONS_H_ */
