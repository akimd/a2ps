/* prange.h - page ranges
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

#ifndef _PRANGE_H_
#define _PRANGE_H_

struct job;

/*
 * List of pages to print
 */
struct page_range;

struct page_range * page_range_new PARAMS ((void));
void page_range_free PARAMS ((struct page_range * array));

void a2ps_page_range_set_string PARAMS ((struct a2ps_job * job,
					 const char * string));

void report_pages_to_print PARAMS ((struct a2ps_job * job,
				    FILE * stream));

int print_page PARAMS ((struct a2ps_job * job,
			int page_num));

void page_range_to_buffer PARAMS ((struct page_range * page_range,
				   uchar * buf, int offset));
int page_range_applies_above PARAMS ((struct page_range * page_range,
				      int offset));

#endif
