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
 * $Id: parsessh.y,v 1.1.1.1.2.2 2007/12/29 01:58:35 mhatta Exp $
 */

#include "a2ps.h"
#include "jobs.h"
#include "ffaces.h"
#include "ssheet.h"
#include "message.h"
#include "routines.h"
#include "yy2ssh.h"
#include "regex.h"

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define YYPRINT(file, type, value)   yyprint (file, type, value)

/* We need to use the same `const' as bison, to avoid the following
   prototypes to diverge from the function declarations */
#undef const
#ifndef __cplusplus
# ifndef __STDC__
#  define const
# endif
#endif

/* Comes from the parser */
extern int sshlineno;

/* Comes from the caller */
extern FILE * sshin;
extern struct a2ps_job * job;
extern const char * sshfilename;

/* Local prototypes */
void yyerror PARAMS ((const char *msg));
static void yyprint ();

/* Initilizes the obstacks */
void sshlex_initialize PARAMS ((void));

/* Comes from main.c */
extern int highlight_level;

int yylex PARAMS ((void));
struct style_sheet * parse_style_sheet PARAMS ((const char * filename));

/* Defines the style sheet being loaded */
static struct style_sheet * parsed_style_sheet = NULL;

%}
%union
{
  int integer;
  uchar * string;
  struct pattern * pattern;
  struct style_sheet * sheet;
  struct rule * rule;
  struct sequence * sequence;
  struct darray * array;
  struct words * words;
  struct faced_string * faced_string;
  enum face_e face;			/* Face			*/
  enum fflag_e fflags;			/* Flags for faces	*/
  struct fface_s fface;			/* Flagged face		*/
  enum case_sensitiveness sensitiveness;
}

%token tSTYLE tIS tEND tKEYWORDS tARE tIN tOPERATORS tSEQUENCES
%token tFIRST tSECOND tALPHABET tALPHABETS tDOCUMENTATION tEXCEPTIONS
%token tCASE tCSTRING tCCHAR tOPTIONAL tCLOSERS
%token tWRITTEN tBY tVERSION tREQUIRES tA2PS tANCESTORS
%token <face> tFACE
%token <fflags> tFFLAGS
%token <string> tSTRING tLATEXSYMBOL
%token <pattern> tREGEX
%token <sensitiveness> tSENSITIVENESS
%token <integer> tBACK_REF

%type <pattern> regex
%type <fface> fface fface_sxp
%type <string> authors documentation version long_string requirement
%type <faced_string> a_rhs
%type <rule> rule keyword_regex operator_regex
%type <sheet> style_sheet definition_list
%type <words> keywords_def keywords_rule_list
%type <words> operators_def operators_rule_list
%type <array> sequence_list sequence_def
%type <words> exception_def_opt
%type <array> ancestors_def ancestors_list
%type <words> closers_opt
%type <sensitiveness> case_def
%type <array> rhs rhs_list
%type <sequence> sequence
%%

/************************************************************************/
/*		Top most						*/
/************************************************************************/
file	:
  style_sheet
    {
      parsed_style_sheet = $1;
    }
  ;

style_sheet :
   tSTYLE tSTRING tIS definition_list tEND tSTYLE
      {
	$4->name = $2;
	$4->key = "<No key yet>";
	$$ = $4;
      }
   ;

