/* getnum.h - get number in a given interval, get length with units
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

#ifndef GETNUM_H_
#define GETNUM_H_

enum range_type_e
{
  range_no_limit,
  range_min,
  range_min_strict,
  range_max,
  range_max_strict,
  range_min_max,
  range_min_strict_max,
  range_min_max_strict,
  range_min_strict_max_strict
};

int get_integer_in_range
	(const char * option, const char * arg,
		 int min, int max,
		 enum range_type_e range_type);

float get_length
	(const char * option, const char * arg,
		 float min, float max,
		 const char * unit, enum range_type_e range_type);
#endif /* !GETNUM_H_ */
