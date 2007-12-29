%{ /* -*- c -*- */
/*
 * Lexer for a2ps.
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
 * $Id: lexssh.l,v 1.1.1.1.2.1 2007/12/29 01:58:35 mhatta Exp $
 */

#include "main.h"
#include "yy2ssh.h"
#include "parsessh.h"
#include "xobstack.h"
#include "regex.h"

/* Defines if the comments are printed or not */
extern int strip_level;

/* #define YY_DECL yylex (YYSTYPE *lvalp);*/

int yylex PARAMS ((void));
void yyerror PARAMS ((const char *));

/* Initilizes the obstacks */
void sshlex_initialize PARAMS ((void));

/* Obstack for strings reading */
static struct obstack string_stack;

/* The file we scan. */
const char * sshfilename;

#define LATEX_SYM_RETURN(Str)	\
   yylval.string = (uchar *) xstrdup (Str) ; return tLATEXSYMBOL ;

#define RETURN_FACE(_f_)\
   {yylval.face = _f_ ; return tFACE ;}

#define RETURN_FFLAGS(_f_)\
   {yylval.fflags = _f_ ; return tFFLAGS ;}

%}

%option yylineno
%option prefix="ssh"
%option outfile="lex.yy.c"


%x STATE_STRING STATE_REGEXP

