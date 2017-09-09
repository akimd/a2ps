/* versions.h - handling standard version numbers
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

#ifndef VERSIONS_H_
# define VERSIONS_H_

/* Beware that the routines are not actually ready for a meer change
   of VERSION_LENGTH.  Changes will be needed. */
#define VERSION_LENGTH 3
typedef int version_t[VERSION_LENGTH];

void version_set_to_null (version_t version);

bool version_null_p (version_t version);

int version_cmp (version_t v1, version_t v2);

void version_cpy (version_t d, version_t s);

void version_self_print (version_t version , FILE *stream);

/* Return the length occupied by this version number once printed */
int version_length (version_t version);

void version_add (version_t v1, version_t v2);

void string_to_version (const char *string, version_t version);

#endif /* not defined VERSIONS_H_ */
