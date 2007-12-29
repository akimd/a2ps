/* msg.h -- declaration for verbosity sensitive feedback function
   Copyright (c) 1988-1993 Miguel Santana
   Copyright (c) 1995-1999 Akim Demaille, Miguel Santana

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _MSG_H_
#define _MSG_H_

/* Definition of the verbosity levels. */

#define msg_null	(0)

/* Normal flow of messages
 * This one is painful, because we want it to be easy to understand as
 * -v0, -v1 and -v2 */
#define msg_report	(1 << 0)
#define msg_report2	(1 << 1)
#define msg_report1	(msg_report2 | msg_report)

/* Style sheets processing */
#define msg_sheet	(1 << 2)

/* Path walk function */
#define msg_pw		(1 << 3)

/* Encoding functions */
#define msg_enc		(1 << 4)

/* Options / Configuration */
#define msg_opt		(1 << 5)

/* Files related stuff */
#define msg_file	(1 << 6)

/* Tools (small routines) related stuff */
#define msg_tool	(1 << 7)

/* Font related stuff */
#define msg_font	(1 << 8)

/* PPD related */
#define msg_ppd		(1 << 9)

/* Meta sequences */
#define msg_meta	(1 << 10)

/* Any kind of parsers */
#define msg_parse	(1 << 11)


/* Report everything */
#define msg_all		0xFFFF


#endif /* not MSG_H_ */
