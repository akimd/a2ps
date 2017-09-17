/* printlen.h - return number of chars used by a printf like call
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

#ifndef _PRINTLEN_H_
#define _PRINTLEN_H_

/*
 * Return length of the string sprintf would produce. Always > 0
 */
/*
 * Varaible arguments
 */
extern int printflen (const char *format, ...)
     __attribute__ ((__format__ (__printf__, 1, 2)));

/*
 * List of arguments.
 */
int vprintflen (const char *format, va_list args);


#endif /* ! defined(_PRINTLEN_H_) */
