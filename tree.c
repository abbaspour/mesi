/* Tree function definitions */

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "defines.h"
#include "stack.h"

#define max(x, y) x > y ? x : y

/*static*/ struct Stack *first_stack, *current_stack; // gcc-4.4 no static needed
static int last_temp = 0;

tree *
mkid (char *id)
{
   tree *t = mknode (t_id, 0, NULL, NULL);
   t->value.idval = id;
   t->id = counter++;
   return t;
}

tree *
mknum (int num)
{
   tree *t = mknode (t_num, 0, NULL, NULL);
   t->value.numval = num;
   t->id = counter++;
   return t;
}

tree *
mknode (treetype type, int op, tree * l, tree * r)
{
   tree *t = (tree *) malloc (sizeof (tree));
   t->type = type;
   t->op = op;
   t->left = l;
   t->right = r;
   t->id = counter++;
   return t;
}

void
print_tree (tree * t)
{
   if (!t)
      return;
   switch (t->type)
   {
      case t_block:
	 printf ("{\n");
	 print_tree (t->left);
	 printf ("}\n");
	 break;

      case t_join:
	 print_tree (t->left);
	 if (t->right)
	    print_tree (t->right);
	 break;

      case t_if:
	 printf ("if( ");
	 print_tree (t->left);
	 printf (")\n");
	 t = t->right;
	 print_tree (t->left);
	 if (t->right)
	 {
	    printf ("else\n");
	    print_tree (t->right);
	 }
	 break;

      case t_assign:
	 print_tree (t->left);
	 printf (" = ");
	 print_tree (t->right);
	 printf (";\n");
	 break;

      case t_while:
	 printf ("while( ");
	 print_tree (t->left);
	 printf (") do\n ");
	 print_tree (t->right);
	 printf (";\n");
	 break;

      case t_for:
	 printf ("for ");
	 print_tree (t->left->left);
	 if(t->op == 1) 
	   printf (" to ");
	 else
	   printf (" downto ");
	 print_tree (t->left->right);
	 printf (" do\n ");
	 print_tree (t->right);
	 printf (";\n");
	 break;

      case t_goto:
	 printf (" goto %s\n", t->left->value.idval);
	 break;

      case t_program:
	 printf ("program\n");
	 if (t->left)
	 {
	    printf ("var\n");
	    print_tree (t->left);
	 }
	 printf ("begin\n");
	 print_tree (t->right);
	 printf ("end.\n");
	 break;

      case t_def:
	 print_tree (t->left);
	 printf (" : %s \n", t->right->value.idval);
	 break;

      case t_null:
	 printf ("\n");
	 break;

      case t_op:
	 printf ("(");
	 print_tree (t->left);
	 printf (" %c ", t->op);
	 print_tree (t->right);
	 printf (")");
	 break;

      case t_num:
	 printf (" %d ", t->value.numval);
	 break;

      case t_id:
	 printf (" %s ", t->value.idval);
	 break;
   }
}

void
init_draw_tree ()
{
   out = fopen ("tree.dot", "w+");
   if (out == NULL)
   {
      fprintf (stderr, "Cannot open output file.\n");
      exit (0);
   }
   counter = 0;
   fprintf (out, "/* Output from mesi compiler */\n");
   fprintf (out, "digraph Mesi {\n");
   fprintf (out, "node[/*style=filled, color=gray92,*/ shape=box];\n");
   printf("Finished init draw tree\n");
   fflush(stdout);
}

void
finish_draw_tree ()
{
   fprintf (out, "}\n");
   fclose (out);
   printf("Finished finish draw tree\n");
   fflush(stdout);
}

