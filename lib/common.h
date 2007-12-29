/*
 * common.h
 *
 * liba2ps shared memory (data used by several modules)
 *
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

/*
 * $Id: common.h,v 1.1.1.1.2.1 2007/12/29 01:58:16 mhatta Exp $
 */

#ifndef _COMMON_H_
#define _COMMON_H_
#include "a2ps.h"

/*
 * Shared memory for liba2ps: data that several modules
 * (encodings, printers etc.) will need.
 * This will actually disapear...
 */

struct a2ps_common_s
{
  char ** path;		/* Path to the files used by a2ps	*/
};

/* Default a shared mem values */
void a2ps_common_reset PARAMS ((struct a2ps_common_s * common));

/* Create a shared mem with default values */
struct a2ps_common_s * a2ps_common_new PARAMS ((void));

/* Free the common mem */
void a2ps_common_free PARAMS ((struct a2ps_common_s * common));

/* Finalize the common mem to the regular values HOME is the user's
 * home dir.  */
void a2ps_common_finalize PARAMS ((struct a2ps_common_s * common,
				   const char * home));
#endif /* !defined (_COMMON_H_) */
