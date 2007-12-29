/*
 * routines.h -- general use routines
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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

#ifndef _ROUTINES_H_
# define _ROUTINES_H_

/*
 * From xgetcwd.c
 */
char *xgetcwd PARAMS ((void));

/*
 * from xgethostname.c
 */
char *xgethostname PARAMS ((void));

char *stpcpy PARAMS ((char * dest, const char * src));
char *stpncpy PARAMS ((char * dest, const char * src, size_t n));

/*
 * unsigned char variation of usual functions on strings
 */
#define ustrcat(x,y) 					\
	(strcat((char *)(x), (const char *)(y)))

#define ustrncat(x,y,n)	\
	(strncat(((char *)x), (const char *) y, n))

#define ustrcpy(x,y) 					\
	(strcpy((char *)(x), (const char *)(y)))
#define ustrncpy(x,y,z) 				\
	(strncpy((char *)(x), (const char *)(y), (z)))

#define ustpcpy(x,y)					\
	(stpcpy((char *)(x), (const char *)(y)))
#define ustpncpy(x,y,z)					\
	(stpncpy((char *)(x), (const char *)(y), (z)))

#define ustrcmp(x,y)					\
	(strcmp((const char *)(x), (const char *)(y)))
#define ustrncmp(x,y,z)					\
	(strncmp((const char *)(x), (const char *)(y), (z)))

#define ustrlen(x)					\
	(strlen((const char *)(x)))

#define ustrchr(x,y) 					\
	((uchar *) strchr((char *)(x), (int)(y)))
#define ustrrchr(x,y) 					\
	((uchar *) strrchr((char *)(x), (int)(y)))

#define xustrdup(x) 					\
	((uchar *) xstrdup((const char *)(x)))

#define ustrtok(x,y)					\
	((uchar *) strtok ((char *)(x), (const char *)(y)))

/*
 * Put in X a copy of chars in Y from Z to T
 */
#define ustrsub(x,y,z,t)				\
        ((uchar *) strsub ((char *)(x), (const char *)(y), (z), (t)));

/*
 * A string prefixes another
 */
#define strprefix(s1, s2)				\
	(!strncmp(s1, s2, strlen(s1)))
#define ustrprefix(s1, s2)				\
	(!ustrncmp(s1, s2, ustrlen(s1)))

/*
 * A string is the end of another
 *
 * Note that there are too many strlens.  But I know
 * no other way...
 */
#define strsuffix(s1, s2)				\
    ((strlen (s1) < strlen (s2))			\
     ? 0						\
     : !strcmp (s1 + strlen (s1) - strlen (s2), s2))

#define ustrsuffix(s1, s2)				\
    ((ustrlen (s1) < ustrlen (s2))			\
     ? 0						\
     : !ustrcmp (s1 + ustrlen (s1) - ustrlen (s2), s2))

/*
 * Replace a malloc'd string with another
 */
#define xstrcpy(s1, s2)					\
 do {				       			\
   const char *my_s2 = (s2);	       			\
   XFREE (s1);					       	\
   s1 = !IS_EMPTY (my_s2) ? xstrdup (my_s2) : NULL;	\
 } while (0)

/* We cannot just define this one like
    xstrcpy ((char *) s1, (const char *) s2);
   because it will expand into a line like
    (char *) s1 = ...
   and AIX 3.2's cc choke on this!!!  It says
    (S) Operand must be a modifiable lvalue. */

#define xustrcpy(s1, s2)				\
 do {				       			\
   const uchar *my_s2 = (uchar *) (s2);			\
   XFREE (s1);					       	\
   s1 = !IS_EMPTY (my_s2) ? xustrdup (my_s2) : UNULL;	\
 } while (0)

/*
 * Cut the _STRING_ a the first occurence of the _CHAR_ if there is
 */
#define strcut(_string_, _char_)		\
  do {						\
    char * __strcut_cp;				\
    __strcut_cp = strchr (_string_, _char_);	\
    if (__strcut_cp)				\
      *__strcut_cp = '\0';			\
  } while (0)

#define ustrcut(_ustring_, _uchar_)	\
  strcut ((char *) (_ustring_), (char) _uchar_)

/*
 * Cut the _STRING_ a the last occurence of the _CHAR_ if there is
 */
