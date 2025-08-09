#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "token.h"

/**
 * when checking the first set, if the first terminal of a set is part of a
 * non-terminal for that set use peak. otherwise use get. when using token_get
 * the caller needs to free the token it took from the queue. when using
 * token_peak remember to NULL its not required but i like programing
 * defencivly. later down the line some other function will have a ptr to the
 * same token that got peaked so i like NULL-ing ptrs that are done with with.
 */

/**
 * NOTES
 * i should make null checks return a ptr instead of void and then use them in
 * 	conditionals
 * i should make each function check its own first and follow set instead of
 * 	the caller checking if they can call the next funtion (i think)
 * i should also find a way to keep track of LINE and COL for debugging
 */

struct token_queue *input;
void Exp();
void Scope();
void Items();

void null_token_check(struct token *t, char *str) {
	if (t == NULL){
		printf("token in %s was null\n", str);
		exit(1);
	}
}

void parse_error(char *caller) {
	printf("tail: %d\n", input->tail);
	print_queue(input);
	printf("parse error from %s\n", caller);
	exit(1);
}

bool check_token_get(enum token_type type, char *caller) {
	struct token *t = token_get(input);
	null_token_check(t, caller);
	if (t->type != type) {
		free(t);
		t = NULL;
		return false;
	}
	free(t);
	t = NULL;
	return true;
}

bool check_token_peak(enum token_type type, char *caller) {
	struct token *t = token_peak(input);
	null_token_check(t, caller);
	if (t->type != type) {
		t = NULL;
		return false;
	}
	t = NULL;
	return true;

}

void ArgsP() {
	if (check_token_peak(COMMA, "BitOrP")) {
		token_get(input);
		Exp();
		ArgsP();
	}
}

void Args() {
	Exp();
	ArgsP();
}

void Function() {
	if (!check_token_get(LPR, "Function")) {
		parse_error("Function");
	}
	Args();

	if (!check_token_get(RPR, "Function")) {
		parse_error("Function");
	}
}

void Var() {
	if (!check_token_get(ID, "Var")) {
		parse_error("Var");
	}
	if (check_token_peak(LPR, "Var")) {
		Function();
	}
}

//Val -> "("Exp")" .
//Val -> Var .
//Val -> num .
//Var -> id Function .
//Function -> "("Args")" .
//Function -> .
//follow set == ), ,, ;, +, -, *, /, >>, <<, |, &
void Val() {
	if (check_token_peak(LPR, "Val")) {
		token_get(input);
		Exp();
		if (!check_token_get(RPR, "Val")) {
			parse_error("Val");
		}
	} else if (check_token_peak(ID, "Val")) {
		Var();
	} else if (check_token_get(LIT, "Val")) {
		//num
	} else {
		parse_error("Val");
	}
}

void BitNot() {
	if (check_token_peak(NOT, "BitAndP")) {
		token_get(input);
		Val();
	}
	Val();
}

void BitAndP() {
	if (check_token_peak(AND, "BitAndP")) {
		token_get(input);
		BitNot();
		BitAndP();
	}
}

void BitAnd() {
	BitNot();
	BitAndP();
}

void BitOrP() {
	if (check_token_peak(OR, "BitOrP")) {
		token_get(input);
		BitAnd();
		BitOrP();
	}
}

void BitOr() {
	BitAnd();
	BitOrP();
}

void BitShiftP() {
	if (check_token_peak(LSH, "BitShiftP")) {
		token_get(input);
		BitOr();
		BitShiftP();
	} else if (check_token_peak(RSH, "BitShiftP")) {
		token_get(input);
		BitOr();
		BitShiftP();
	}
}

void BitShift() {
	BitOr();
	BitShiftP();
}

void MulP() {
	if (check_token_peak(MUL, "MulP")) {
		token_get(input);
		BitShift();
		MulP();
	} else if (check_token_peak(DIV, "MulP")) {
		token_get(input);
		BitShift();
		MulP();
	}
}

void Mul() {
	BitShift();
	MulP();
}

