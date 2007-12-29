/* dstring.h - Dynamic string handling include file.  Requires strings.h.
   Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
   Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana

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

#ifndef DSTRING_H_
# define DSTRING_H_

/* Support of prototyping when possible */
#ifndef PARAMS
#  if PROTOTYPES
#    define PARAMS(protos) protos
#  else /* no PROTOTYPES */
#    define PARAMS(protos) ()
#  endif /* no PROTOTYPES */
#endif

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

extern int ds_exit_error;	/* exit value when encounters an error	*
				 * default is EXIT_FAILURE		*/


/* How the automatic resizing should work */
enum ds_growth {
  ds_steady,		/* Take care of it by yourself	*/
  ds_linear,		/* size += increment		*/
  ds_geometrical	/* size *= increment		*/
};

/* A dynamic string consists of record that records the size of an
   allocated string and the pointer to that string.  The actual string
   is a normal zero byte terminated string that can be used with the
   usual string functions.  The major difference is that the
   dynamic_string routines know how to get more space if it is needed
   by allocating new space and copying the current string.  */

struct dstring {
  size_t len;		/* Really used					*/
  enum ds_growth growth;/* See above					*/
  size_t increment;	/* See above					*/
  size_t size; 		/* Actual amount of storage allocated.		*/
  size_t original_size;	/* size given at creation			*/
  char *content;	/* String. 					*/
} ;


/* Macros that look similar to the original string functions.
   WARNING:  These macros work only on pointers to dynamic string records.
   If used with a real record, an "&" must be used to get the pointer.  */
#define ds_strcmp(s1, s2)	strcmp ((s1)->string, (s2)->string)
#define ds_strncmp(s1, s2, n)	strncmp ((s1)->string, (s2)->string, n)
#define ds_index(s, c)		index ((s)->string, c)
#define ds_rindex(s, c)		rindex ((s)->string, c)

/*
 * Maintaining
 */
struct dstring *
ds_new PARAMS ((size_t size,
		enum ds_growth growth, size_t increment));
void ds_erase PARAMS ((struct dstring *string));
void ds_print_stats PARAMS ((struct dstring * str, FILE * stream));
void ds_resize PARAMS ((struct dstring *string, size_t size));
void ds_grow PARAMS ((struct dstring *string));



/*
 * Testing
 */
int ds_is_full PARAMS ((struct dstring *string));



/*
 * Usual string manipulations
 */
void ds_strcat PARAMS ((struct dstring *s, char *t));
void ds_strncat PARAMS ((struct dstring *s, char *t, int n));
void ds_strccat PARAMS ((struct dstring *s, char c));

/*
 * Sprintf variations
 */
/* Sprintf in the dstring, resizing if necessary */
void ds_vsprintf PARAMS ((struct dstring *s, const char *format,
			  va_list args));
/* sprintf at the end of the dstring, resize if necessary */
void ds_cat_vsprintf PARAMS ((struct dstring *s, const char *format,
			      va_list args));
/* Same as the two previous, but much less care is taken to
 * make sure the dstring is big enough */
void ds_unsafe_vsprintf PARAMS ((struct dstring *s, const char *format,
				 va_list args));
void ds_unsafe_cat_vsprintf PARAMS ((struct dstring *s, const char *format,
				     va_list args));

/* Same as the previous, but with variable num of args */
# if defined (__STDC__) && __STDC__
extern void ds_sprintf (struct dstring *s, const char *format, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
/* Like sprintf, but not too much carre is taken for length */
extern void ds_unsafe_sprintf (struct dstring *s, const char *format, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
extern void ds_cat_sprintf (struct dstring *s, const char *format, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
/* Like sprintf, but not too much carre is taken for length */
extern void ds_unsafe_cat_sprintf (struct dstring *s, const char *format, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
# else
void ds_sprintf ();
void ds_unsafe_sprintf ();
void ds_cat_sprintf ();
void ds_unsafe_cat_sprintf ();
# endif

/*
 * Dealing with files
 */
char *ds_getline PARAMS ((struct dstring *s, FILE * stream));
char *ds_getdelim PARAMS ((struct dstring *s, char eos, FILE * stream));

#endif /* _DSTRING_H_ */
