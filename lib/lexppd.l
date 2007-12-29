%{ /* -*- c -*- */
/*
 * Lexer for PPD files.
 *
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

/*
 * $Id: lexppd.l,v 1.1.1.1.2.1 2007/12/29 01:58:19 mhatta Exp $
 */

#include "a2ps.h"
#include "lexppd.h"
#include "ppd.h"
#include "parseppd.h"
#include "routines.h"
#include "xobstack.h"
#include "message.h"
#include "pathwalk.h"

/* File currently parsed, and the path where to find PPD files. */
char * ppdfilename;
char * const *ppdpath;

int yylex PARAMS ((void));
void yyerror PARAMS ((const char *));

/* Initilizes the obstacks */
void ppdlex_initialize PARAMS ((void));

/* Obstack for strings reading */
static struct obstack string_stack;

/* Stack to handle included PPD files. */

#define MAX_INCLUDE_DEPTH 10
static YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
static char *filename_stack[MAX_INCLUDE_DEPTH];
static int include_stack_ptr = 0;
%}

%option yylineno
%option prefix="ppd"
%option outfile="lex.yy.c"


%x STATE_STRING
%x STATE_USTRING
%x STATE_INCLUDE

eol	\n|\r\n
comment	\*%.*{eol}
blank	[ \t\f]*
symbol	[^/#, \t\r\f\n\":()+]*
%%

{comment}	{ ; }
{blank}		{ ; }

\"		{ BEGIN STATE_STRING; }
\/		{ BEGIN STATE_USTRING; }
"*Include:"	{ BEGIN STATE_INCLUDE; }

"*DefaultFont"	{ return tDefaultFont; }
"*Font"		{ return tFont; }
"*ModelName"	{ return tModelName; }
"*NickName"	{ return tNickName; }

{symbol}	{  yylval.string = xstrdup (yytext);
		   return SYMBOL;
		}

.		{ return yytext[0]; }

{eol}		{ return EOL; }

<STATE_STRING>{		/* string of characters */
  \"		{ /* return the string */
    char * string;

    obstack_1grow (&string_stack, '\0');
    string = obstack_finish (&string_stack);

    obstack_free (&string_stack, string);

    BEGIN INITIAL;	/* Return to the regular scanning */
    yylval.string = xstrdup (string);

    return STRING;
  }

  \\[0-7]{1,3}		{
    int value = yytext[1] - '0';
    char *cursor = yytext + 2;

    while (*cursor)
      value = 8 * value + *cursor++ - '0';
    obstack_1grow (&string_stack, value);
  }

  \\x[0-9a-fA-F]{1,2}	{
    int value = 0;
    char *cursor = yytext + 2;

    while (*cursor)
      if (*cursor >= 'a' && *cursor <= 'f')
	value = 16 * value + *cursor++ - 'a' + 10;
      else if (*cursor >= 'A' && *cursor <= 'F')
	value = 16 * value + *cursor++ - 'A' + 10;
      else
	value = 16 * value + *cursor++ - '0';
    obstack_1grow (&string_stack, value);
  }

  \\a			{ obstack_1grow (&string_stack, '\007'); }
  \\b			{ obstack_1grow (&string_stack, '\b'); }
  \\d			{ obstack_1grow (&string_stack, 127); }
  \\e			{ obstack_1grow (&string_stack, 27); }
  \\f			{ obstack_1grow (&string_stack, '\f'); }
  \\n			{ obstack_1grow (&string_stack, '\n'); }
  \\r			{ obstack_1grow (&string_stack, '\r'); }
  \\t			{ obstack_1grow (&string_stack, '\t'); }
  \\v			{ obstack_1grow (&string_stack, '\v'); }
  \\.			{ obstack_1grow (&string_stack, yytext[1]); }

  [^\"]+		{	/* \n are legal in string */
    obstack_grow (&string_stack, yytext, yyleng);
  }
}

<STATE_USTRING>{		/* string of characters */
  [:\n]		{ /* return the string */
    uchar * string;

    obstack_1grow (&string_stack, '\0');
    string = (uchar *) obstack_finish (&string_stack);

    obstack_free (&string_stack, string);

    BEGIN INITIAL;	/* Return to the regular scanning */
    yylval.ustring = xustrdup (string);

    return USTRING;
  }

  \\[0-7]{1,3}		{
    int value = yytext[1] - '0';
    char *cursor = yytext + 2;

    while (*cursor)
      value = 8 * value + *cursor++ - '0';
    obstack_1grow (&string_stack, value);
  }

  \\x[0-9a-fA-F]{1,2}	{
    int value = 0;
    char *cursor = yytext + 2;

    while (*cursor)
      if (*cursor >= 'a' && *cursor <= 'f')
	value = 16 * value + *cursor++ - 'a' + 10;
      else if (*cursor >= 'A' && *cursor <= 'F')
	value = 16 * value + *cursor++ - 'A' + 10;
      else
	value = 16 * value + *cursor++ - '0';
    obstack_1grow (&string_stack, value);
  }

  \\a			{ obstack_1grow (&string_stack, '\007'); }
  \\b			{ obstack_1grow (&string_stack, '\b'); }
  \\d			{ obstack_1grow (&string_stack, 127); }
  \\e			{ obstack_1grow (&string_stack, 27); }
  \\f			{ obstack_1grow (&string_stack, '\f'); }
  \\n			{ obstack_1grow (&string_stack, '\n'); }
  \\r			{ obstack_1grow (&string_stack, '\r'); }
  \\t			{ obstack_1grow (&string_stack, '\t'); }
  \\v			{ obstack_1grow (&string_stack, '\v'); }
  \\.			{ obstack_1grow (&string_stack, yytext[1]); }

  [^\n\\:]+		{	/* \n are legal in string */
    obstack_grow (&string_stack, yytext, yyleng);
  }
}

<STATE_INCLUDE>{	/* Including another PPD file. */

  [ \t]*      	{ ; } /* eat the whitespace */
  \"[^ \t\n]+\"   { /* got the include file name */
    if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
      error (1, 0, _("too many includes"));

    include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
    filename_stack[include_stack_ptr++] = ppdfilename;

    /* Skip the first quote, kill the last one. */
    yytext ++;
    yytext [yyleng - 2] = 0;
    message (msg_file, (stderr, "%s:%d: includes %s\n",
			ppdfilename, ppdlineno, yytext));

    /* Don't put the suffix, Adobe says it's part of the name. */
    ppdfilename = xpw_find_included_file (ppdpath, ppdfilename,
					  yytext, NULL);
    yyin = xrfopen (ppdfilename);

    yy_switch_to_buffer (yy_create_buffer (yyin, YY_BUF_SIZE));

    BEGIN(INITIAL);
  }
}

<<EOF>> {
  message (msg_file, (stderr, "End of PPD file `%s'.\n", ppdfilename));
  if (--include_stack_ptr < 0)
    {
      yyterminate();
    }
  else
    {
      fclose (yyin);
      yy_delete_buffer (YY_CURRENT_BUFFER);
      XFREE (ppdfilename);
      ppdfilename = filename_stack[include_stack_ptr];
      yy_switch_to_buffer (include_stack[include_stack_ptr]);
      message (msg_file, (stderr, "Back to file `%s'.\n", ppdfilename));
    }
}

%%

int
yywrap (void)
{
  return 1;
}

/*
 * Initialize the obstacks
 */
void
ppdlex_initialize (void)
{
  static int first_time = 1;
  if (first_time)
    {
      first_time = 0;
      obstack_init (&string_stack);
    }
  /* Reset the include stack. */
  include_stack_ptr = 0;
}