void
draw_tree (tree * t)
{
   if (!t)
      return;
   switch (t->type)
   {
      case t_program:
	 fprintf (out, "l%d[label=\"program (%d)\"];\n", t->id, t->label);
	 if (t->left)
	 {
	    fprintf (out, "def[label=\"definition (%d)\", fontcolor=%s];\n", t->left->label, COLOR_MAIN);
	    draw_tree (t->left);
	 }
	 fprintf (out, "l%d->def[style=dotted];\n", t->id);
	 fprintf (out, "def->l%d[style=dotted];\n", t->left->id);

	 fprintf (out, "body[label=\"body (%d)\", fontcolor=%s];\n", t->right->label, COLOR_MAIN);
	 draw_tree (t->right);
	 fprintf (out, "l%d->body;\n", t->id);
	 fprintf (out, "body->l%d;\n", t->right->id);
	 break;

      case t_block:
	 fprintf (out, "l%d[label=\"block (%d)\"];\n", t->id, t->label);
	 draw_tree (t->left);
	 fprintf (out, "l%d->l%d;\n", t->id, t->left->id);
	 break;

      case t_join:
	 if (t->op == LABEL_JOIN)
	 {
	    fprintf (out, "l%d[label=\"label (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_GOTO_LBL);
	    fprintf (out, "l%d[label=\"%s (%d)\",fontcolor=%s];\n", t->left->id, t->left->value.idval, t->left->label, COLOR_LABEL);
	    fprintf (out, "l%d->l%d;\n", t->id, t->left->id);
	 }
	 else
	 {
	    fprintf (out, "l%d[label=\"join (%d)\" ,fontcolor=%s];\n", t->id, t->label, COLOR_JOIN);
	    draw_tree (t->left);
	    if (isleaf(t->left) || (t->left->type == t_def))
	       fprintf (out, "l%d->l%d[style=dotted];\n", t->id, t->left->id);
	    else
	       fprintf (out, "l%d->l%d;\n", t->id, t->left->id);
	 }
	 if (t->right)
	 {
	    draw_tree (t->right);
	    if (isleaf(t->right) || (t->right->type == t_def) || (t->right->left->type == t_def))
	       fprintf (out, "l%d->l%d[style=dotted];\n", t->id, t->right->id);
	    else
	       fprintf (out, "l%d->l%d;\n", t->id, t->right->id);
	 }
	 break;

      case t_if:
	 fprintf (out, "l%d[label=\"if (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_IF_THEN_ELSE);
	 draw_tree (t->left);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->left->id);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->right->id);
	 t = t->right;
	 fprintf (out, "l%d[label=\"then/else (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_IF_THEN_ELSE);
	 draw_tree (t->left);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->left->id);
	 if (t->right)
	 {
	    draw_tree (t->right);
	    fprintf (out, "l%d -> l%d;\n", t->id, t->right->id);
	 }
	 else
	 {
	    fprintf (out, "l%d_null[label=\"null (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_NULL);
	    fprintf (out, "l%d -> l%d_null;\n", t->id, t->id);
	 }
	 break;

      case t_goto:
	 fprintf (out, "l%d[label=\"goto (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_GOTO_LBL);
	 fprintf (out, "l%d[label=\"%s (%d)\",fontcolor=%s];\n", t->left->id, t->left->value.idval, t->label , COLOR_LABEL);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->left->id);
	 break;

      case t_while:
	 fprintf (out, "l%d[label=\"while (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_WHILE);
	 draw_tree (t->left);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->left->id);
	 draw_tree (t->right);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->right->id);
	 break;

      case t_for:
	 fprintf (out, "l%d[label=\"for (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_FOR);
	 draw_tree (t->left);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->left->id);
	 draw_tree (t->right);
	 fprintf (out, "l%d -> l%d;\n", t->id, t->right->id);
	 break;

      case t_sub:
	 fprintf(out, "l%d[label=\"%s (%d)\", fontcolor=\"%s\"];\n", t->id, t->left->value.idval, t->left->label, COLOR_SUB);
	 draw_tree(t->right);
	 fprintf(out, "l%d->l%d;\n", t->id, t->right->id);
	 break;

      case t_param:
	 fprintf(out, "l%d[label=\", (%d)\", fontcolor=\"%s\"];\n", t->id, t->label, COLOR_PARAM);
	 draw_tree(t->left);
	 fprintf(out, "l%d->l%d;\n", t->id, t->left->id);
	 if(t->right) {
	    draw_tree(t->right);
	    fprintf(out, "l%d->l%d;\n", t->id, t->right->id);
	 }
	 break;

      case t_def:
	 fprintf (out, "l%d[label=\"as type (%d)\",fontcolor=%s];\n", t->id, t->label, COLOR_DEF);
	 draw_tree (t->left);
	 draw_tree (t->right);
	 fprintf (out, "l%d->l%d[style=dotted];\n", t->id, t->left->id);
	 fprintf (out, "l%d->l%d[style=dotted, color=%s];\n", t->id, t->right->id, COLOR_TYPE_LINK);
	 break;

     case t_null:
	 fprintf (out, "l%d[label=\"NULL (%d)\",fontcolor=%s];\n", t->id, t->label,COLOR_NULL);
	 break;

      case t_assign:
	 fprintf (out, "l%d[label=\":= (%d)\", fontcolor=%s];\n", t->id, t->label, COLOR_ASSIGN);
	 draw_tree (t->left);
	 draw_tree (t->right);
	 fprintf (out, "l%d->l%d;\n", t->id, t->left->id);
	 fprintf (out, "l%d->l%d;\n", t->id, t->right->id);
	 break;

      case t_op:
	 fprintf (out, "l%d[label=\"%c (%d)\",fontcolor=%s]\n", t->id, t->op, t->label, COLOR_OP);
	 draw_tree (t->left);
	 draw_tree (t->right);
	 fprintf (out, "l%d->l%d;\n", t->id, t->left->id);
	 fprintf (out, "l%d->l%d;\n", t->id, t->right->id);
	 break;

      case t_num:
	 fprintf (out, "l%d[label=\"%d (%d)\",fontcolor=%s];\n", t->id, t->value.numval, t->label, COLOR_NUM);
	 break;

      case t_id:
	 fprintf (out, "l%d[label=\"%s (%d)\",fontcolor=%s];\n", t->id, t->value.idval, t->label, COLOR_ID);
	 break;

      default:
	 break;
   }
}

