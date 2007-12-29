%p 3000
%{ /* -*- c -*- */ /* <--- EMACS knows now about what mode to use ... */
  /* Time-stamp: "99/06/29 14:48:25 bauer" */

  /* Filter for Windows NT 4.0 postscript printer driver. Enables
   * the use of psnup from Angus Duggan's psutils package
   * ( http://www.dcs.ed.ac.uk/home/ajcd/psutils/ )
   * Copyright: GNU General Public License (GPL)
   * Authors:
   *         Holger Bauer 1998, 1999    bauer@itsm.uni-stuttgart.de
   *         Michael Rath 1998, 1999    rath@itsm.uni-stuttgart.de
   *         Akim Demaille      1999    demaille@inf.enst.fr
   * Version: 0.1c
   * Release Date: 1999/02/04
   *
   * Compilation:
   *
   * flex fixnt.l
   * cc -O -o fixnt lex.yy.c
   *
   * Usage:
   * csh-prompt> cat BAD_NT_Postscript_FILE.ps | fixnt | psnup -4 | gs -
   * or:
   * csh-prompt> cat BAD_NT_Postscript_FILE.ps | fixnt | psnup -4 >GOOD.ps
   *
   * Windows NT 3.5 users may be happy with:
   * csh-prompt> cat BAD_NT_Postscript_FILE.ps | \
   *             sed 's/NTPSOct94/NTPSOct95/g' | fixnt | psnup -4 >GOOD.ps
   *
   * TODO-List:
   * - check for NTPSOct94 (in the meantime use the sed command above)
   *
   *
   * BUG-Reports: to Authors (above) (please, no 20MB postscript files !!!!!!)
   * Patches (welcome) to code maintainer:   bauer@itsm.uni-stuttgart.de
   *
   */


#ifdef HAVE_CONFIG_H
# include "config.h"
# include <stdio.h>
# if defined STDC_HEADERS || defined _LIBC || defined HAVE_STDLIB_H
#  include <stdlib.h>
# endif
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif
# if HAVE_UNISTD_H
#  include <unistd.h>
# endif
#else /* !HAVE_CONFIG_H */
# include <stdio.h>
# ifdef _WIN32
#  include <stdlib.h>
#  include <process.h>
#  include <wtypes.h>
#  include <winbase.h>
# else /* !_WIN32 */
#  include <unistd.h>
# endif
# include <sys/types.h>
char *getenv ();
#endif /* !HAVE_CONFIG_H */

/* Support of prototyping when possible */
#ifndef PARAMS
#  if PROTOTYPES
#    define PARAMS(protos) protos
#  else /* no PROTOTYPES */
#    define PARAMS(protos) ()
#  endif /* no PROTOTYPES */
#endif

/* Do not allow redefinition of malloc and realloc. */
#undef malloc
#undef realloc

#if defined (YYLMAX)
# undef YYLMAX
# define YYLMAX 1024
#endif

void reassemble PARAMS ((FILE*,FILE*,FILE*,char*,char*,char*));
static int first_time = 1;
FILE *font;
FILE *adobe;
FILE *body;
int ifpagesv   = 0;
int ifcorel    = 0;
int ifendsetup = 0;
int pid;
char  fontfname[255];
char adobefname[255];
char  bodyfname[255];
#ifdef _WIN32
char tmpdir[_MAX_PATH];
#else
const char *tmpdir;
#endif
%}

%Start DUMMY NORMAL BODY ADOBE FONT

%%

%{
  if(first_time){
    BEGIN DUMMY;
    first_time = 0;
  }
%}

<DUMMY>%!.*\n { /* regular Postscript starts here ... */
      ECHO;
      BEGIN NORMAL;
}

<DUMMY>^.*\n { /* nothing to do */
}

<NORMAL>%%Title:.*CorelDRAW.*\n { /* Corel Draw modus recognized */
  ifcorel = 1;
  ECHO;
}

<NORMAL>%%EndSetup.*\nNTPSOct95[ ]+begin.*\n { /* suppress EndSetup */
  fprintf(yyout,"NTPSOct95 begin\n");
  ifendsetup = 1;
}

<NORMAL>%%Page:.*\n { /* read up to first page and just output everyting */
  sprintf(bodyfname, "%s/fixnt_Body_%d", tmpdir, pid);
  body = fopen(bodyfname,"w+");
  fprintf(body,"%s",yytext);
  BEGIN BODY;
}

<NORMAL>.*\n {    /* o.k. */
                 fprintf(yyout,"%s",yytext);
}

<BODY>[/]Adobe_WinNT_Driver_Gfx[ ]175[ ]dict[ ]dup[ ]begin.*\n { /* Adobe Stuff */
  sprintf(adobefname, "%s/fixnt_Adobe_%d", tmpdir, pid);
  adobe = fopen(adobefname,"w+");
  fprintf(adobe,"%s",yytext);
  BEGIN ADOBE;
}

<ADOBE>end[ ]reinitialize.*\n { /* End of Adobe definition stuff */
  fprintf(adobe,"%s",yytext);
  BEGIN BODY;
}

<ADOBE>.*\n { /* print just everything ... */
  fprintf(adobe,"%s",yytext);
}


