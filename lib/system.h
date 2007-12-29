/*
 * system.h - shared system header with the whole package
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-2000 Akim Demaille, Miguel Santana
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

#ifndef SYSTEM_H_
# define SYSTEM_H_

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/*-------------------------.
| This guy must be first.  |
`-------------------------*/

#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
#  define HAVE_ALLOCA 1
# else
#  if defined HAVE_ALLOCA_H || defined _LIBC
#   include <alloca.h>
#  else
#   ifdef _AIX
 #pragma alloca
#   else
#    ifdef __hpux
void *alloca ();
#    endif /* __hpux */
#   endif
#  endif
# endif
#endif

#define ALLOCA(t, n) ((t *) alloca (sizeof (t) * (n)))

/*-------------------------------------.
| Support of prototyping when possible |
`-------------------------------------*/

#ifndef PARAMS
#  if PROTOTYPES
#    define PARAMS(protos) protos
#  else /* no PROTOTYPES */
#    define PARAMS(protos) ()
#  endif /* no PROTOTYPES */
#endif


/*-------------------------------.
| Stdio and missing prototypes.  |
`-------------------------------*/

#include <stdio.h>
#ifndef HAVE_DECL_FPUTS
extern int fputs PARAMS ((const char *s, FILE *stream));
#endif



/*-------------------.
| Including stdlib.  |
`-------------------*/

#if defined STDC_HEADERS || defined _LIBC || defined HAVE_STDLIB_H
# include <stdlib.h>
#else
extern char *getenv PARAMS ((const char *var));
#endif


/*--------------------.
| Including strings.  |
`--------------------*/

#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# include <strings.h>
char *memchr ();
#endif

#if !defined(HAVE_STRCHR) && !defined(strchr)
# define strchr index
# define strrchr rindex
#endif
extern char *strchr  PARAMS ((const char *s, int c));
extern char *strrchr PARAMS ((const char *s, int c));

#  ifndef HAVE_STRERROR
extern char *strerror PARAMS ((int));
#  endif

#define strequ(s1, s2)		(strcmp ((s1), (s2)) == 0)
#define strnequ(s1, s2, n)	(strncmp ((s1), (s2), (n)) == 0)

#ifndef HAVE_STRNDUP
extern char *strndup PARAMS ((char const *, size_t n));
#endif


/*---------------.
| Math headers.  |
`---------------*/

#ifdef HAVE_MATH_H
#  include <math.h>
#else
extern double atan2 PARAMS ((double, double));
extern double ceil  PARAMS ((double));
extern double floor PARAMS ((double));
#endif

#ifdef HAVE_ERRNO_H
#  include <errno.h>
#endif
#ifndef errno
extern int errno;
#endif


/*-------------------.
| Ctype and family.  |
`-------------------*/

#include <ctype.h>
/* Jim Meyering writes:

   "... Some ctype macros are valid only for character codes that
   isascii says are ASCII (SGI's IRIX-4.0.5 is one such system --when
   using /bin/cc or gcc but without giving an ansi option).  So, all
   ctype uses should be through macros like ISPRINT...  If
   STDC_HEADERS is defined, then autoconf has verified that the ctype
   macros don't need to be guarded with references to isascii. ...
   Defining isascii to 1 should let any compiler worth its salt
   eliminate the && through constant folding."  */

#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
#define ISASCII(c) 1
#else
#define ISASCII(c) isascii((int) c)
#endif

#ifdef isblank
#define ISBLANK(c) (ISASCII (c) && isblank ((int) c))
#else
#define ISBLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifdef isgraph
#define ISGRAPH(c) (ISASCII (c) && isgraph ((int) c))
#else
#define ISGRAPH(c) (ISASCII (c) && isprint ((int) c) && !isspace ((int) c))
#endif

