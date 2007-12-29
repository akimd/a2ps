/*
 * sshread.h
 *
 * routines of input, and formatting according to the styles
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: sshread.h,v 1.1.1.1.2.1 2007/12/29 01:58:36 mhatta Exp $
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

#ifndef _SSHREAD_H_
#define _SSHREAD_H_

#include "ssheet.h"
#include "buffer.h"
struct a2ps_job;
/*
 * Pretty print the current file of JOB
 */
void ssh_print_postscript PARAMS ((struct a2ps_job * job, 
				   buffer_t * buffer,
				   struct style_sheet * sheet));

#endif /* not defined(_SHHREAD_H_) */
