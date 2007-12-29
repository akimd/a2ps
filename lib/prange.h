/*
 * prange.h
 *
 * Page ranges
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 *
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

/*
 * $Id: prange.h,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */
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