#define ISPRINT(c) (ISASCII (c) && isprint   ((int) c))
#define ISDIGIT(c) (ISASCII (c) && isdigit   ((int) c))
#define ISALNUM(c) (ISASCII (c) && isalnum   ((int) c))
#define ISALPHA(c) (ISASCII (c) && isalpha   ((int) c))
#define ISCNTRL(c) (ISASCII (c) && iscntrl   ((int) c))
#define ISLOWER(c) (ISASCII (c) && islower   ((int) c))
#define ISPUNCT(c) (ISASCII (c) && ispunct   ((int) c))
#define ISSPACE(c) (ISASCII (c) && isspace   ((int) c))
#define ISUPPER(c) (ISASCII (c) && isupper   ((int) c))
#define ISXDIGIT(c) (ISASCII (c) && isxdigit ((int) c))

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif


/*---------------------------.
| Include unistd and fixes.  |
`---------------------------*/

#if HAVE_PWD_H
# include <pwd.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef _POSIX_VERSION
off_t lseek ();
#endif

#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif

#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef STAT_MACROS_BROKEN
# undef S_ISBLK
# undef S_ISCHR
# undef S_ISDIR
# undef S_ISFIFO
# undef S_ISLNK
# undef S_ISMPB
# undef S_ISMPC
# undef S_ISNWK
# undef S_ISREG
# undef S_ISSOCK
#endif /* STAT_MACROS_BROKEN.  */

#if !defined(S_ISBLK) && defined(S_IFBLK)
# define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif
#if !defined(S_ISCHR) && defined(S_IFCHR)
# define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#if !defined(S_ISDIR) && defined(S_IFDIR)
# define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
# define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISFIFO) && defined(S_IFIFO)
# define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#endif
#if !defined(S_ISLNK) && defined(S_IFLNK)
# define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK) && defined(S_IFSOCK)
# define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#if !defined(S_ISMPB) && defined(S_IFMPB) /* V7 */
# define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
# define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
#endif
#if !defined(S_ISNWK) && defined(S_IFNWK) /* HP/UX */
# define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
#endif

#ifndef S_IEXEC
# define S_IEXEC S_IXUSR
#endif

#ifndef S_IXUSR
# define S_IXUSR S_IEXEC
#endif
#ifndef S_IXGRP
# define S_IXGRP (S_IEXEC >> 3)
#endif
#ifndef S_IXOTH
# define S_IXOTH (S_IEXEC >> 6)
#endif
#ifndef S_IXUGO
# define S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#endif

#ifdef ST_MTIM_NSEC
# define ST_TIME_CMP_NS(a, b, ns) ((a).ns < (b).ns ? -1 : (a).ns > (b).ns)
#else
# define ST_TIME_CMP_NS(a, b, ns) 0
#endif
#define ST_TIME_CMP(a, b, s, ns) \
  ((a).s < (b).s ? -1 : (a).s > (b).s ? 1 : ST_TIME_CMP_NS(a, b, ns))
#define ATIME_CMP(a, b) ST_TIME_CMP (a, b, st_atime, st_atim.ST_MTIM_NSEC)
#define CTIME_CMP(a, b) ST_TIME_CMP (a, b, st_ctime, st_ctim.ST_MTIM_NSEC)
#define MTIME_CMP(a, b) ST_TIME_CMP (a, b, st_mtime, st_mtim.ST_MTIM_NSEC)

#if !defined(HAVE_MKFIFO)
# define mkfifo(path, mode) (mknod ((path), (mode) | S_IFIFO, 0))
#endif


/*---------------------------------------.
| Defining various limits on int types.  |
`---------------------------------------*/

#if HAVE_LIMITS_H
/* limits.h must come before pathmax.h because limits.h on some systems
   undefs PATH_MAX, whereas pathmax.h sets PATH_MAX.  */
# include <limits.h>
#endif

#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

