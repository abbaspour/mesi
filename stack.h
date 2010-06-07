#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>

#define MAX_VAR_NAME 20

struct Stack {
   char var[MAX_VAR_NAME];
   int temp;
   struct Stack *next;
   struct Stack *prev;   
};
extern struct Stack *first_stack, *current_stack;

static void push(char *name) {
   struct Stack *s = (struct Stack*) malloc(sizeof(struct Stack));
   strcpy(s->var, name);
   s->temp = 0;
   s->next = NULL;
   
   current_stack -> next = s;
   s->prev = current_stack;
   current_stack = s;
};

static void push_temp(char *name) {
   struct Stack *s = (struct Stack*) malloc(sizeof(struct Stack));
   strcpy(s->var, name);
   s->temp = 1;
   s->next = NULL;
   
   current_stack -> next = s;
   s->prev = current_stack;
   current_stack = s;
};

static struct Stack* top() {
   return current_stack;
};

static char* pop() {
   /* No free()!*/
   struct Stack *c = current_stack;
   char *vn = (char*)malloc(sizeof(char) * MAX_VAR_LEN);	/* variable name */
   
   strcpy(vn, c->var);
   
   if(c->prev) 
     {
	current_stack = c->prev;
	free(c);
     }
   
   return vn;
};

static void init_stack() {
   first_stack = (struct Stack *) malloc(sizeof(struct Stack));
   first_stack -> next = NULL;
   first_stack -> prev = NULL;
   
   current_stack = first_stack;
};

#endif // __STACK_H__
