/*
 * prange.c
 *
 * Page range
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98, 99 Akim Demaille, Miguel Santana
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
 * $Id: prange.c,v 1.1.1.1.2.1 2007/12/29 01:58:22 mhatta Exp $
 */
#include "a2ps.h"
#include "jobs.h"
#include "fjobs.h"
#include "prange.h"
#include "routines.h"
#include "quotearg.h"

/************************************************************************/
/*	Selecting the pages to print					*/
/*	1. The intervals 						*/
/************************************************************************/
/*
 * The cells used for the interval
 */
struct interval
{
  int min;
  int max;
};

static struct interval *
interval_new (int min, int max)
{
  struct interval * res = XMALLOC (struct interval, 1);
  res->min = min;
  res->max = max;
  return res;
}

static void
interval_self_print (struct interval * interval, FILE * stream)
{
  if (interval->min && interval->max)
    fprintf (stream, "%d-%d", interval->min, interval->max);
  else if (interval->min)
    fprintf (stream, "%d-", interval->min);
  else
    fprintf (stream, "-%d", interval->max);
}

static int
interval_is_in (struct interval * interval, int num)
{
  if (interval->min && interval->max)
    return (interval->min <= num && num <= interval->max);
  else if (interval->min)
    return (interval->min <= num);
  else
    return (num <= interval->max);
}

/*
 * Drop in BUF the written form of INTERVAL, taking the OFFSET
 * into account.
 * E.g. 3-5 with offset 4 is 1-2.
 */
static uchar *
interval_to_buffer (struct interval * interval, uchar * buf, int offset)
{
  int min = 0;

  /* This interval is passed */
  if (interval->max
      && interval->max < offset)
    return buf;

  /* The first pages are printed yet */
  if (interval->min
      && (interval->min <= offset))
    min = 1;
  else
    min = interval->min - offset;

  if (min == interval->max)
    sprintf ((char *) buf, "%d", min);
  else if (min && interval->max)
    sprintf ((char *) buf, "%d-%d", min, interval->max - offset);
  else if (min)
    sprintf ((char *) buf, "%d-", min);
  else
    /* It is better to specify the `1'.  For instance dvips
     * breaks on -pp-10 */
    sprintf ((char *) buf, "1-%d", interval->max - offset);

  return buf + ustrlen (buf);
}

/*
 * Does the interval says something about pages not to be printed
 * above OFFSET?
 *
 * 2-4 says something to 5: it should not be printed, a priori
 * 2- says nothing about restriction: 5 can be freely printed
 */
static inline int
interval_applies_above (struct interval * interval, int offset)
{
  if (interval->min <= offset && interval->max == 0)
    /* offset \in [min,-]: all should be printed */
    return false;
/*
  if (interval->min <= offset && offset <= interval->max)
    return false;
  */
  return true;
}

/************************************************************************/
/*	Selecting the pages to print					*/
/*	2. The page range						*/
/************************************************************************/
struct page_range {
  struct darray * intervals;	/* list of the intervals to print	*/
  bool toc;			/* if true, only the toc is printed	*/
};

/*
 * A darray specialized for page range
 */
struct page_range *
page_range_new (void)
{
  NEW (struct page_range, res);
  res->intervals = da_new ("Page Range Interval", 5, da_linear, 10,
			   (da_print_func_t) interval_self_print, NULL);
  res->toc = false;
  return res;
}

/*
 * Free the structure
 */
void
page_range_free (struct page_range * page_range)
{
  da_free (page_range->intervals, (da_map_func_t) free);
  free (page_range);
}

/*
 * Reset to a default value
 */
static void
page_range_reset (struct page_range * page_range)
{
  da_free_content (page_range->intervals, (da_map_func_t) free);
  page_range->toc = false;
}

/*
 * A darray specialized for page range
 */
static inline void
page_range_self_print (struct page_range * page_range, FILE * stream)
{
  da_self_print (page_range->intervals, stream);
}


/* job->print_page is an array of int, to 0 if the page
 *      should not be printed, 1 otherwise,
 * job->size_print_page stores the size of the array
 * Return 1 on success, 0 on error
 */
