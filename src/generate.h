/* generate.h - input files and pretty printing
   Copyright 1995-2017 Free Software Foundation, Inc.

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

#ifndef GENERATE_H_
# define GENERATE_H_

void print (unsigned char * name, int * native_jobs, int * delegated_jobs);
void print_toc (const unsigned char * name, const unsigned char * value,
		       int * native_jobs);

void msg_job_pages_printed (a2ps_job * job);
void msg_nothing_printed (void);

void guess (unsigned char * name);

#endif /* not defined(GENERATE_H_) */
