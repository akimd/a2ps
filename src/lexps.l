%{ /* -*- c -*- */
/* A Simple Lexer for PostScript.

   Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000, 2002 Akim
   Demaille, Miguel Santana */

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

#include "main.h"
#include "lexps.h"

int yylex PARAMS ((void));
void yyerror PARAMS ((const char *));

/* Initilizes the obstacks */
void pslex_initialize PARAMS ((void));
%}

%option prefix="ps"
%option outfile="lex.yy.c"

%x FINISH

begin_setup	^%%BeginSetup.*\n
%%

{begin_setup}	{
  ECHO;
  pagedevice_dump (yyout, job);
  BEGIN FINISH;
}

.*	        ECHO;

<FINISH>{
  .*	        ECHO;
}
%%

int
yywrap (void)
{
  return 1;
}

/* Dump the file FILENAME on STREAM, while inserting the special
   pagedevice requests of JOB. */

void
pslex_dump (FILE *stream, const char *filename)
{
  yyin = xrfopen (filename);
  yyout = stream;

  pslex ();
}
