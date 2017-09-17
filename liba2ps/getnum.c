/* getnum.c - get number in a given interval, get length with units
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>   /* abort() is wanted */

#include "argmatch.h"
#include "getnum.h"
#include "quotearg.h"
#include "routines.h"
#include "error.h"

/************************************************************************
 *	Get a length/integer in an interval				*
 ************************************************************************/
int
get_integer_in_range (const char * option, const char * arg,
		      int min, int max, enum range_type_e range_type)
{
  char buf [256];
  int res;

  if ((sscanf (arg, "%d%255s", &res, buf) != 1))
    error (EXIT_FAILURE, 0, _("invalid argument `%s' for `%s'"),
	   quotearg (arg), option);

  buf [0] = '\0';	/* means no error */

  switch (range_type)
    {
    case range_min:
      if (res < min)
	sprintf (buf, "%d <= n", min);
      break;

    case range_min_strict:
      if (res <= min)
	sprintf (buf, "%d < n", min);
      break;

    case range_max:
      if (res > max)
	sprintf (buf, "n <= %d", max);
      break;

    case range_max_strict:
      if (res >= max)
	sprintf (buf, "n <= %d", max);
      break;

    case range_min_max:
      if ((res < min) || (res > max))
	sprintf (buf, "%d <= n <= %d", min, max);
      break;

    case range_min_strict_max:
      if ((res <= min) || (res > max))
	sprintf (buf, "%d < n <= %d", min, max);
      break;

    case range_min_max_strict:
      if ((res < min) || (res >= max))
	sprintf (buf, "%d <= n < %d", min, max);
      break;

    case range_min_strict_max_strict:
      if ((res < min) || (res >= max))
	sprintf (buf, "%d < n < %d", min, max);
      break;

    case range_no_limit:
      break;

    default:
      abort ();
      break;
    }

  if (buf [0])
    {
      error (0, 0,
	     _("invalid argument `%s' for `%s'"), quotearg (arg), option);
      fprintf (stderr,
	       _("Valid arguments are integers n such that: %s\n"), buf);
      exit (EXIT_FAILURE);
    }
  return res;
}

/*
 * Return a float in a given range, with a specified unit
 */
static float
get_float_in_range (const char * option, const char * arg,
		    const char * const * args_list,
		    float * types_list,
		    float min, float max,
		    const char * unit, enum range_type_e range_type)
{
  float res;
  char buf[256];

  switch (sscanf (arg, "%f%255s", &res, buf))
    {
    case 2:
      /* Multiply by the given unit */
      res *= XARGMATCH (option, buf, args_list, types_list);
      break;

    case 1:
      break;

    default:
      error (EXIT_FAILURE, 0, _("invalid argument `%s' for `%s'"),
	     quotearg (arg), option);
      break;
    }

  /* Divide by the desired unit */
  res /= types_list [__xargmatch_internal ("internal conversion", unit,
					   args_list,
					   (const char *) types_list,
					   sizeof (*types_list),
					   (argmatch_exit_fn) abort)];

  buf [0] = '\0';	/* means no error */

  /* Check that it is in the expected range */
  switch (range_type)
    {
    case range_min:
      if (res < min)
	sprintf (buf, "%.1f%s <= f", min, unit);
      break;

    case range_min_strict:
      if (res <= min)
	sprintf (buf, "%.1f%s < f", min, unit);
      break;

    case range_max:
      if (res > max)
	sprintf (buf, "f <= %.1f%s", max, unit);
      break;

    case range_max_strict:
      if (res >= max)
	sprintf (buf, "f <= %.1f%s", max, unit);
      break;

    case range_min_max:
      if ((res < min) || (res > max))
	sprintf (buf, "%.1f%s <= f <= %.1f%s", min, unit, max, unit);
      break;

    case range_min_strict_max:
      if ((res <= min) || (res > max))
	sprintf (buf, "%.1f%s < f <= %.1f%s", min, unit, max, unit);
      break;

    case range_min_max_strict:
      if ((res < min) || (res >= max))
	sprintf (buf, "%.1f%s <= f < %.1f%s", min, unit, max, unit);
      break;

    case range_min_strict_max_strict:
      if ((res <= min) || (res >= max))
	sprintf (buf, "%.1f%s < f < %.1f%s", min, unit, max, unit);
      break;

    case range_no_limit:
      break;

    default:
      abort ();
      break;
    }

  if (buf [0])
    {
      error (0, 0,
	     _("invalid argument `%s' for `%s'"), quotearg (arg), option);
      fprintf (stderr,
	       _("Valid arguments are floats f such that: %s\n"), buf);
      exit (EXIT_FAILURE);
    }

  return res;
}

/*
 * Return the ratio to inch
 */
static const char *const length_args[] =
{
  "points", "pts",
  "inchs",
  "cm", "centimeters",
  0
};

static float length_types[] =
{
  1.0, 1.0,
  72.0,
  (72 / 2.54), (72 / 2.54)
};

/*
 * Return a length in the desired unit
 */
float
get_length (const char * option, const char * arg, float min, float max,
	    const char * unit, enum range_type_e range_type)
{
  return get_float_in_range (option, arg, length_args, length_types,
			     min, max, unit, range_type);
}