definition_list :
	/* empty */ {
	  $$ = new_style_sheet ((const uchar *) "<no name>");
	}
	| definition_list tOPTIONAL tKEYWORDS keywords_def tKEYWORDS
	{
	  if (highlight_level == 2) {
	    words_set_no_face ($4, Plain_fface);
	    words_merge_rules_unique ($1->keywords, $4);
	  } else
	    words_free ($4);
	  $$ = $1;
	}
	| definition_list tKEYWORDS keywords_def tKEYWORDS
	{
	  words_set_no_face ($3, Plain_fface);
	  words_merge_rules_unique ($1->keywords, $3);
	  $$ = $1;
	}
	| definition_list tOPTIONAL tOPERATORS operators_def tOPERATORS
	{
	  if (highlight_level == 2) {
	    words_set_no_face ($4, Plain_fface);
	    words_merge_rules_unique ($1->operators, $4);
	  } else
	    words_free ($4);
	  $$ = $1;
	}
	| definition_list tOPERATORS operators_def tOPERATORS
	{
	  words_set_no_face ($3, Plain_fface);
	  words_merge_rules_unique ($1->operators, $3);
	  $$ = $1;
	}
	| definition_list tOPTIONAL sequence_def {
	  if (highlight_level == 2) {
	    da_concat ($1->sequences, $3);
	    da_erase ($3);
	  } else
	    da_free ($3, (da_map_func_t) free_sequence);
	  $$ = $1;
	}
	| definition_list sequence_def {
	  da_concat ($1->sequences, $2);
	  da_erase ($2);
	  $$ = $1;
	}
	| definition_list ancestors_def {
	  da_concat ($1->ancestors, $2);
	  da_erase ($2);
	  $$ = $1;
	}
	| definition_list tALPHABETS tARE tSTRING {
	  string_to_array ($1->alpha1, $4);
	  string_to_array ($1->alpha2, $4);
	  /* This is the syntax table used by regex */
	  free ($4);
	  $4 = NULL;
	  $$ = $1;
	}
	| definition_list tFIRST tALPHABET tIS tSTRING {
	  string_to_array ($1->alpha1, $5);
	  /* This is the syntax table used by regex */
	  free ($5);
	  $5 = NULL;
	  $$ = $1;
	}
	| definition_list tSECOND tALPHABET tIS tSTRING {
	  string_to_array ($1->alpha2, $5);
	  /* This is the syntax table used by regex */
	  free ($5);
	  $5 = NULL;
	  $$ = $1;
	}
	| definition_list case_def {
	  $1->sensitiveness = $2;
	}
	| definition_list documentation {
	  $1->documentation = $2;
	}
	| definition_list authors {
	  $1->author = $2;
	}
	| definition_list version {
	  style_sheet_set_version ($1, (const char *) $2);
	}
	| definition_list requirement {
	  /* Make sure now that we won't encounter new tokens.
	   * This avoids nasty error messages, or worse:
	   * unexpected behavior at run time */
	  if (!style_sheet_set_requirement ($1, (const char *) $2))
	    error (1, 0,
		   _("cannot process `%s' which requires a2ps version %s"),
		   sshfilename, $2);
	}
	;

/************************************************************************/
/*		Dealing with the inessential informations		*/
/************************************************************************/
requirement :
	  tREQUIRES tA2PS tSTRING { $$ = $3 ; }
	| tREQUIRES tA2PS tVERSION tSTRING { $$ = $4 ; } ;

documentation :
	  tDOCUMENTATION tIS long_string tEND tDOCUMENTATION { $$ = $3 ; };

long_string: tSTRING { $$ = $1; }
	| long_string tSTRING
	{
	  size_t len1;
	  size_t len2;

	  len1 = ustrlen ($1);
	  $1[len1] = '\n';
	  len2 = ustrlen ($2);
	  $$ = XMALLOC (uchar, len1 + len2 + 2);
	  ustpcpy (ustpncpy ($$, $1, len1 + 1), $2);
	  free ($1);
	  free ($2);
	}
	;

authors : tWRITTEN tBY tSTRING	{ $$ = $3 ; };

version :
	  tVERSION tIS tSTRING 	{ $$ = $3 ; }
	| tVERSION tSTRING 	{ $$ = $2 ; };

/************************************************************************/
/*		Dealing with the ancestors of a style sheet		*/
/************************************************************************/
ancestors_def : tANCESTORS tARE ancestors_list tEND tANCESTORS
	{
	  /* The list of keys of style sheets from which it inherits */
  	  $$ = $3;
	}
	;

ancestors_list: tSTRING
	{
	  /* Create a list of ancestors, and drop the new one in */
	  $$ = da_new ("Ancestors tmp", 2,
		       da_linear, 2,
		       (da_print_func_t) da_str_print, NULL);
	  da_append ($$, $1);
	}
	| ancestors_list ',' tSTRING
	{
	  da_append ($1, $3);
	  $$ = $1;
	}
	;

case_def : tCASE tSENSITIVENESS { $$ = $2 ; } ;

