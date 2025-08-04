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
int LABEL_COUNTER = 0;
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

struct memory_offset {
	bool is_global;
	int offset;
};

union memory {
	enum regs reg;
	struct memory_offset mem;
};

struct memory_location {
	bool is_reg;
	union memory mem;
};

}
%union {
	struct integer_lit int_lit;
	struct string_lit str_lit;
	struct label lab;
	enum symbol_type type;
	struct memory_location memloc;
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
Assignment: Definition MOV Exp {
		if (look_up(TABLE, $1.str) == NULL) {
			yyerror("undefined reference");
		}printf("\t(=, -%d, ACC)\n", look_up(TABLE, $1.str)->stack_offset);
}
	  | ID {
		if (look_up(TABLE, $1.str) == NULL) {
			yyerror("undefined reference");
		}
	    } MOV Exp {printf("\t(=, -%d, ACC)\n", look_up(TABLE, $1.str)->stack_offset);
}
          | Definition {printf("\t hi");}
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
Condition: IF LEFT_PARENTHESES Exp RIGHT_PARENTHESES{/*printf("\t(cmp ACC, 0)\n\t(jnz, L%d)\n\t(jmp,)"); */} LEFT_CURLY Body RIGHT_CURLY
	 ;
Loop: FOR LEFT_PARENTHESES Assignment SEMICOLON Exp SEMICOLON Assignment RIGHT_PARENTHESES LEFT_CURLY RIGHT_CURLY
    ;
Exp: Add {
	if ($1.is_reg) {
		if ($1.mem.reg != ACC) {
			printf("\t(=, ACC, R%d)\n", $1.mem.reg - 1);
			free_reg(REGS, $1.mem.reg);
		}
	} else {
		printf("\t(=, ACC, -%d)\n", $1.mem.mem.offset);
	}
}
   ;
Add: Add ADD Mul {
   if ($1.is_reg && $3.is_reg) {
	printf("\t(+, R%d, R%d)\n", $1.mem.reg -1, $3.mem.reg-1);
	free_reg(REGS, $3.mem.reg);
	$$ = $1;
   } else if (!$1.is_reg && $3.is_reg) {
	printf("\t(+, R%d, -%d)\n", $3.mem.reg -1, $1.mem.mem.offset);
	$$ = $3;
   } else if ($1.is_reg && !$3.is_reg) {
	printf("\t(+, R%d, -%d)\n", $1.mem.reg -1, $3.mem.mem.offset);
	$$ = $1;
   } else if (!$1.is_reg && !$3.is_reg) {
	$$.mem.reg = alloc_reg(REGS);
	$$.is_reg = true;
	printf("\t(=, R%d, -%d)\n", $$.mem.reg -1, $1.mem.mem.offset);
	printf("\t(+, R%d, -%d)\n", $$.mem.reg -1, $3.mem.mem.offset);
   }
}
   | Add SUB Mul 
   | Mul
   ;
Mul: Mul MUL Item
   | Mul DIV Item
   | Item
   ;
Item: ID {
    struct symbol *s = look_up(TABLE, $1.str);
    if (s == NULL) {
	yyerror("undefined reference");
    }
    $$.is_reg = false;
    if (s->s_type == GLOBAL_VAR) {
	$$.mem.mem.is_global = true;
    } else {
        $$.mem.mem.is_global = false;
    }
    $$.mem.mem.offset = s->stack_offset;
    s = NULL; //avoiding dangling pointers
}
    | LITERAL {$$.mem.reg = alloc_reg(REGS); $$.is_reg = true; printf("\t(=, R%d, %d)\n", $$.mem.reg - 1, $1.val);}
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
