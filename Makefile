CC = gcc -g -w
YACC = yacc -y
LIBS = -lfl
LEX = flex 

OBJS = tree.o symbol.o  mesi.o

all:	mesil syntax mesi

mesil: mesi.l
	$(LEX) mesi.l

syntax: mesi.y
	$(YACC) mesi.y

mesi:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:	
	rm -f *.o mesi y.tab.c lex.yy.c *.ps *.aux *.dvi *.log *.tex *.ftx *~ y.output *.dot *.3a README.html

TAGS: 
	etags *.c *.h

doxygen: Doxyfile
	doxygen