void
init_gen_code ()
{
   int i = 0;

   out = fopen ("code.3a", "w+");
   if (out == NULL)
   {
      fprintf (stderr, "Cannot open output file.\n");
      exit (0);
   }
   counter = 0;
   lab_count = 0;

   init_stack();

   st = st_start = (struct SymbolTable *) malloc (sizeof (struct SymbolTable));
}

void
finish_gen_code ()
{
   fclose (out);
   printf("Finished finish_gen_code.\n");
   fflush(stdout);
}

char* name(tree *n)
{
   char *vn = (char*)malloc(sizeof(char) * MAX_VAR_LEN);	/* variable name */
   sprintf(vn, "null");
   if(n->type == t_id)
      sprintf(vn, "%s", n->value.idval);
   else if(n->type == t_num)
      sprintf(vn, "%d", n->value.numval);
   return vn;
}

char * operation(tree *n)
{
   char *on = (char*)malloc(sizeof(char) * MAX_OP_LEN);	/* operation name */
   switch(n->type)
   {
      case t_if:
	 sprintf(on, "if");
	 return on;

      case t_else:
	 sprintf(on, "else");
	 return on;

      case t_goto:
	 sprintf(on, "goto");
	 return on;

      case t_assign:
	 sprintf(on, ":=");
	 return on;

      case t_op:
	 sprintf(on, "%c", n->op);
	 return on;
   }
   return NULL;
}

char * neg_operation(tree *n)
{
   char *on = (char*)malloc(sizeof(char) * MAX_OP_LEN);	/* operation name */
   switch(n->op) {
      case '=' :
	 sprintf(on, "!=");
	 return on;

      case '<' :
	 sprintf(on, ">");
	 return on;

      case '>' :
	 sprintf(on, "<");
	 return on;
	 
      default:
	 sprintf(on, "?");
	 return on;
   }
   return NULL;
}

void
gencode_main (tree * n)
{
   gensymtab(n->left);
   gencode(n->right);
   print_st();
}

void print_st()
{
   struct SymbolTable *st = st_start->next;
   while(st)
   {
      printf("Found (%s) as type (%s) at offset(%d)\n", st->name, tsize[st->type].name, st->offset);
      st = st->next;
   }
}

void
gensymtab(tree *n)
{
   if(n == NULL)
      return;

   switch(n->type)
   {
      case t_def:
	 gensymtab_worker(n->right->value.idval, n->left);
	 break;

      case t_join:
	 gensymtab(n->left);
	 gensymtab(n->right);
	 break;
   }
}

void gensymtab_worker(char *t, tree *n)
{
   if(n->type == t_id)
   {
      fprintf(out, "def\t%s\t%s\n", t, n->value.idval);
      add2(n->value.idval, t);
      return;
   }
   gensymtab_worker(t, n->left);
   gensymtab_worker(t, n->right);
}

void checktype(tree *n)
{
   if(!isleaf(n))
      return;

   if(n->type == t_id && !lookup(name(n)))
   {
      fprintf(stderr, "Unknown variable(%s)\n", name(n));
      exit(UNKNOWN_VAR_ERR);
   }
}

int istemp(char *name) 
{
   if(name[0]=='_' && name[1] == 't')
      return 1;
   return 0;
}

