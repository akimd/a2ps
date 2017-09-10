/* options.h - read and understanding everything about the options
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

#ifndef OPTIONS_H_
# define OPTIONS_H_

struct a2ps_job;

/*
 * Type of the functions for options hooks
 */
typedef int (* option_hook) (int opt, char * arg);

extern option_hook handle_option_hook;

/* Handle arguments */
bool a2ps_get_bool
  (const char * option, const char * arg);

/* Handle options.  Returns the indice of the first element of ARGV
   which is not an option. */

int a2ps_handle_options
   (struct a2ps_job * job, int argc, char *argv[]);

int a2ps_handle_string_options
   (struct a2ps_job * job, const char *string);

#endif /* !OPTIONS_H_ */
