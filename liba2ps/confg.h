/* confg.h - read and understanding everything about the
             options and (dynamic) configuration of a2ps
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

#ifndef _CONFG_H_
#define _CONFG_H_

struct a2ps_job;

/*
 * Type of the functions that can be used as hooks when reading
 * the config files
 */
typedef void (* config_hook) (const char * filename, int line,
				      char * config_line);
extern config_hook delegation_hook;
extern config_hook toc_entry_hook;

/* liba2ps.h:begin */
/*
 * Read the configuration file
 */
int a2_read_config (struct a2ps_job * job,
			 const char *path, const char *file);

/*
 * Read the system's configuration file
 * (i.e., a2_read_config  (job, etc, a2ps.cfg))
 */
void a2_read_sys_config (struct a2ps_job * job);
/* liba2ps.h:end */
#endif
