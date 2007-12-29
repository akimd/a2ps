%{ /* -*- c -*- */
/*
 * Grammar for parsing the style sheets
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
 *
 */

/*
 * This file is part of a2ps
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
 * $Id: parseppd.y,v 1.1.1.1.2.1 2007/12/29 01:58:21 mhatta Exp $
 */

#include "a2ps.h"

/* We need to use the same `const' as bison, to avoid the following
   prototypes to diverge from the function declarations */
#undef const
#ifndef __cplusplus
# ifndef __STDC__
#   define const
# endif
#endif

#include "jobs.h"
#include "ppd.h"
#include "message.h"
#include "routines.h"
#include "lexppd.h"

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define YYPRINT(file, type, value)   yyprint (file, type, value)

/* Comes from the caller */
extern FILE * ppdin;
extern struct a2ps_job * job;

/* Local prototypes */
void yyerror PARAMS ((const char *msg));
static void yyprint ();

/* Initilizes the obstacks */
void ppdlex_initialize PARAMS ((void));

int yylex PARAMS ((void));

static struct ppd * ppd_parse_result;

%}
%union
{
  char * string;
  uchar * ustring;
  struct ppd * ppd;
}

%token EOL
%token tDefaultFont tFont tModelName tNickName
%token <string> STRING SYMBOL
%token <ustring> USTRING

%type <ppd> ppd
%type <string> font_clause


%%

/************************************************************************/
/*		Top most						*/
/************************************************************************/
file	:
  ppd
    {
      ppd_parse_result = $1;
      /* Nothing Right Now */;
    }
  ;

ppd :
    /* Empty */
    {
      $$ = ppd_new ();
    }
  | ppd font_clause EOL
    {
      ppd_font_add ($1, $2);
      $$ = $1;
    }
  | ppd tModelName ':' STRING EOL
    {
      if (!$1->modelname)
	$1->modelname = $4;
      $$ = $1;
    }
  | ppd tNickName ':' STRING EOL
    {
      if (!$1->nickname)
	$1->nickname = $4;
      $$ = $1;
    }
  | ppd SYMBOL
    {
      $$ = $1;
    }
  | ppd STRING
    {
      $$ = $1;
    }
  | ppd USTRING
    {
      $$ = $1;
    }
  | ppd ':'
    {
      $$ = $1;
    }
  | ppd EOL
    {
      $$ = $1;
    }
  ;

/************************************************************************/
/*		*Font entry						*/
/************************************************************************/
font_clause :
     tFont SYMBOL ':' SYMBOL STRING SYMBOL SYMBOL
       { $$= $2; }
   | tDefaultFont ':' SYMBOL
       { $$= $3; }
   ;
%%

void
yyerror (const char *msg)
{
  error_at_line (1, 0, ppdfilename, ppdlineno, msg);
}

/*
 * For debugging
 */
static void
yyprint (FILE *file, int type, YYSTYPE value)
{
  switch (type) {
  case STRING:
    fprintf (file, " \"%s\"", value.string);
    break;

  case SYMBOL:
    fprintf (file, " %s", value.string);
    break;

  case USTRING:
    fprintf (file, " u\"%s\"", value.ustring);
    break;
  }
}

struct ppd *
a2ps_ppd_parse (const char * filename, char * const * path)
{
  /* The filename won't be changed. */
  ppdfilename = (char *) filename;
  ppdlineno = 1;
  ppdin = xrfopen (ppdfilename);
  ppdpath = path;

  message (msg_file | msg_ppd | msg_parse,
	   (stderr, "Parsing file `%s'\n", ppdfilename));

  ppdlex_initialize ();

  if (msg_test (msg_parse))
    yydebug = true;
  else
    yydebug = false;

  yyparse ();	/* FIXME: test return value? */

  fclose (ppdin);
  return ppd_parse_result;
}
