/* output.h - routines for ram-diverted output
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

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

# ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__
#   define __attribute__(Spec) /* empty */
#  endif
/* The __-protected variants of `format' and `printf' attributes
   are accepted by gcc versions 2.6.4 (effectively 2.7) and later.  */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#   define __format__ format
#   define __printf__ printf
#  endif
# endif


struct output;
typedef void (*delayed_routine_t) (FILE * stream, void * arg);

/*
 * Maintenance
 */
struct output * output_new (const char * name);
void output_free (struct output * out);
void output_report  (struct output * out, FILE * stream);

# if defined (__STDC__) && __STDC__
extern void output (struct output * out, const char *format, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
# else
void output ();
# endif

void output_char (struct output * out, unsigned char c);
void output_delayed_int (struct output * out, int * ptr);
void output_delayed_string (struct output * out, unsigned char ** ptr);
void output_delayed_chunk (struct output * out, 
				   struct output * sub_out);

struct a2ps_job;
void output_file (struct output * out, 
			  struct a2ps_job * job, 
			  const char *name, const char *suffix);
void output_delayed_routine (struct output * out, 
				     delayed_routine_t fn,
				     void * fn_arg);

void output_to_void (struct output * out, int forget);
int output_is_to_void (struct output * out);

/*
 * Debug info
 */
void output_self_print (struct output * out, FILE * stream);

/*
 * Dumping the content somewhere
 */
void undivert (struct a2ps_job * job);
void output_dump (struct output * out, FILE * stream);
#endif

