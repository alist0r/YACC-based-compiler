make:
	yacc -vd bootstrap.y
	lex bootstrap.l
	gcc -Wall -lfl -g y.tab.c symbol-table.c regs.c
