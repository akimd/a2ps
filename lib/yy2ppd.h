/*
 * Replace the parser/lexer prefix yy into ppd
 */

#define	yymaxdepth ppdmaxdepth
#define	yyparse	ppdparse
#define	yylex	ppdlex
#define	yyerror	ppderror
#define	yylval	ppdlval
#define	yychar	ppdchar
#define	yydebug	ppddebug
#define	yypact	ppdpact
#define	yyr1	ppdr1
#define	yyr2	ppdr2
#define	yydef	ppddef
#define	yychk	ppdchk
#define	yypgo	ppdpgo
#define	yyact	ppdact
#define	yyexca	ppdexca
#define yyerrflag ppderrflag
#define yynerrs	ppdnerrs
#define	yyps	ppdps
#define	yypv	ppdpv
#define	yys	ppds
#define	yy_yys	ppdyys
#define	yystate	ppdstate
#define	yytmp	ppdtmp
#define	yyv	ppdv
#define	yy_yyv	ppdyyv
#define	yyval	ppdval
#define	yylloc	ppdlloc
#define yyreds	ppdreds
#define yytoks	ppdtoks
#define yylhs	ppdyylhs
#define yylen	ppdyylen
#define yydefred ppdyydefred
#define yydgoto	ppdyydgoto
#define yysindex ppdyysindex
#define yyrindex ppdyyrindex
#define yygindex ppdyygindex
#define yytable	 ppdyytable
#define yycheck	 ppdyycheck
#define yyname   ppdyyname
#define yyrule   ppdyyrule
