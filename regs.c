#include "regs.h"
#include <stdlib.h>
#include <stdio.h>

enum regs alloc_acc(struct reg_stack *s) {
	if (s->acc_lock == true) {
		return NIL;
	} else {
		s->acc_lock = true;
		return ACC;
	}
}

void free_acc(struct reg_stack *s) {
	s->acc_lock = false;
}

struct reg_stack* init_reg_stack() {
	struct reg_stack *s = (struct reg_stack*)malloc(sizeof(struct reg_stack));
	s->array[0] = R9;
	s->array[1] = R8;
	s->array[2] = R7;
	s->array[3] = R6;
	s->array[4] = R5;
	s->array[5] = R4;
	s->array[6] = R3;
	s->array[7] = R2;
	s->array[8] = R1;
	s->top = 8;
	s->acc_lock = false;
	return s;
}

enum regs alloc_reg(struct reg_stack *s) {
	if (s->top < 0) {
		return NIL;
	}
	return s->array[s->top--];
}

void free_reg(struct reg_stack *s, enum regs reg) {
	if (s->top >= 8) {
		printf("reg.c: reg_stack is full how did you get here?");
		exit(1);
	}
	s->array[++s->top] = reg;
}