comment 	#.*
blank		[ \t\r\f\n]*
naked_string	[^/#, \t\r\f\n\"()+]*

%%

{comment}	{ ; }
{blank}		{ ; }

\"		{ BEGIN STATE_STRING; }
\/ 		{ BEGIN STATE_REGEXP; }

"\\forall"	LATEX_SYM_RETURN ("\042");
"\\exists"	LATEX_SYM_RETURN ("\044");
"\\suchthat"	LATEX_SYM_RETURN ("\047");
"\\cong"	LATEX_SYM_RETURN ("\100");
"\\Alpha"	LATEX_SYM_RETURN ("\101");
"\\Beta"	LATEX_SYM_RETURN ("\102");
"\\Chi"		LATEX_SYM_RETURN ("\103");
"\\Delta"	LATEX_SYM_RETURN ("\104");
"\\Epsilon"	LATEX_SYM_RETURN ("\105");
"\\Phi"		LATEX_SYM_RETURN ("\106");
"\\Gamma"	LATEX_SYM_RETURN ("\107");
"\\Eta"		LATEX_SYM_RETURN ("\110");
"\\Iota"	LATEX_SYM_RETURN ("\111");
"\\vartheta"	LATEX_SYM_RETURN ("\112");
"\\Kappa"	LATEX_SYM_RETURN ("\113");
"\\Lambda"	LATEX_SYM_RETURN ("\114");
"\\Mu"		LATEX_SYM_RETURN ("\115");
"\\Nu"		LATEX_SYM_RETURN ("\116");
"\\Omicron"	LATEX_SYM_RETURN ("\117");
"\\Pi"		LATEX_SYM_RETURN ("\120");
"\\Theta"	LATEX_SYM_RETURN ("\121");
"\\Rho"		LATEX_SYM_RETURN ("\122");
"\\Sigma"	LATEX_SYM_RETURN ("\123");
"\\Tau"		LATEX_SYM_RETURN ("\124");
"\\Upsilon"	LATEX_SYM_RETURN ("\125");
"\\varsigma"	LATEX_SYM_RETURN ("\126");
"\\Omega"	LATEX_SYM_RETURN ("\127");
"\\Xi"		LATEX_SYM_RETURN ("\130");
"\\Psi"		LATEX_SYM_RETURN ("\131");
"\\Zeta"	LATEX_SYM_RETURN ("\132");
"\\therefore"	LATEX_SYM_RETURN ("\134");
"\\perp"	LATEX_SYM_RETURN ("\136");
"\\radicalex"	LATEX_SYM_RETURN ("\140");
"\\alpha"	LATEX_SYM_RETURN ("\141");
"\\beta"	LATEX_SYM_RETURN ("\142");
"\\chi"		LATEX_SYM_RETURN ("\143");
"\\delta"	LATEX_SYM_RETURN ("\144");
"\\epsilon"	LATEX_SYM_RETURN ("\145");
"\\phi"		LATEX_SYM_RETURN ("\146");
"\\gamma"	LATEX_SYM_RETURN ("\147");
"\\eta"		LATEX_SYM_RETURN ("\150");
"\\iota"	LATEX_SYM_RETURN ("\151");
"\\varphi"	LATEX_SYM_RETURN ("\152");
"\\kappa"	LATEX_SYM_RETURN ("\153");
"\\lambda"	LATEX_SYM_RETURN ("\154");
"\\mu"		LATEX_SYM_RETURN ("\155");
"\\nu"		LATEX_SYM_RETURN ("\156");
"\\omicron"	LATEX_SYM_RETURN ("\157");
"\\pi"		LATEX_SYM_RETURN ("\160");
"\\theta"	LATEX_SYM_RETURN ("\161");
"\\rho"		LATEX_SYM_RETURN ("\162");
"\\sigma"	LATEX_SYM_RETURN ("\163");
"\\tau"		LATEX_SYM_RETURN ("\164");
"\\upsilon"	LATEX_SYM_RETURN ("\165");
"\\varpi"	LATEX_SYM_RETURN ("\166");
"\\omega"	LATEX_SYM_RETURN ("\167");
"\\xi"		LATEX_SYM_RETURN ("\170");
"\\psi"		LATEX_SYM_RETURN ("\171");
"\\zeta"	LATEX_SYM_RETURN ("\172");
"\\sim"		LATEX_SYM_RETURN ("\176");
"\\varUpsilon"	LATEX_SYM_RETURN ("\241");
"\\prime"	LATEX_SYM_RETURN ("\242");
"\\leq"		LATEX_SYM_RETURN ("\243");
"\\infty"	LATEX_SYM_RETURN ("\245");
"\\florin"	LATEX_SYM_RETURN ("\246");
"\\clubsuit"	LATEX_SYM_RETURN ("\247");
"\\diamondsuit"	LATEX_SYM_RETURN ("\250");
"\\heartsuit"	LATEX_SYM_RETURN ("\251");
"\\spadesuit"	LATEX_SYM_RETURN ("\252");
"\\leftrightarrow" LATEX_SYM_RETURN ("\253");
"\\leftarrow"	LATEX_SYM_RETURN ("\254");
"\\uparrow"	LATEX_SYM_RETURN ("\255");
"\\rightarrow"	LATEX_SYM_RETURN ("\256");
"\\downarrow"	LATEX_SYM_RETURN ("\257");
"\\circ"	LATEX_SYM_RETURN ("\260");
"\\pm"		LATEX_SYM_RETURN ("\261");
"\\geq"		LATEX_SYM_RETURN ("\263");
"\\times"	LATEX_SYM_RETURN ("\264");
"\\propto"	LATEX_SYM_RETURN ("\265");
"\\partial"	LATEX_SYM_RETURN ("\266");
"\\bullet"	LATEX_SYM_RETURN ("\267");
"\\div"		LATEX_SYM_RETURN ("\270");
"\\neq"		LATEX_SYM_RETURN ("\271");
"\\equiv"	LATEX_SYM_RETURN ("\272");
"\\approx"	LATEX_SYM_RETURN ("\273");
"\\ldots"	LATEX_SYM_RETURN ("\274");
"---"		LATEX_SYM_RETURN ("\276");
"\\carriagereturn" LATEX_SYM_RETURN ("\277");
"\\aleph"	LATEX_SYM_RETURN ("\300");
"\\Im"		LATEX_SYM_RETURN ("\301");
"\\Re"		LATEX_SYM_RETURN ("\302");
"\\wp"		LATEX_SYM_RETURN ("\303");
"\\otimes"	LATEX_SYM_RETURN ("\304");
"\\oplus"	LATEX_SYM_RETURN ("\305");
"\\emptyset"	LATEX_SYM_RETURN ("\306");
"\\cap"		LATEX_SYM_RETURN ("\307");
"\\cup"		LATEX_SYM_RETURN ("\310");
"\\supset"	LATEX_SYM_RETURN ("\311");
"\\supseteq"	LATEX_SYM_RETURN ("\312");
"\\not\\subset"	LATEX_SYM_RETURN ("\313");
"\\subset"	LATEX_SYM_RETURN ("\314");
"\\subseteq"	LATEX_SYM_RETURN ("\315");
"\\in"		LATEX_SYM_RETURN ("\316");
"\\not\\in"	LATEX_SYM_RETURN ("\317");
"\\angle"	LATEX_SYM_RETURN ("\320");
"\\nabla"	LATEX_SYM_RETURN ("\321");
"\\varregister"	LATEX_SYM_RETURN ("\322");
"\\varcopyright" LATEX_SYM_RETURN ("\324");
"\\vartrademark" LATEX_SYM_RETURN ("\324");
"\\prod"	LATEX_SYM_RETURN ("\325");
"\\surd"	LATEX_SYM_RETURN ("\326");
"\\cdot"	LATEX_SYM_RETURN ("\327");
"\\not"		LATEX_SYM_RETURN ("\330");
"\\wedge"	LATEX_SYM_RETURN ("\331");
"\\vee"		LATEX_SYM_RETURN ("\332");
"\\Leftrightarrow" LATEX_SYM_RETURN ("\333");
"\\Leftarrow"	LATEX_SYM_RETURN ("\334");
"\\Uparrow"	LATEX_SYM_RETURN ("\335");
"\\Rightarrow"	LATEX_SYM_RETURN ("\336");
"\\Downarrow"	LATEX_SYM_RETURN ("\337");
"\\vardiamondsuit" LATEX_SYM_RETURN ("\340");
"\\langle"	LATEX_SYM_RETURN ("\341");
"\\register"	LATEX_SYM_RETURN ("\342");
"\\copyright"	LATEX_SYM_RETURN ("\342");
"\\trademark" 	LATEX_SYM_RETURN ("\344");
"\\sum"		LATEX_SYM_RETURN ("\345");
"\\lceil"	LATEX_SYM_RETURN ("\351");
"\\lfloor"	LATEX_SYM_RETURN ("\353");
"\\rangle"	LATEX_SYM_RETURN ("\361");
"\\int"		LATEX_SYM_RETURN ("\362");
"\\rceil"	LATEX_SYM_RETURN ("\371");
"\\rfloor"	LATEX_SYM_RETURN ("\373");

\\[0-9]+		{
  int value = yytext[1] - '0';
  char *cursor = yytext + 2;

  while (*cursor)
    value = 8 * value + *cursor++ - '0';
  yylval.integer = value;
  return tBACK_REF;
}

"a2ps"		return tA2PS;
"alphabet" 	return tALPHABET;
"alphabets" 	return tALPHABETS;
"ancestors" 	return tANCESTORS;
"are"	 	return tARE;
"by"	 	return tBY;
"case" 		return tCASE;
"closers"	return tCLOSERS;
"C-char" 	return tCCHAR;
"C-string" 	return tCSTRING;
"documentation"	return tDOCUMENTATION;
"end"	 	return tEND;
"exceptions" 	return tEXCEPTIONS;
"first" 	return tFIRST;
"in" 		return tIN;
"insensitive" 	{ yylval.sensitiveness = case_insensitive ;
		   return tSENSITIVENESS;
		}
"is" 		return tIS;
"keywords" 	return tKEYWORDS;
"requires" 	return tREQUIRES;
"second" 	return tSECOND;
"sensitive" 	{ yylval.sensitiveness = case_sensitive ;
	          return tSENSITIVENESS;
		}
"operators" 	return tOPERATORS;
"optional" 	return tOPTIONAL;
"sequences" 	return tSEQUENCES;
"style" 	return tSTYLE;
"version" 	return tVERSION;
"written" 	return tWRITTEN;

"Plain" 	RETURN_FACE (Plain)
"Keyword" 	RETURN_FACE (Keyword)
"Keyword_strong" RETURN_FACE (Keyword_strong)
"Error" 	RETURN_FACE (Error)
"Label" 	RETURN_FACE (Label)
"Label_strong"	RETURN_FACE (Label_strong)
"String" 	RETURN_FACE (String)
"Symbol" 	RETURN_FACE (Symbol)
"Comment" 	{	/* Strip if required */
		  if ((strip_level == 1) || (strip_level == 3))
		    RETURN_FFLAGS (ff_Invisible)
		  else
		    RETURN_FACE (Comment)
		}
"Comment_strong"	{	/* Strip if required */
		  if ((strip_level == 2) || (strip_level == 3))
		    RETURN_FFLAGS (ff_Invisible)
		  else
		    RETURN_FACE (Comment_strong)
		}

"Tag1" 		RETURN_FFLAGS (ff_Tag1)
"Tag2"	 	RETURN_FFLAGS (ff_Tag2)
"Tag3" 		RETURN_FFLAGS (ff_Tag3)
"Tag4"	 	RETURN_FFLAGS (ff_Tag4)
"Index1" 	RETURN_FFLAGS (ff_Index1)
"Index2" 	RETURN_FFLAGS (ff_Index2)
"Index3" 	RETURN_FFLAGS (ff_Index3)
"Index4" 	RETURN_FFLAGS (ff_Index4)
"Encoding" 	RETURN_FFLAGS (ff_Encoding)
"Invisible"	RETURN_FFLAGS (ff_Invisible)

{naked_string}	{  yylval.string = xustrdup (yytext);
		   return tSTRING;
		}

.		{ return yytext[0]; }

<STATE_STRING>{		/* string of characters */
  \" 			{ /* return the string */
    uchar * string;

    obstack_1grow (&string_stack, '\0');
    string = (uchar *) obstack_finish (&string_stack);

    obstack_free (&string_stack, string);

    BEGIN INITIAL;	/* Return to the regular scanning */
    yylval.string = xustrdup (string);
    return tSTRING;
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

  \n 			{
    yyerror (_("end-of-line in string constant"));
  }

  [^\"\n\\]+		{
    obstack_grow (&string_stack, yytext, yyleng);
  }
}

<STATE_REGEXP>{		/* a regular expression */
  \/ 			{ /* return the string */
    char * pattern;
    int pattern_len;

    /* I'm not sure I got the 0 terminate the pattern */
    obstack_1grow (&string_stack, '\0');
    pattern_len = obstack_object_size (&string_stack);
    pattern = (char *) obstack_finish (&string_stack);
    obstack_free (&string_stack, pattern);

    yylval.pattern = XMALLOC (struct pattern, 1);
    /* len - 1, because the NUL must not be part of the pattern that
     * will be compiled.  We put tough, to be able to use the pattern
     * in regular C strings manipulations. */
    yylval.pattern->len = pattern_len - 1;
    yylval.pattern->pattern = XMALLOC (char, pattern_len);
    memcpy (yylval.pattern->pattern, pattern, pattern_len);

    BEGIN INITIAL;	/* Return to the regular scanning */
    return tREGEX;
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

  \n 			{
    error_at_line (1, 0, sshfilename, sshlineno,
		   _("end of line inside a %s"), "\"..\"");
  }

  [^\n\\\/]+		{
    obstack_grow (&string_stack, yytext, yyleng);
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
sshlex_initialize (void)
{
  static int first_time = 1;
  if (first_time)
    {
      first_time = 0;
      obstack_init (&string_stack);
    }
}
