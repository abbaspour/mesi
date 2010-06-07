/* lex specification for Pascal subset */

%{
#include <string.h>
  static int line_no = 1;
%}

ID [A-Za-z][A-Za-z0-9]*
BC /\*
EC \*/

%%

"begin"  { return tBEGIN; }
"end"    { return tEND; }
"if"     { return tIF; }
"then"   { return tTHEN; }
"else"   { return tELSE; } 
":="     { return tASSIGN; }
"goto"   { return tGOTO; }
"not"    { return tNOT; }
"while"  { return tWHILE; }
"do"     { return tDO; }
"for"    { return tFOR; }
"to"     { return tTO; }
"downto" { return tDOWNTO; }
":"	 { return tCOLON; }
","	 { return tSEP; }
";"      { return tSCOLON; }
"int"    { yylval.idval = strdup(yytext); return tINT; }
"char"   { yylval.idval = strdup(yytext); return tCHAR; }
"var"	 { return tVAR; }
"+"      { return tPLUS; }
"-"      { return tMINUS;}
"*"      { return tSTAR; }
"/"      { return tDIV; }
"<"      { return tLESS; }
">"      { return tMORE; }
"("      { return tOP; }
")"      { return tCP; }
"."      { return tDOT; }

{ID}     { yylval.idval = strdup(yytext); return tIDENTIFIER; }
[0-9]+   { yylval.numval = atoi(yytext); return tNUMBER; }
[ \t]  /* ignore whitespace */
[{BC}.*{EC}] 
.        { return *yytext; }
\n       { line_no++; } 
%%
