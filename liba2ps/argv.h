/* argv.h - create and destroy argument vectors (argv's)
   Copyright 1992-2017 Free Software Foundation, Inc.

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

/* Written by Fred Fish @ Cygnus Support */

/*  Create and destroy argument vectors.  An argument vector is simply an
    array of string pointers, terminated by a NULL pointer. */

#ifndef _ARGC_H_
#define _ARGC_H_


/*

NAME

	dupargv -- duplicate an argument vector

SYNOPSIS

	char **dupargv (vector)
	char **vector;

DESCRIPTION

	Duplicate an argument vector.  Simply scans through the
	vector, duplicating each argument argument until the
	terminating NULL is found.

RETURNS

	Returns a pointer to the argument vector if
	successful. Returns NULL if there is insufficient memory to
	complete building the argument vector.

*/

char ** dupargv (char **argv);

/*

NAME

	freeargv -- free an argument vector

SYNOPSIS

	void freeargv (vector)
	char **vector;

DESCRIPTION

	Free an argument vector that was built using buildargv.  Simply scans
	through the vector, freeing the memory for each argument until the
	terminating NULL is found, and then frees the vector itself.

RETURNS

	No value.

*/

void freeargv (char **vector);
void freeargv_from (char **vector, int from);

/*

NAME

	buildargv -- build an argument vector from a string

SYNOPSIS

	char **buildargv (sp)
	char *sp;

DESCRIPTION

	Given a pointer to a string, parse the string extracting fields
	separated by whitespace and optionally enclosed within either single
	or double quotes (which are stripped off), and build a vector of
	pointers to copies of the string for each field.  The input string
	remains unchanged.

	All of the memory for the pointer array and copies of the string
	is obtained from malloc.  All of the memory can be returned to the
	system with the single function call freeargv, which takes the
	returned result of buildargv, as it's argument.

	The memory for the argv array is dynamically expanded as necessary.

RETURNS

	Returns a pointer to the argument vector if successful. Returns NULL
	if the input string pointer is NULL or if there is insufficient
	memory to complete building the argument vector.

NOTES

	In order to provide a working buffer for extracting arguments into,
	with appropriate stripping of quotes and translation of backslash
	sequences, we allocate a working buffer at least as long as the input
	string.  This ensures that we always have enough space in which to
	work, since the extracted arg is never larger than the input string.

	If the input is a null string (as opposed to a NULL pointer), then
	buildarg returns an argv that has one arg, a null string.

	Argv is always kept terminated with a NULL arg pointer, so it can
	be passed to freeargv at any time, or returned, as appropriate.
*/

char **buildargv_argc (const char *input, int * argc);
char **buildargv (const char *input);
#endif /* !defined (_ARGC_H_) */
