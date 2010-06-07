#ifndef __TREE_H__
#define __TREE_H__

/* tree.h - definitions for syntax tree */
#include <stdio.h>
#include "symbol.h"

#define LABEL_JOIN 	1
#define MAX_REGISTERS 	1
#define MAX_TEMPS 	10
#define MAX_OP_LEN   	3
#define COLOR_LEN 	10

#define COLOR_ID "darkgreen"
#define COLOR_NUM "deepskyblue4"
#define COLOR_OP "deeppink"
#define COLOR_ASSIGN "darkviolet"
#define COLOR_LABEL "khaki4"
#define COLOR_WHILE "darkblue"
#define COLOR_FOR "darkblue"
#define COLOR_NULL "gray50"
#define COLOR_GOTO_LBL "burlywood4"
#define COLOR_IF_THEN_ELSE "darkorange2"
#define COLOR_DEF "yellowgreen"
#define COLOR_MAIN "gold"
#define COLOR_JOIN "skyblue2"
#define COLOR_TYPE_LINK "red"
#define COLOR_SUB "blue"
#define COLOR_PARAM "darkblue"

/* Tree types */
typedef enum {
		 t_block, /* For statements in a block */  
		 t_join,  /* For statements within a block */  
                 t_if,    /* For if statements */
                 t_else,  /* For else parts of if statements */
		 t_goto,  /* For goto statements */
		 t_while, /* For while statements */
		 t_for,   /* For for statements */
		 t_sub,	  /* Fore subroutine call statements */
		 t_param, /* holds subroutine parameters */
		 t_program, /* holds var def and statements */
                 t_assign,/* For assignments */
		 t_def,   /* for var statements */
		 t_null,  /* used for empty rules */
		 t_op,    /* For expressions with an operator */
		 t_num,   /* For numbers */
		 t_id    /* For identifiers */
} treetype;

typedef struct t {
  treetype type;
  int id;
  int op;
  int label;
  union {
    int numval;
    char *idval;
  } value;
  struct t *left;
  struct t *right;
} tree;

tree *mknum(int);
tree *mkid(char *);
tree *mknode(treetype, int, tree *, tree *);
void draw_tree(tree*);
void init_draw_tree();
void finish_draw_tree();
void print_tree(tree*);
void init_gen_code();
void finish_gen_code();
void gen_code(tree*);
void gencode(tree*);
void gensymtab(tree*);
void print_st();
void gensymtab_worker(char *, tree*);
void genlabel(tree*);
int isleaf(tree *);

static FILE *out;
static int counter;
static int lab_count;
struct SymbolTable *st_start;
struct SymbolTable *st;

#endif //__TREE_H__