inline static int
add_pages_interval (struct a2ps_job * job, int min, int max)
{
  /* If max is null, it means `from MIN up to the end'.
   * If min > size, then set max to min,
   * otherwise, max := size
   */
  if (max && (max < min))
    return false;

  da_append (job->page_range->intervals, interval_new (min, max));
  return true;
}

/*
 * Drop in BUF the written form of PAGE_RANGE, taking the OFFSET
 * into account.
 * E.g. 3-5 with offset 4 is 1-2.
 */
void
page_range_to_buffer (struct page_range * page_range, uchar * buf, int offset)
{
  size_t i;
  int put_a_comma = false;
  struct interval ** intervals =
    (struct interval **) page_range->intervals->content;

  for (i = 0 ; i < page_range->intervals->len ; i++)
    if (interval_applies_above (intervals [i], offset))
      {
	if (put_a_comma)
	  *buf++ = ',';
	buf = interval_to_buffer (intervals [i], buf, offset);
	put_a_comma = true;
      }
}

/*
 * Does the page range has something to say above the offset?
 * For instance, 2,5,10-20 says something for OFFSET above 21
 * (that it must not be printed) and for any OFFSET less than 20.
 */
int
page_range_applies_above (struct page_range * page_range, int offset)
{
  size_t i;
  struct interval ** intervals =
    (struct interval **) page_range->intervals->content;

  if (page_range->intervals->len == 0)
    return false;

  for (i = 0 ; i < page_range->intervals->len ; i++)
    if (intervals [i]->min < offset
	&& intervals [i]->max == 0)
      /* offset \in [min,-]: all should be printed */
      return false;

  return true;
}

/************************************************************************/
/*	Selecting the pages to print					*/
/*	3. The interface with a2ps_job					*/
/************************************************************************/
/*
 * Called by the handling of options,
 */
void
a2ps_page_range_set_string (struct a2ps_job * job, const char * string)
{
  /* The format is -2, 4, 10-15, 20-
   * Print 1, 2, 4, 10 to 15, 20 to the end.
   */
  char * cp, * string_copy;
  int min, max;

  page_range_reset (job->page_range);
  if (!string)
    return;

  /* Avoid doing too many things at the same time with alloca */
  astrcpy (string_copy, string);
  cp = strtok (string_copy, ", \t");

  while (cp)
    {
      min = max = 0;
      if (isdigit ((int) *cp))
	{
	  min = atoi (cp);
	  while (isdigit ((int) *cp))
	    cp++;
	}
      switch (*cp++)
	{
	case '\0':
	  add_pages_interval (job, min, min);
	  break;

	case ':':
	case '-':
	  max = atoi (cp);
	  while (isdigit ((int) *cp))
	    cp++;
	  /* Make sure that the separator is respected */
	  if (*cp != '\0')
	    goto failed;
	  if (!add_pages_interval (job, min, max))
	    error (1, 0,
		   _("invalid interval `%s'"), quotearg (string));
	  break;

	case 't':
	  if (strprefix ("toc", cp - 1))
	    {
	      /* the toc has to be printed */
	      job->page_range->toc = true;
	      cp += 2;
	      /* Make sure that the separator is respected */
	      if (*cp != '\0')
		goto failed;
	    } else
	      goto failed;
	  break;

	failed:
	default:
	  error (1, 0, _("invalid interval `%s'"), quotearg (string));
	  break;
	}
      cp = strtok (NULL, ", \t");
    }
}

void
report_pages_to_print (struct a2ps_job * job, FILE * stream)
{
  page_range_self_print (job->page_range, stream);
}

/*
 * Return true if the page PAGE_NUM is to be printed
 */
int
print_page (struct a2ps_job * job, int page_num)
{
  size_t i;
  struct interval ** intervals =
    (struct interval **) job->page_range->intervals->content;

  /* If only the toc is to be printed, then take advantage
   * of having JOB to see if the current file is the toc.
   * It is not very clean, but it's really simple */
  if (job->page_range->toc && CURRENT_FILE (job)->is_toc)
    return true;

  /* If no page range where ever specified (not even toc), then
   * any page is to be printed */
  if (job->page_range->intervals->len == 0 && !job->page_range->toc)
    return true;

  for (i = 0 ; i < job->page_range->intervals->len  ; i++)
    if (interval_is_in (intervals [i], page_num))
      return true;

  return false;
}
