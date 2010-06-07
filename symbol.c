#include "symbol.h"
#include <string.h>
#include <stdlib.h>

struct SymbolTable* lookup(char *name) {
  struct SymbolTable *st = st_start;
  while(st != NULL) {
    if(!strcmp(st->name, name))
      return st;
    st = st->next;
  }
  return NULL;
}

int  offset(char *name) {
  struct SymbolTable *st = lookup(name);
  if(!st)
    return 0;
  return st->offset;
}

int gettype(char *name) {
  struct SymbolTable *st = lookup(name);
  if(!st)
    return -1;
  return st->type;
}

void add2(char *name, char *type) {
  int i;
  for(i = 0; i < MAX_TYPE; i++)
    if(!strcmp(type, tsize[i].name))
      add(name, tsize[i].type);
  return;
}

void add(char *name, int type) {
  int i;
  struct SymbolTable *st = st_start;
  struct SymbolTable *newst;

  newst = lookup(name);

  if(newst) {
    if(newst->ok)
      fprintf(stderr, "Warning : dublication in variable definition for (%s)\n", name);
    newst->ok = 1;		/* and make it OK */
    return;
  }

  newst = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));

  newst->next = NULL;
  newst->type = type;
  newst->ok = 1;
  sprintf(newst->name, "%s", name);
  newst->offset = nextoffset(type);

  while(st->next != NULL) {
    st = st->next;
  }
  
  st->next = newst;
}

int nextoffset(int type) {
  int p = offsetpos;
  offsetpos += tsize[type].size;
  return p;
}
