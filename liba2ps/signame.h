/* signame.h - convert between signal names and numbers
   Copyright 1990-2017 Free Software Foundation, Inc.

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

/* Initialize `sys_siglist'.  */
void signame_init (void);

/* Return the abbreviation (e.g. ABRT, FPE, etc.) for signal NUMBER.
   Do not return this as a const char *.  The caller might want to
   assign it to a char *.  */
char *sig_abbrev (int number);

/* Return the signal number for an ABBREV, or -1 if there is no
   signal by that name.  */
int sig_number (const char *abbrev);

/* Avoid conflicts with a system header file that might define these three.  */

#ifndef HAVE_PSIGNAL
/* Print to standard error the name of SIGNAL, preceded by MESSAGE and
   a colon, and followed by a newline.  */
void psignal (int signal, const char *message);
#endif

#ifndef HAVE_STRSIGNAL
/* Return the name of SIGNAL.  */
char *strsignal (int signal);
#endif