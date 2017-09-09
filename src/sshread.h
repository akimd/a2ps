/* sshread.h - routines of input, and formatting according to the styles
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

#ifndef _SSHREAD_H_
#define _SSHREAD_H_

#include "ssheet.h"
#include "buffer.h"
struct a2ps_job;
/*
 * Pretty print the current file of JOB
 */
void ssh_print_postscript (struct a2ps_job * job, 
				   buffer_t * buffer,
				   struct style_sheet * sheet);

#endif /* not defined(_SHHREAD_H_) */
