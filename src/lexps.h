/*
 * A Simple Lexer for PostScript.
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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
 * $Id: lexps.h,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

#ifndef LEXPS_H_
# define LEXPS_H_

/* Dump the file FILENAME on STREAM, while inserting the special
   pagedevice requests of JOB. */

void pslex_dump PARAMS ((FILE *stream, const char *filename));

#endif