/* The extra casts work around common compiler bugs,
   e.g. Cray C 5.0.3.0 when t == time_t.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))
#define TYPE_MINIMUM(t) ((t) (TYPE_SIGNED (t) \
                         ? ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1) : (t) 0))
#define TYPE_MAXIMUM(t) ((t) (~ (t) 0 - TYPE_MINIMUM (t)))

#ifndef CHAR_MIN
# define CHAR_MIN TYPE_MINIMUM (char)
#endif

#ifndef CHAR_MAX
# define CHAR_MAX TYPE_MAXIMUM (char)
#endif

#ifndef UCHAR_MAX
# define UCHAR_MAX TYPE_MAXIMUM (unsigned char)
#endif

#ifndef SHRT_MIN
# define SHRT_MIN TYPE_MINIMUM (short int)
#endif

#ifndef SHRT_MAX
# define SHRT_MAX TYPE_MAXIMUM (short int)
#endif

#ifndef INT_MAX
# define INT_MAX TYPE_MAXIMUM (int)
#endif

#ifndef UINT_MAX
# define UINT_MAX TYPE_MAXIMUM (unsigned int)
#endif

#ifndef LONG_MAX
# define LONG_MAX TYPE_MAXIMUM (long)
#endif

#ifndef ULONG_MAX
# define ULONG_MAX TYPE_MAXIMUM (unsigned long)
#endif


/*--------------------------------.
| Defining the PATHMAX some way.  |
`--------------------------------*/

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

#if !defined(PATH_MAX) && defined(_PC_PATH_MAX)
# define PATH_MAX (pathconf ("/", _PC_PATH_MAX) < 1 ? 1024 : pathconf ("/", _PC_PATH_MAX))
#endif

/* Don't include sys/param.h if it already has been.  */
#if defined(HAVE_SYS_PARAM_H) && !defined(PATH_MAX) && !defined(MAXPATHLEN)
# include <sys/param.h>
#endif

#if !defined(PATH_MAX) && defined(MAXPATHLEN)
# define PATH_MAX MAXPATHLEN
#endif

#ifndef PATH_MAX
# define PATH_MAX _POSIX_PATH_MAX
#endif


/*-----------------.
| Time and dates.  |
`-----------------*/

#if TIME_WITH_SYS_TIME
#  include <time.h>
#  include <sys/time.h>
#else
#  ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#if HAVE_STDBOOL_H
# include <stdbool.h>
#else
typedef enum {false = 0, true = 1} bool;
#endif


/*---------------------------.
| Take care of NLS matters.  |
`---------------------------*/

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#ifndef HAVE_SETLOCALE
# define setlocale(Category, Locale)
#endif

#include "gettext.h"
#define _(Msgid)  gettext (Msgid)
#define N_(Msgid) (Msgid)


/*----------.
| fnmatch.  |
`-----------*/

#if HAVE_FNMATCH_H
# include <fnmatch.h>
#endif

/*---------------------.
| Variadic arguments.  |
`---------------------*/

#if HAVE_VPRINTF || HAVE_DOPRNT || _LIBC
# if __STDC__
#  include <stdarg.h>
#  define VA_START(args, lastarg) va_start(args, lastarg)
# else
#  include <varargs.h>
#  define VA_START(args, lastarg) va_start(args)
# endif
#else
# define va_alist a1, a2, a3, a4, a5, a6, a7, a8
# define va_dcl char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
#endif

#include "xalloc.h"
#include "error.h"

/*
 * Missing functions
 */
#ifndef HAVE_TEMPNAM
# define tempnam(_dir_,_pfx_) xstrdup (tmpnam (NULL))
#endif

/*
 * More defines
 */
/* The following test is to work around the gross typo in
   systems like Sony NEWS-OS Release 4.0C, whereby EXIT_FAILURE
   is defined to 0, not 1.  */
#if !EXIT_FAILURE
# undef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif

#ifndef NULL
#  define NULL  0
#endif

/*-----------------.
| GCC facilities.  |
`-----------------*/

/* Specifying that a parameter of a function is not used. */

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#  define PARAM_UNUSED __attribute__ ((unused))
#else
#  define PARAM_UNUSED
#endif

/* Specifying printf like arguments for variadic functions. */

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

/* Cardinality of a static array. */

#define cardinalityof(ARRAY) (sizeof (ARRAY) / sizeof ((ARRAY)[0]))

#endif /* !defined (SYSTEM_H_) */