void AddP() {
	if (check_token_peak(ADD, "AddP")) {
		token_get(input);
		Mul();
		AddP();
	} else if (check_token_peak(SUB, "AddP")) {
		token_get(input);
		Mul();
		AddP();
	}
}

void Add() {
	Mul();
	AddP();
}

void Exp() {
	Add();
}

void Declaration() {
	if (!check_token_get(DEC, "Declaration")) {
		parse_error("Declaration1");
	}
	if (!check_token_get(ID, "Declaration")) {
		parse_error("Declaration2");
	}
}

void DecArgsP() {
	if (check_token_peak(COMMA, "DecArgsP")) {
		token_get(input);
		Declaration();
		DecArgsP();
	}
}

void DecArgs() {
	if (!check_token_peak(RPR, "DecArgs")) {
		Declaration();
		DecArgsP();
	}
}
//Assignment -> Declaration "=" Exp .
//Assignment -> id "=" Exp .
void Assignment() {
	if (check_token_peak(DEC, "Assignment")) {
		Declaration();
		if (!check_token_get(MOV, "Assignment")) {
			parse_error("Assignment");
		}
		Exp();
		if (!check_token_get(SEM, "Assignment")) {
			parse_error("Assignment");
		}
		Items();
	} else if (check_token_get(ID, "Assignment")) {
		if (!check_token_get(MOV, "Assignment")) {
			parse_error("Assignment");
		}
		Exp();
		if (!check_token_get(SEM, "Assignment")) {
			parse_error("Assignment");
		}
		Items();
	} else {
		parse_error("Assignment");
	}

}


void Conditional() {
	token_get(input); //move on past for token
	if (!check_token_get(LPR, "Loop")) {
		parse_error("Loop");
	}
	Exp();
	if (!check_token_get(RPR, "Loop")) {
		parse_error("Loop");
	}
	Scope();
}

void Loop() {
	token_get(input); //move on past for token
	if (!check_token_get(LPR, "Loop")) {
		parse_error("Loop");
	}
	Assignment();

	if (!check_token_get(SEM, "Loop")) {
		parse_error("Loop");
	}
	Exp();

	if (!check_token_get(SEM, "Loop")) {
		parse_error("Loop");
	}
	Assignment();

	if (!check_token_get(RPR, "Loop")) {
		parse_error("Loop");
	}
	Scope();
}

void Return() {
	if (!check_token_get(RET, "Return")) {
		parse_error("Return");
	}
	Exp();
	if (!check_token_get(SEM, "Return")) {
		parse_error("Return");
	}
	Items();
}

void Items() {
	struct token *t = token_peak(input);
	null_token_check(t, "Items");
	switch (t->type) {
	case LBR:
		Scope();
		break;
	case ID:
	case DEC:
		Assignment();
		break;
	case FOR:
		Loop();
		break;
	case IF:
		Conditional();
		break;
	case RET:
		Return();
		break;
	case RBR:
		return;
		//epsilon
	default:
		parse_error("Items");
	}

}

void Scope() {
	if (!check_token_get(LBR, "Scope")) {
		parse_error("Scope");
	}
	Items();

	if (!check_token_get(RBR, "Scope")) {
		parse_error("Scope");
	}
}

void FuncDec() {
	if (!check_token_peak(DEC, "FuncDec")) {
		parse_error("FuncDec");
	}
	Declaration();

	if (!check_token_get(LPR, "FuncDec")) {
		parse_error("FuncDec");
	}
	DecArgs();

	if (!check_token_get(RPR, "FuncDec")) {
		parse_error("FuncDec");
	}
	Scope();
}

void Program() {
	if (check_token_peak(DEC, "Program")) {
		FuncDec();
		Program();
	} else if (check_token_get(END, "Program")) {
		return;
	} else {
		parse_error("Program");
	}
}

void parse(FILE *fp) {
	struct token_queue *q = token_queue_init(10);
	input = q;
	init_lex(fp, &input);
	lex();
	Program();
	printf("done\n");
}