<BODY>^[ ]@gs[ ]spg[ ]@gr.*\n { ;
  /* remove showpage definition of CorelDraw defintions */
  if( ifcorel == 1 ){
    fprintf(body," @gs @gr\n"); /* Corel draw defines spg as showpage */
  } else {
    fprintf(body,"%s",yytext);  /* Somebody else defined spg, leave it in */
  }
}

<BODY>(NTPSOct95[ ]){0,1}[/]FontSV[ ]save[ ](put|def).*\n { ;
  /* font definitions from here on  */
  sprintf(fontfname, "%s/fixnt_Font_%d", tmpdir, pid);
  font = fopen(fontfname,"w+");
  fprintf(font,"%s",yytext);
  BEGIN FONT;
}

<FONT,BODY>%%BeginFont:.*\n { /* Font definitions go to FILE* font */
  fprintf(font,"%s",yytext);
  BEGIN FONT;
}

<FONT>%%EndFont.*\n { /* End of Font Definition */
  fprintf(font,"%s",yytext);
  BEGIN BODY;
}

<FONT,BODY>%%BeginResource:[ ]font.*\n { ;
/* NT 3.5 Postscript Files have different FontDefinitions */
 fprintf(font,"%s",yytext);
 BEGIN FONT;
}

<FONT>%%EndResource.*\n { ;
/* NT 3.5 Postscript Files have different FontDefinitions */
 fprintf(font,"%s",yytext);
 BEGIN BODY;
}

<FONT>.*\n { /* goes all to FILE *font ... */
  fprintf(font,"%s",yytext);
}

<BODY>(NTPSOct95[ ]){0,1}[/]PageSV[ ]save[ ](put|def).*\n { ;
  ifpagesv = 1;
}

<BODY>Adobe_WinNT_Driver_Gfx[ ]dup[ ][/]terminate[ ]get[ ]exec.*\nFontSV[ ]restore.*\nPageSV[ ]restore.*\n%%Trailer.*\n {
  fprintf(body,"%%%%Trailer\nAdobe_WinNT_Driver_Gfx dup /terminate get exec\nPageSV restore\nFontSV restore\n");
}

<BODY>FontSV[ ]restore.*\nPageSV[ ]restore.*\n    { ;
  /* switch FontSV/PageSV */
  fprintf(body,"PageSV restore\nFontSV restore\n");
}

<BODY>PageSV[ ]restore.*\n%%Trailer.*\n    { ;
/* put PageSV after Trailer */
  fprintf(body,"%%%%Trailer\nPageSV restore\n");
}


<BODY>FontSV[ ]restore.*\nPageSV[ ]restore.*\n%%Trailer.*\n    { ;
/* put PageSV/FontSV after Trailer */
  fprintf(body,"%%%%Trailer\nPageSV restore\nFontSV restore\n");
}


<BODY>PageSV[ ]restore.*\n { ;
/* do not allow any PageSV's to appear within the actual body */
}


<BODY>%%EOF.*\n { ;
/* What to do at the end of the file (either recognized by %%EOF or
   reg. end of file */
 fprintf(body,"%s",yytext);
 return 0;
}

<BODY><<EOF>> { ;
/* This section is obsolete, could be removed ..., assume there is no
   string <<EOF>> in a Postscript file ... */

/* Only flex knows about the <<EOF>> symbol therefore put the
   reassembling process into the main program. Plain lex does return 0
   for regular end of file marks. */
 return 0;
}

<BODY>.*\n { /* put pages in FILE *body */
  fprintf(body,"%s",yytext);
}

%%
int
yywrap ()
{
  return 1;
}

int
main ()
{
#ifdef _WIN32
  GetTempPath(_MAX_PATH, tmpdir);
#else
  if (!(tmpdir = getenv("TMPDIR")))
    tmpdir = "/tmp";
#endif
  pid = (int) getpid ();
  yylex();
  /* at the end always reassemble the file and clean up ... */
  reassemble(adobe, font, body, adobefname, fontfname, bodyfname);

  return 0;
}

void
reassemble (adobe, font, body, adobefname, fontfname, bodyfname)
  FILE* adobe;
  FILE* font;
  FILE* body;
  char *adobefname;
  char *fontfname;
  char *bodyfname;
{
  int xx;

  /* put everything in right order ... */
  if( adobe != NULL ){
    rewind(adobe);
    while( (xx = getc( adobe )) != EOF ){
      putc(xx,yyout);
    }
    fclose(adobe);
    remove(adobefname);
  }

  if( font != NULL){
    rewind(font);
    while( (xx = getc( font )) != EOF ){
      putc(xx,yyout);
    }
    fclose(font);
    remove(fontfname);
  }

  if( ifendsetup == 1){
    fprintf(yyout,"%%%%EndSetup:\n");
  }

  if( body != NULL){
    rewind(body);
    if(ifpagesv == 1)
      fprintf(yyout,"NTPSOct95 /PageSV save put\n");
    while( (xx = getc( body )) != EOF ){
      putc(xx,yyout);
    }
    fclose(body);
    remove(bodyfname);
  }
  return;
}

/*
  Local Variables:
  Mode: C
  End:
*/
