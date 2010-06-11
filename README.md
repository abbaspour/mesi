Simple Academic Pascal-like Syntax Compiler to 3 Address Machine Code
=====================================================================

Intro
-----
This program is a very simple compiler which accepts a Pascal like syntax and generates a 3 address assembly code along with a compile structure graph. 

In fact this is a tool for generating input sample for another experimental work which is a global optimizer.

Please note that this is *not intended* to be a fully operational compiler.

Requirement
----------
* bison
* flex
* gcc
* make
* graphviz (optional)
* doxygen (optional)

Steps to run:
-------------
	make 
	./mesi < samples/hard.msi 

Now, you have "code.3a" which is 3 address code, and "tree.dot" which is the tree structure, with graphwiz syntax. If you wanna see it

	dot -Tps tree.dot

Now you have tree.ps. If you wanna jpeg output just use -Tjpeg.

Example Output
--------------

This is input code:
	
	var
	a,b:int;
	c,d:char;
	e:int;
	f:int;
	h:char;
	g:char;
	
	begin
	a := 2;
	c := d * (3+6) - f;
	lab:
	        if a then
        	        c := d * e 
	        else
        	        if d then goto lab;
	lab2:
	if c then
	        d:=5
        	else goto lab2;
	end.

This is generated 3 address assembly code:

	def     int     a
	def     int     b
	def     char    c
	def     char    d
	def     int     e
	def     int     f
	def     char    h
	def     char    g
	a       :=      2
	_t0     :=      3       +       6
	_t1     :=      d       *       _t0
	c       :=      _t1     -       f
	lab     :
	if      a       =       0       goto    _l0
	c       :=      d       *       e
	goto    _l1
	_l0     :
	if      d       =       0       goto    _l2
	goto    lab
	_l2     :
	_l1     :
	lab2    :
	if      c       =       0       goto    _l3
	d       :=      5
	goto    _l4
	_l3     :
	goto    lab2
	_l4     :

And this is graph:
![Sample Output Graph](http://aminsblog.files.wordpress.com/2010/06/tree.jpeg?w=477&h=287 "Sample Output Graph")