/************************************************************************/
/*	Rhs							*/
/* (Lists of (strings/regexp back references, face)			*/
/************************************************************************/
/* Regex split upon several lines */
regex:
	  tREGEX
	{
	  $$ = $1;
	}
	| regex tREGEX
	{
	  /* Concatenate $2 to $1 makes $$ */
	  $$ = $1;
	  $$->pattern = XREALLOC ($$->pattern, char, $1->len + $2->len + 1);
	  strncpy ($$->pattern + $$->len, $2->pattern, $2->len);
	  $$->len += $2->len;
	  free ($2->pattern);
	}
	;

rhs:
	  a_rhs
	{
	  $$ = rhs_new ();
	  rhs_add ($$, $1);
	}
	;

a_rhs:
	  tSTRING fface
	{
	  $$ = faced_string_new  ($1, 0, $2);
	}
	| tSTRING
	{
	  $$ = faced_string_new  ($1, 0, No_fface);
	}
	| fface
	{
	  $$ = faced_string_new  (NULL, 0, $1);
	}
	| tBACK_REF fface
	{
	  $$ = faced_string_new  (UNULL, $1, $2);
	}
	| tBACK_REF
	{
	  $$ = faced_string_new  (UNULL, $1, No_fface);
	}
	| tLATEXSYMBOL
	{
	  $$ = faced_string_new ($1, 0, Symbol_fface);
	}
	;

rhs_list:
	a_rhs
	{
	  $$ = rhs_new ();
	  rhs_add ($$, $1);
	}
	|
	rhs_list ',' a_rhs
	{
	  rhs_add ($1, $3);
	  $$ = $1;
	}
	;

/*
 * The flagged faces (One (true face or Invisible) plus flags)
 */
fface:
	 tFACE
	{
	  fface_set_face ($$, $1);
	  fface_reset_flags ($$);
	}
	| tFFLAGS
	{
	  fface_reset_face ($$);
	  fface_set_flags ($$, $1);
	  /* If there is no face, then set Invisible */
	  fface_add_flags ($$, ff_Invisible);
	}
	| '(' fface_sxp ')'
	{
	  $$ = $2;
	  /* If there is no face, then set Invisible */
	  if (fface_get_face ($$) == No_face)
	    fface_add_flags ($$, ff_Invisible);
	}
	;

fface_sxp:
	 tFACE
	{
	  fface_set_face($$, $1);
	  fface_reset_flags($$);
	}
	| tFFLAGS
	{
	  fface_reset_face($$);
	  fface_set_flags($$, $1);
	}
	| fface_sxp '+' tFACE
	{
	  /* FIXME: Overloading of the face should be forbidden */
	  $$ = $1;
	  fface_set_face($$, $3);
	}
	| fface_sxp '+' tFFLAGS
	{
	  $$ = $1;
	  fface_add_flags($$, $3);
	}
	;

/************************************************************************/
/*		Symbol atoms						*/
/************************************************************************/
/* A rule is the group of 1. string to match, 2. the rhs
 *
 * In the special shortcut where no face is given, see where
 * rule_list appears for resolving */
rule:   tSTRING rhs
	{
	  $$ = rule_new ($1, NULL, $2,
			 sshfilename, sshlineno);
	}
	| tSTRING
	{
	  $$ = rule_new ($1, NULL,
			 rhs_new_single (UNULL, 0, No_fface),
			 sshfilename, sshlineno);
	}
	| '(' tSTRING rhs_list ')'
	{
	  $$ = rule_new ($2, NULL, $3,
			 sshfilename, sshlineno);
	}
	;

/************************************************************************/
/*		Keywords lists						*/
/* We make the difference because the regex must be compiled with a	*/
/* difference (which is \\b\\(%s\\)\\b) between keywords and operators	*/
/************************************************************************/
keywords_def : tIN fface tARE keywords_rule_list tEND {
	  words_set_no_face ($4, $2);
	  $$ = $4;
	}
	| tARE keywords_rule_list tEND {
  	  /* First of all, the No_face must be turned into Plain */
  	  $$ = $2;
	}
	;