#define strrcut(_string_, _char_)		\
  do {						\
    char * __strrcut_cp;			\
    __strrcut_cp = strrchr (_string_, _char_);	\
    if (__strrcut_cp)				\
      *__strrcut_cp = '\0';			\
  } while (0)

#define ustrrcut(_ustring_, _uchar_)	\
  strrcut ((char *) (_ustring_), (char) _uchar_)

/*
 * alloca version of some str routines
 */
/*
  Here is a very interesting part of the GNU libc doc,
  which explains a problem I had...

   Do not use `alloca' inside the arguments of a function call--you
   will get unpredictable results, because the stack space for the
   `alloca' would appear on the stack in the middle of the space for the
   function arguments.  An example of what to avoid is `foo (x, alloca
   (4), y)'.
*/
#define astrcpy(_d_,_s_)				\
   do {							\
	const char * _tmp_ = (const char *) (_s_);	\
	_d_ = ALLOCA (char, strlen (_tmp_) + 1);	\
	strcpy (_d_, _tmp_);				\
   } while (0)

#define austrcpy(_d_,_s_)				\
   do {							\
	const uchar * _tmp_ = (const uchar *) (_s_);	\
	_d_ = ALLOCA (uchar, ustrlen (_tmp_) + 1);	\
	ustrcpy (_d_, _tmp_);				\
   } while (0)

#define strcat2(_d_,_s1_,_s2_)		\
 do {					\
   stpcpy (stpcpy (_d_, _s1_), _s2_);	\
 } while (0)

#define astrcat2(_d_,_s1_,_s2_)			\
 do {						\
   const char * _tmp1_ = (const char *) (_s1_);	\
   const char * _tmp2_ = (const char *) (_s2_);	\
   _d_ = ALLOCA (char, (strlen (_tmp1_) 	\
       		        + strlen (_tmp2_) + 1));\
   strcat2(_d_,_s1_,_s2_);			\
 } while (0)

/*
 * Concatenation of a char. No malloc is done.
 */
#define USTRCCAT(s, c)					\
  do { int __len = strlen((const char *)s);		\
       *(s+__len) = c;					\
       *(s+__len+1) = '\0'; 				\
  } while (false)


#define IS_BETWEEN(x, min, max) (((min) <= (x)) && ((x) <= (max)))

#define IS_EMPTY(s1)	\
	(((const char *) (s1) == NULL) || (*(s1) == '\0'))
#define UNNULL(s1)	\
 ((((void const *) s1) == NULL) ? (const char *) "" : (const char *) (s1))
#define ustrequ(s1, s2) (!ustrcmp(s1, s2))
#define strcaseequ(s1, s2) (!strcasecmp(s1, s2))

/* Functions on strings */
void string_to_array PARAMS ((uchar arr[256], const uchar * string));
int is_strlower PARAMS((const uchar * string));
uchar *strnlower PARAMS ((uchar * string, size_t len));
uchar *strlower PARAMS ((uchar * string));
uchar *strcpylc PARAMS ((uchar *dst, const uchar *src));

void ustrccat PARAMS((uchar * string, uchar c));
int strcnt PARAMS((uchar * s, uchar c));
char * strsub PARAMS ((char * dest,
		       const char * string, int start, int length));

/* Copy the content of IN into OUT */
void streams_copy PARAMS ((FILE * in, FILE * out));

/* Dump the content of a file onto STREAM */
void stream_dump PARAMS ((FILE * stream, const char * filename));
/* unlink FILENAME */
void unlink2 PARAMS ((void* dummy, const char * filename));

/* Fopen but exits on failure */
FILE * xfopen PARAMS ((const char * filename, const char * rights,
		       const char * format));
/* opened "r", and "w" */
FILE * xrfopen PARAMS ((const char * filename));
FILE * xwfopen PARAMS ((const char * filename));

/* Popen but exits on failure */
FILE * xpopen PARAMS ((const char * command, const char * rights,
		       const char * format));
/* opened "r", and "w" */
FILE * xrpopen PARAMS ((const char * command));
FILE * xwpopen PARAMS ((const char * command));

/* If _STR_ is not defined, give it a tempname in _TMPDIR_ */
#define tempname_ensure(Str)				\
do {							\
  (Str) = (Str) ? (Str) : tempnam (NULL, "a2_");	\
} while (0)

#endif
