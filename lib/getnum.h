/*
 * getnum.h -- Get number in a given interval, get length with units
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

#ifndef GETNUM_H_
#define GETNUM_H_

#ifndef PARAMS
# if PROTOTYPES || (defined (__STDC__) && __STDC__)
#  define PARAMS(args) args
# else
#  define PARAMS(args) ()
# endif  /* GCC.  */
#endif  /* Not PARAMS.  */

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
	PARAMS ((const char * option, const char * arg,
		 int min, int max,
		 enum range_type_e range_type));

float get_length
	PARAMS ((const char * option, const char * arg,
		 float min, float max,
		 const char * unit, enum range_type_e range_type));
#endif /* !GETNUM_H_ */
