#ifndef __SYMBOL_H__
#define __SYMBOL_H__
#include <stdio.h>

#define MAX_VAR_LEN 32
#define MAX_TYPE_LEN 20
#define MAX_TYPE 3

enum Types {
  INT = 0, CHAR , LABEL
};

#define INT_SIZE 2
#define CHAR_SIZE 1
#define LABEL_SIZE  0

struct type_size {
  char name[MAX_TYPE_LEN];
  int size;
  int type;
};

static struct type_size tsize[MAX_TYPE] = {{"int", INT_SIZE, INT}, {"char", CHAR_SIZE, CHAR}, {"label", LABEL_SIZE, LABEL} };

struct SymbolTable {
  char name[MAX_VAR_LEN];
  int  type;
  int  offset;
  short ok;
  struct SymbolTable *next;
};

extern struct SymbolTable *st_start;
static int offsetpos = 1;	/* beginig pos is 1 cuz be supose that 0 means false existance */

struct SymbolTable* lookup(char *name);
int  offset(char *name);
void add(char *name, int type);
void add2(char *name, char *type);
int gettype(char *name);
int nextoffset(int type);

#endif //__SYMBOL_H__
