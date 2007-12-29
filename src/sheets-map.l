%{ /* -*- c -*- */
/* Scanner for sheets.map files.
 * Copyright (c) 1999-2000 Akim Demaille, Miguel Santana
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

#include "main.h"
#include "select.h"
#include "xobstack.h"

extern struct darray * sheets_map;

int yylex PARAMS ((void));
void yyerror PARAMS ((const char *));
static void yy_include_push PARAMS ((char *file));
static void yy_include_pop PARAMS ((void));

/* Obstack for strings reading */
static struct obstack string_stack;

/* Stack to handle included files. */

#define MAX_INCLUDE_DEPTH 10
static YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
static int lineno_stack[MAX_INCLUDE_DEPTH];
static char *filename_stack[MAX_INCLUDE_DEPTH];
static int include_stack_ptr;
static char *yyfilename;

typedef enum
{
  tKEY = 10,
  tGLOB = 11,
  tFILE = 12
} token_t;

static char *token_val;
static bool insensitive_p;

static inline void
obstack_hexa_grow (struct obstack *o, const char *str)
{
  int value = 0;

  /* FIXME: Requires ASCII?  */
  while (*str)
    if (*str >= 'a' && *str <= 'f')
      value = 16 * value + *str++ - 'a' + 10;
    else if (*str >= 'A' && *str <= 'F')
      value = 16 * value + *str++ - 'A' + 10;
    else
      value = 16 * value + *str++ - '0';
  obstack_1grow (o, value);
}

static inline void
obstack_octal_grow (struct obstack *o, const char *str)
{
  int value = *str++ - '0';

  while (*str)
    value = 8 * value + *str++ - '0';
  obstack_1grow (o, value);
}


%}

%option yylineno
%option prefix="smap"
%option outfile="lex.yy.c"

%x STATE_GLOB
%x STATE_FILE

white         [[:space:]]+
key           [-a-zA-Z0-9_]+:
comment       #.*
include       include\([^)]+\)
%%

{key}         token_val = xstrndup (yytext, yyleng - 1); return tKEY;
\/            BEGIN STATE_GLOB;
\<            BEGIN STATE_FILE;
{white}       ;
{comment}     ;
{include}     {	/* Kill the closing paren and pass the file name. */
                yytext[yyleng - 1] = '\0';
                yy_include_push (yytext + strlen ("include("));
              }

.             { error_at_line (1, 0, yyfilename, yylineno,
			       _("unexpected character `%c'"), *yytext);
              }
<<EOF>>       {   message (msg_file, (stderr, "End of file `%s'.\n",
				      yyfilename));
               	if (--include_stack_ptr < 0)
               	  yyterminate ();
		else
		  yy_include_pop () ;
               }

<STATE_GLOB>{
  \/i? 			{ /* return the string */
    uchar *string;

    obstack_1grow (&string_stack, '\0');
    string = (uchar *) obstack_finish (&string_stack);
    obstack_free (&string_stack, string);
    token_val = xustrdup (string);

    /* Decode the additional flags. */
    insensitive_p = yytext[1] == 'i';

    BEGIN INITIAL;	/* Return to the regular scanning */
    return tGLOB;
  }

  \\[0-7]{1,3}		{ obstack_octal_grow (&string_stack, yytext + 1); }
  \\x[0-9a-fA-F]{1,2}	{ obstack_hexa_grow (&string_stack, yytext + 2); }
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

  \n 			{
    error_at_line (1, 0, yyfilename, yylineno,
		   /* TRANS: %s is ".." or <..> or /../ etc. */
		   _("end of line inside a %s"), "/../");
  }

  [^\/\n\\]+		{
    obstack_grow (&string_stack, yytext, yyleng);
  }
}

<STATE_FILE>{		/* string of characters */
  \>i? 			{ /* return the string */
    uchar *string;

    obstack_1grow (&string_stack, '\0');
    string = (uchar *) obstack_finish (&string_stack);
    obstack_free (&string_stack, string);
    token_val = xustrdup (string);

    /* Decode the additional flags. */
    insensitive_p = yytext[1] == 'i';

    BEGIN INITIAL;	/* Return to the regular scanning */
    return tFILE;
  }

  \\[0-7]{1,3}		{ obstack_octal_grow (&string_stack, yytext + 1); }
  \\x[0-9a-fA-F]{1,2}	{ obstack_hexa_grow (&string_stack, yytext + 2); }
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

  \n 			{
    error_at_line (1, 0, yyfilename, yylineno,
		   _("end of line inside a %s"), "<..>");
  }

  [^>\n\\]+		{
    obstack_grow (&string_stack, yytext, yyleng);
  }
}
%%

/*-----------------------------------------------------.
| Handle the inclusion of files at the scanner level.  |
`-----------------------------------------------------*/

/* Switch the scanning onto FILE, coming back to YYIN later.  */

static void
yy_include_push (char *file)
{
  if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
    error (1, 0, _("too many includes"));

  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  lineno_stack[include_stack_ptr] = yylineno;
  filename_stack[include_stack_ptr++] = yyfilename;

  message (msg_file, (stderr, "%s:%d: includes %s\n",
		      yyfilename, yylineno, file));

  yyfilename = file;
  yyin = xrfopen (yyfilename);

  yy_switch_to_buffer (yy_create_buffer (yyin, YY_BUF_SIZE));
}

/* Pop the inclusion stack and proceed.  To be called on <<EOF>>.*/

static void
yy_include_pop (void)
{
  fclose (yyin);
  yy_delete_buffer (YY_CURRENT_BUFFER);
  yyfilename = filename_stack[include_stack_ptr];
  yylineno = lineno_stack[include_stack_ptr];
  yy_switch_to_buffer (include_stack[include_stack_ptr]);
  message (msg_file, (stderr, "Back to file `%s'.\n", yyfilename));
}


/*----------------------.
| Prepare the scanner.  |
`----------------------*/

/* Initialize the include stack to FILE. */

static inline void
yy_open (const char *file)
{
  yyfilename = xstrdup (file);
  yyin = xrfopen (yyfilename);
  include_stack_ptr = 0;
}

/* End of the scanning. */

static inline void
yy_close (void)
{
  fclose (yyin);
  free (yyfilename);
}

int
yywrap (void)
{
  return 1;
}


void
sheets_map_load (const char *filename)
{
  static int first_time = 1;
  token_t token;
  char *key = NULL;

  if (first_time)
    {
      first_time = 0;
      obstack_init (&string_stack);
    }

  yy_open (filename);

  while ((token = yylex ()))
    {
      switch (token)
	{
	case tKEY:
	  key = token_val;
	  break;

	case tGLOB:
	  if (!key)
	    error_at_line (1, 0, yyfilename, yylineno,
			   _("no key defined for `%s'"), quotearg (token_val));
	  sheets_map_add (token_val, false, insensitive_p, key);
	  break;

	case tFILE:
	  if (!key)
	    error_at_line (1, 0, yyfilename, yylineno,
			   _("no key defined for `%s'"), quotearg (token_val));
	  sheets_map_add (token_val, true, insensitive_p, key);
	  break;
	}
    }

  yy_close ();
}
