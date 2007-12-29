/*
 * versions.h -- Handling standard version numbers
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 *
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

#ifndef VERSIONS_H_
# define VERSIONS_H_

/* Beware that the routines are not actually ready for a meer change
   of VERSION_LENGTH.  Changes will be needed. */
#define VERSION_LENGTH 3
typedef int version_t[VERSION_LENGTH];

void version_set_to_null PARAMS ((version_t version));

bool version_null_p PARAMS ((version_t version));

int version_cmp PARAMS ((version_t v1, version_t v2));

void version_cpy PARAMS ((version_t d, version_t s));

void version_self_print PARAMS ((version_t version , FILE *stream));

/* Return the length occupied by this version number once printed */
int version_length PARAMS ((version_t version));

void version_add PARAMS ((version_t v1, version_t v2));

void string_to_version PARAMS ((const char *string, version_t version));

#endif /* not defined VERSIONS_H_ */
