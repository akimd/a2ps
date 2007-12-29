/*
 * confg.h
 *
 * Read and understanding everything about the options 
 * & (dynamic) configuration of a2ps.
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
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

#ifndef _CONFG_H_
#define _CONFG_H_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

struct a2ps_job;

/*
 * Type of the functions that can be used as hooks when reading
 * the config files
 */
typedef void (* config_hook) PARAMS ((const char * filename, int line,
				      char * config_line));
extern config_hook delegation_hook;
extern config_hook toc_entry_hook;

/* liba2ps.h:begin */
/*
 * Read the configuration file
 */
int a2_read_config PARAMS ((struct a2ps_job * job,
			 const char *path, const char *file));

/*
 * Read the system's configuration file
 * (i.e., a2_read_config  (job, etc, a2ps.cfg))
 */
void a2_read_sys_config PARAMS ((struct a2ps_job * job));
/* liba2ps.h:end */
#endif
