%{
#include <stdio.h>
#include "tree.h"
%}

%union {
  int	numval;
  char	*idval;
  tree  *tval;
}

%token tBEGIN
%token tEND
%token tIF
%token tTHEN
%token tELSE
%token tASSIGN
%token tGOTO
%token tNOT
%token tWHILE
%token tDO
%token tFOR
%token tTO
%token tDOWNTO
%token tCOLON
%token tSCOLON
%token tINT
%token tCHAR
%token tVAR
%token tSEP
%token tPLUS
%token tMINUS
%token tDIV
%token tLESS
%token tMORE
%token tSTAR
%token tOP
%token tCP
%token tDOT

%token <idval> tIDENTIFIER tINT tCHAR 
%token <numval> tNUMBER
%type  <tval> program prog variables vardefs vardef varname vartype statements statement assignexpr ifexpr gotoexpr whileexpr forexpr expression factor term condition subexpr paramlist

%start program
%left tLESS
%left tPLUS  tMINUS
%left tSTAR  tDIV

%%

program: prog { print_tree($1); genlabel ($1); init_draw_tree(); draw_tree($1);  finish_draw_tree();  init_gen_code(); gencode_main($1); finish_gen_code();}
;

prog: variables tBEGIN statements tEND tDOT { $$ = mknode(t_program, 0, $1, $3); }
;

variables: tVAR vardefs { $$ = $2; }
| { $$ = mknode(t_null, 0, NULL, NULL); }
;

vardefs : vardef vardefs { $$ = mknode(t_join, 0, $1, $2); }
        | vardef { $$ = $1; }
;

vardef : varname tCOLON vartype tSCOLON { $$ = mknode(t_def, 0, $1, $3); } 
;

varname : tIDENTIFIER tSEP varname { $$ = mknode(t_join, 0, mkid($1), $3); }
   | tIDENTIFIER { $$ = mkid($1); }
;

vartype: tINT { $$ = mkid($1); }
| tCHAR { $$ = mkid($1); }
;

/*label part = tIDENTIFIER tCOLON*/

statements:  statement { $$ = $1; }
          | statement tSCOLON  { $$ = $1; }
          | statement tSCOLON  statements { $$ = mknode(t_join, 0, $1, $3); }
          | tIDENTIFIER tCOLON statements { $$ = mknode(t_join, LABEL_JOIN, mkid($1), $3); }
;

statement : assignexpr  { $$ = $1; }
          | ifexpr  { $$ = $1; }
          | gotoexpr  { $$ = $1; }
          | subexpr { $$ = $1; }
	  | whileexpr { $$ = $1; }
	  | forexpr { $$ = $1; }
          | tBEGIN statements tEND { $$ = mknode(t_block, 0, $2, NULL); }
          | tBEGIN tEND { $$ = mknode(t_null, 0, NULL, NULL); }
;

assignexpr: tIDENTIFIER tASSIGN expression   { $$ = mknode(t_assign, 0, mkid($1), $3); }
;

ifexpr: tIF condition tTHEN statement { $$ = mknode(t_if, 0, $2, mknode(t_else, 0, $4, NULL)); }
   | tIF condition tTHEN statement tELSE statement { $$ = mknode(t_if, 0, $2, mknode(t_else, 0, $4, $6)); }
;

condition: expression { $$ = $1; }
| expression tLESS expression { $$ = mknode(t_op, '<', $1, $3); }
| expression tMORE expression { $$ = mknode(t_op, '>', $1, $3); }
| tOP condition tCP { $$ = $2; }
;

gotoexpr:tGOTO tIDENTIFIER { $$ = mknode(t_goto, 0, mkid($2), NULL); }		 
;

subexpr: tIDENTIFIER tOP paramlist tCP { $$ = mknode(t_sub, 0, mkid($1), $3); }
;

whileexpr: tWHILE condition tDO statement  { $$ = mknode(t_while, 0, $2, $4); }
;

forexpr: tFOR assignexpr tTO expression tDO statement  { $$ = mknode(t_for, 1, mknode(t_join, 0, $2, $4), $6); }
| tFOR assignexpr tDOWNTO expression tDO statement  { $$ = mknode(t_for, -1, mknode(t_join, 0, $2, $4), $6); }
;
	 
expression: expression tPLUS term { $$ = mknode(t_op, '+', $1, $3); }
           | expression tMINUS term { $$ = mknode(t_op, '-', $1, $3); }
           | term { $$ = $1; }
;

term:  term tSTAR factor { $$ = mknode(t_op, '*', $1, $3); }
         | term tDIV factor { $$ = mknode(t_op, '/', $1, $3); }
         | factor { $$ = $1; }
;

factor:  tNUMBER { $$ = mknum($1); }
           | tIDENTIFIER { $$ = mkid($1); }
           | tNOT factor { $$ = mknode(t_op, '!', $2, NULL); }
           | tOP expression tCP { $$ = $2; /*mknode(t_block, 0, $2, NULL);*/ }
;

paramlist: expression { $$ = mknode(t_param, 0, $1, NULL); }
         | paramlist tSEP expression { $$ = mknode( t_param, 0, $3, $1); }
         | { $$ = NULL; }
;

%%

int main()
{
  yyparse();
}

#include "lex.yy.c"

int yyerror(char *s)
{
  fprintf(stderr, "%s[%s] at line %d\n", s, yytext, line_no);
  return 0;
}