keywords_rule_list:
	  rule
	{
	  $$ = words_new ("Keywords: Strings", "Keywords: Regexps", 100, 100);
	  words_add_string ($$, $1);
	}
	| keyword_regex
	{
	  $$ = words_new ("Keywords: Strings", "Keywords: Regexps", 100, 100);
	  words_add_regex ($$, $1);
	}
	| keywords_rule_list ',' rule
	{
	  words_add_string ($1, $3);
	  $$ = $1;
	}
	| keywords_rule_list ',' keyword_regex
	{
	  words_add_regex ($1, $3);
	  $$ = $1;
	}
	;

/* A rule is the group of 1. string to match, 2. string to
 * print, 3. face of the string to print.  In the special
 * shortcut where no face is given, see where rule_list appears
 * for resolving */
keyword_regex:
	  regex rhs
	{
	  $$ = keyword_rule_new (UNULL, $1, $2,
				 sshfilename, sshlineno);
	}
	| regex
	{
	  $$ = keyword_rule_new (UNULL, $1,
				   rhs_new_single (UNULL, 0,
							    No_fface),
				 sshfilename, sshlineno);
	}
	| '(' regex rhs_list ')'
	{
	  $$ = keyword_rule_new (UNULL, $2, $3,
				 sshfilename, sshlineno);
	}
	;

/************************************************************************/
/*		Operators lists						*/
/************************************************************************/
operators_def: tIN fface tARE operators_rule_list tEND {
	  words_set_no_face ($4, $2);
	  $$ = $4;
	}
	| tARE operators_rule_list tEND {
  	  /* First of all, the No_face must be turned into Plain */
  	  $$ = $2;
	}
	;

operators_rule_list:
	  rule
	{
	  $$ = words_new ("Operators: Strings", "Operators: Regexps",
			  100, 100);
	  words_add_string ($$, $1);
	}
	| operator_regex
	{
	  $$ = words_new ("Operators: Strings", "Operators: Regexps",
			  100, 100);
	  words_add_regex ($$, $1);
	}
	| operators_rule_list ',' rule
	{
	  words_add_string ($1, $3);
	  $$ = $1;
	}
	| operators_rule_list ',' operator_regex
	{
	  words_add_regex ($1, $3);
	  $$ = $1;
	}
	;

/* A rule is the group of 1. string to match, 2. string to
 * print, 3. face of the string to print.  In the special
 * shortcut where no face is given, see where rule_list appears
 * for resolving */
operator_regex:
	  regex rhs
	{
	  $$ = rule_new (UNULL, $1, $2,
			 sshfilename, sshlineno);
	}
	| regex
	{
	  $$ = rule_new (UNULL, $1,
			   rhs_new_single (UNULL, 0, No_fface),
			 sshfilename, sshlineno);
	}
	| '(' regex rhs_list ')'
	{
	  $$ = rule_new (UNULL, $2, $3,
			 sshfilename, sshlineno);
	}
	;

/************************************************************************/
/*		Dealing with the sequences				*/
/************************************************************************/
sequence_def : tSEQUENCES tARE sequence_list tEND tSEQUENCES { $$ = $3; }
	;

sequence_list: sequence {
	  $$ = da_new ("Sequence tmp", 100,
		       da_linear, 100,
		       (da_print_func_t) sequence_self_print, NULL);
	  da_append ($$, $1);
	}
	| sequence_list ',' sequence {
	  da_append ($1, $3);
	  $$ = $1;
	}
	;

/*
 * I can understand one will get sick reading this.  It hurts, indeed,
 * but is necessary because of shift/reduce conflicts if one uses
 *
 *    sequence: operators_rule face closers_opt exception_def_opt
 *
 * The problem is that when bison/yacc reads this
 *
 *    <word> <word> . <face1> <face2>
 *
 * it doesn't know whether <face1> should be in $1, or in $2.
 * So we have to inline the rule at hand...
 */