void
gencode (tree * n)
{
   // custion : only value that are not used after calling another gencode
   // cuz these values are not re-entrent

   static int param_count = 0; // ok
   struct SymbolTable *st;
   char *t1, *t2, new_tmp[MAX_VAR_NAME];

   if (n == NULL)
      return;
   if (isleaf (n))
   {
      checktype(n);
      if(istemp(name(n)))
	 push_temp(name(n));
      else
	 push(name(n));
      return;
   }

   switch(n->type)
   {
      case t_assign:
	 if(isleaf(n->right)) 
	 {
	    fprintf(out, "%s\t:=\t%s\n", name(n->left), name(n->right));
	    break;
	 }
	
	 gencode(n->right->left);
	 gencode(n->right->right);

	 fprintf(out, "%s\t:=\t%s\t%s\t%s\n", name(n->left), pop(), operation(n->right), pop());
	 break;
	
      case t_op:
	 gencode(n->right);
	 gencode(n->left);

	 if(top()->temp)
	 {
	    t1 = pop();
	    t2 = pop();
	    fprintf(out, "%s\t:=\t%s\t%s\t%s\n", t1, t1, operation(n), t2);	
	    push_temp(t1);
	    break;
	 }
	 t1 = pop();
	 t2 = pop();
	 sprintf(new_tmp, "_t%d", last_temp++);
	 fprintf(out, "%s\t:=\t%s\t%s\t%s\n", new_tmp, t1, operation(n), t2);	
	 push_temp(new_tmp);
	
	 break;

      case t_goto:
	 st = lookup(name(n->left));
	 if(st && (gettype(name(n->left)) == LABEL))
	    fprintf(out, "%s\t%s\n", operation(n), name(n->left));
	 else {
	    add(name(n->left), LABEL);
	    st = lookup(name(n->left));
	    st->ok = 0;		// waitting to see label defination
	    fprintf(stderr, "Warning: waitting to see label(%s)\n", name(n->left));
	 }
	 break;

      case t_while: {
	 int  Lbegin = newlabel();
	 int  Lend = newlabel();
	 fprintf(out, "_l%d\t:\n", Lbegin);
	 
	 if(isleaf(n->left)) {
	    fprintf(out, "if\t%s\t=\t0\tgoto\t_l%d\n", name(n->left), Lend);
	 } else {		/* TODO : it SHLULD be relop but we dont check */
	    gencode(n->left->left);
	    gencode(n->left->right);
	    
	    fprintf(out, "if\t%s\t%s\t%s\tgoto\t_l%d\n", pop(), neg_operation(n->left), pop(), Lend);
	 }
	 gencode(n->right);
	 fprintf(out, "goto\t_l%d\n", Lbegin);
	 fprintf(out, "_l%d\t:\n", Lend);
	 break;
      }

      case t_for: 
	 fprintf(stderr, "No impl for FOR yet!\n");
	 break;

      case t_sub:
	 gencode(n->right);
	 fprintf(out, "call\t%s\t%d\n", n->left->value.idval, param_count);
	 param_count = 0;
	 break;

      case t_param:
	 gencode(n->left);
	 fprintf(out, "param\t%s\n", pop());
	 param_count++;
	 if(n->right) 
	    gencode(n->right);
	 break;

      case t_if:
      {
	 int L = newlabel();
	 if(isleaf(n->left)) {
	   fprintf(out, "if\t%s\t=\t0\tgoto\t_l%d\n", name(n->left), L);
	 } else {		/* TODO : it SHLULD be relop but we dont check */
	   gencode(n->left->left);
	   gencode(n->left->right);
	   
	   fprintf(out, "if\t%s\t%s\t%s\tgoto\t_l%d\n", pop(), operation(n->left), pop(), L);
	 }
	 gencode(n->right->left);

	 // else
	 if(n->right->right != NULL)
	 {
	    int L2 = newlabel();
	    fprintf(out, "goto\t_l%d\n", L2);
	    fprintf(out, "_l%d\t:\n", L);
	    gencode(n->right->right);
	    fprintf(out, "_l%d\t:\n", L2);
	 } else {
	    fprintf(out, "_l%d\t:\n", L);
	 }
	 break;
      }

      case t_join:
	 if(n->op == LABEL_JOIN)
	 {
	    add(n->left->value.idval, LABEL);
	    fprintf(out, "%s\t:\n", n->left->value.idval);
	 }
	 else
	    gencode(n->left);
	 gencode(n->right);
	 break;

      case t_block:
	 gencode(n->left);
	 break;
   }
   // switch
}

void
genlabel (tree * n)
{
   if (n == NULL)
      return;
   if (!isleaf (n))
   {
      if (isleaf (n->left))
	 n->left->label = 1;
      else
	 genlabel (n->left);

      if (isleaf (n->right))
	 n->right->label = 0;
      else
	 genlabel (n->right);

      if(n->right == NULL)
	 n->label = n->left->label;
      else
	 if (n->right->label != n->left->label)
	    n->label = max (n->right->label, n->left->label);
	 else
	    n->label = n->right->label + 1;
   }
}

int isleaf (tree * n)
{
   if (n == NULL)
      return 0;
   if (n->type == t_id || n->type == t_num || n->type == t_null)
      return 1;
   return 0;
}

int newlabel()
{
   return lab_count++;
}
