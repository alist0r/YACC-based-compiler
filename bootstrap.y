%{
#include "symbol-table.h"
#include "regs.h"
#include "lex.yy.c"
void yyerror(const char *s) {
	fprintf (stderr, "%s\n", s);
	printf("line: %d col: %d\n", lineno, colno);
	printf("last lex: '%s'\n", yytext);
	exit(1);
}
struct table *TABLE = NULL;
struct reg_stack *REGS = NULL;
int yylex();
int yywrap();
%}
%code requires {struct integer_lit {
        int type;
        int line;
        int col;
        int val;
};

struct string_lit {
        int type;
        int line;
        int col;
        char *str;
};

struct label {
        int type;
        int line;
        int col;
        char *str;
};

struct register_ {
	bool is_reg;
	enum regs reg;
};

struct memory {
	bool is_reg;
	bool is_global;
	int offset;
};

union memory_location {
	struct register_ reg;
	struct memory mem;
};

}
%union {
	struct integer_lit int_lit;
	struct string_lit str_lit;
	struct label lab;
	enum symbol_type type;
	union memory_location memloc;
	enum regs reg;
}
%type <lab> Definition
%type <reg> Exp
%type <memloc> Item
%type <memloc> Add
%type <memloc> Mul
%token <str_lit> STRING_LIT 
%token <lab> ID 
%token <int_lit> LITERAL
%token <type> TYPE

%token FOR IF RETURN ELSE ADD SUB MUL DIV RIGHT_CURLY LEFT_CURLY RIGHT_SQUARE LEFT_SQUARE RIGHT_PARENTHESES LEFT_PARENTHESES COMMA SEMICOLON MOV
%%
Program: FunctionDef Program
       | Assignment SEMICOLON Program
       | 
       ;
Definition: TYPE ID {
	if (look_up(TABLE, $2.str) != NULL) {
		yyerror("variable already declaired");
	}
	add_symbol(TABLE, create_symbol($2.str, $1));
	$$ = $2;
};
FunctionDef: Definition LEFT_PARENTHESES DefArgs RIGHT_PARENTHESES LEFT_CURLY {
	   push_table(&TABLE); printf("%s:\n",$1.str);
}Body RIGHT_CURLY{
	pop_table(&TABLE);
}
	   ;
DefArgs: Definition
       | Definition COMMA DefArgs
       |
       ;
Assignment: Definition MOV Exp
	  | ID {
		if (look_up(TABLE, $1.str) == NULL) {
			yyerror("undefined reference");
		}
	    } MOV Exp
          | Definition 
          |
          ;
Body: Assignment SEMICOLON Body
    | Condition Body
    | Loop Body
    | Return Body
    |
    ;
Return: RETURN Exp {printf("\t(ret)\n");}
      ;
Condition: IF LEFT_PARENTHESES Exp RIGHT_PARENTHESES LEFT_CURLY Body RIGHT_CURLY
	 ;
Loop: FOR LEFT_PARENTHESES Assignment SEMICOLON Exp SEMICOLON Assignment RIGHT_PARENTHESES LEFT_CURLY RIGHT_CURLY
    ;
Exp: Add {
	if ($1.reg.reg != ACC) {
		printf("\t(=, ACC, R%d)\n", $1.reg.reg - 1);
		free_reg(REGS, $1.reg.reg);
	}
}
   ;
Add: Add ADD Mul { //TODO if not stored in a register the result of addition needs to be in a register
	printf("\t(+, R%d, R%d)\n", $1.reg.reg -1, $3.reg.reg-1);
	free_reg(REGS, $3.reg.reg);
}
   | Add SUB Mul 
   | Mul
   ;
Mul: Mul MUL Item
   | Mul DIV Item
   | Item
   ;
Item: ID
    | LITERAL {$$.reg.reg = alloc_reg(REGS); $$.reg.is_reg = true; printf("\t(=, R%d, %d)\n", $$.reg.reg - 1, $1.val);}
    ;
%%
int main(int argc, char **argv) {
	push_table(&TABLE);
	REGS = init_reg_stack();
        if (argc == 2) {
                yyin = fopen(argv[1], "r");
        } else {
		printf("useage: a.out <file path>\n");
		exit(1);
	}
	return yyparse();
}