sequence:
	/* Expansion of each possibility for
	 * <operators_rule> <face> <closers_opt> <exception_def_opt>
	 */
	tSTRING tLATEXSYMBOL fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($1, NULL,
				rhs_new_single ($2, 0, Symbol_fface),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $3, $4, $5);
	}
	| tSTRING tSTRING fface fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($1, NULL,
				rhs_new_single ($2, 0, $3),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $4, $5, $6);
	}
	| tSTRING fface fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($1, NULL,
				rhs_new_single (UNULL, 0, $2),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $3, $4, $5);
	}
	| tSTRING tSTRING fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($1, NULL,
				rhs_new_single ($2, 0, $3),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $3, $4, $5);
	}
	| tSTRING fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($1, NULL,
				rhs_new_single (UNULL, 0, $2),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $2, $3, $4);
	}
	| regex tSTRING fface fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new (UNULL, $1,
				rhs_new_single ($2, 0, $3),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $4, $5, $6);
	}
	| regex fface fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new (UNULL, $1,
				rhs_new_single (UNULL, 0, $2),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $3, $4, $5);
	}
	| regex tSTRING fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new (UNULL, $1,
				rhs_new_single ($2, 0, $3),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $3, $4, $5);
	}
	| regex fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new (UNULL, $1,
				rhs_new_single (UNULL, 0, No_fface),
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $2, $3, $4);
	}
	| '(' tSTRING rhs_list ')' fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new ($2, NULL, $3,
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $5, $6, $7);
	}
	| '(' regex rhs_list ')' fface closers_opt exception_def_opt
	{
	  struct rule * open_rule;
	  open_rule = rule_new (UNULL, $2, $3,
				sshfilename, sshlineno);
	  $$ = sequence_new (open_rule, $5, $6, $7);
	}
/* End of the brute force expansion */
	| tCSTRING
	{
	  $$ = new_C_string_sequence ("\"");
	}
	| tCCHAR
	{
	  $$ = new_C_string_sequence ("\'");
	}
	;

closers_opt:
	/* Nothing */
	{
	  /* This is a shortcut which means "up to the end of the line". */
	  $$ = words_new ("Closing: Strings", "Closing: Regexps", 2, 2);
	  words_add_string ($$, rule_new (xustrdup ("\n"), NULL,
					  rhs_new_single (NULL, 0,
							  No_fface),
					  sshfilename, sshlineno));
	}
	| rule
	{
	  /* Only one */
	  $$ = words_new ("Closing: Strings", "Closing: Regexps", 2, 2);
	  words_add_string ($$, $1);
	}
	| operator_regex
	{
	  /* Only one */
	  $$ = words_new ("Closing: Strings", "Closing: Regexps", 2, 2);
	  words_add_regex ($$, $1);
	}
	| tCLOSERS operators_def tCLOSERS
	{
	  /* Several, comma separated, between () */
	  $$ = $2;
	}
	;

exception_def_opt:
	  /* Nothing */
	{
	  $$ = words_new ("Exceptions: Strings", "Exceptions: Regexps", 1, 1);
	}
	| tEXCEPTIONS operators_def tEXCEPTIONS
	{
	  $$ = $2;
	};

%%

void
yyerror (const char *msg)
{
  error_at_line (1, 0, sshfilename, sshlineno, msg);
}

/*
 * FIXME: Cover the other relevant types
 */
static void
yyprint (FILE *file, int type, YYSTYPE value)
{
  switch (type) {
  case tBACK_REF:
    fprintf (file, " \\%d", value.integer);
    break;

  case tFFLAGS:
    putc (' ', file);
    fflag_self_print (value.fflags, file);
    break;

  case tFACE:
    fprintf (file, " %s", face_to_string (value.face));
    break;

  case tREGEX:
    fprintf (file, " /%s/", value.pattern->pattern);
    break;

  case tSTRING:
    fprintf (file, " \"%s\"", value.string);
    break;
  }
}

struct style_sheet *
parse_style_sheet (const char * filename)
{
  int res;

  sshfilename = filename;
  sshlineno = 1;
  sshin = xrfopen (sshfilename);

  message (msg_file | msg_sheet | msg_parse,
	   (stderr, "Parsing file `%s'\n", sshfilename));

  sshlex_initialize ();

  if (msg_test (msg_parse))
    yydebug = true;
  res = yyparse ();  /* FIXME: test the result of parsing */

  if (msg_test (msg_sheet)) {
    fprintf (stderr, "---------- Right after parsing of %s\n",
	     parsed_style_sheet->key);
    style_sheet_self_print (parsed_style_sheet, stderr);
    fprintf (stderr, "---------- End of after parsing of %s\n",
	     parsed_style_sheet->key);
  }

  fclose (sshin);
  return parsed_style_sheet;
}
