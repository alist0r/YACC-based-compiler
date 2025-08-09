#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "token.h"
#include "lex.h"

int current_test = 0;

void test_fail(int test_num, int line) {
	printf("test%d: line %d\n", test_num, line);
	exit(1);
}


void test1() {
	current_test = 1;
	struct token_queue *q = token_queue_init(5);
	for (int i = 0; i < 6; ++i) {
		token_add(&q, malloc(sizeof(struct token)));
	}
	if (q->size != 10) {
		test_fail(current_test, __LINE__ - 1);
	}
	token_queue_free(q);
}

//i dont free the tokens here becuse im lazy but this would be a memory leak
//if i forget to do this in actual code
void test2() {
	current_test = 2;
	struct token_queue *q = token_queue_init(1);
	token_add(&q, make_u64_litteral(1));
	token_add(&q, make_u64_litteral(2));
	token_add(&q, make_u64_litteral(3));
	if (q->size != 4) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_peak(q)->data.u64!= 1) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->data.u64!= 1) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->data.u64!= 2) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->data.u64!= 3) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q) != NULL) {
		test_fail(current_test, __LINE__ - 1);
	}
}

//same here, in actual code i need to free tokens
void test3() {
	current_test = 3;
	FILE *fp = fopen("test3.txt", "r");
	if (errno) {
		perror("test3");
		exit(1);
	}
	struct token_queue *q = token_queue_init(1);
	init_lex(fp, &q);
	lex();
	if (q->size != 8) {
		test_fail(current_test, __LINE__ - 1);
	}

	if (strcmp(token_peak(q)->data.label, "string1")) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != ID) {
		test_fail(current_test, __LINE__ - 1);
	}

	if (strcmp(token_peak(q)->data.label, "string2")) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != ID) {
		test_fail(current_test, __LINE__ - 1);
	}


	if (token_get(q)->type != IF) {
		test_fail(current_test, __LINE__ - 1);
	}


	if (token_get(q)->type != FOR) {
		test_fail(current_test, __LINE__ - 1);
	}

	if (strcmp(token_peak(q)->data.label, "string5")) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != ID) {
		test_fail(current_test, __LINE__ - 1);
	}
}

void test4() {
	current_test = 4;
	FILE *fp = fopen("test4.txt", "r");
	if (errno) {
		perror("test4");
		exit(1);
	}
	struct token_queue *q = token_queue_init(100);
	init_lex(fp, &q);
	lex();
	if (token_get(q)->type != ADD) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != SUB) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != MUL) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != DIV) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != LPR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != RPR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != LBR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != RBR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != SEM) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != ID) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != LSH) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != IF) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != RSH) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != FOR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != MOV) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != NOT) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != AND) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != OR) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != LIT) {
		test_fail(current_test, __LINE__ - 1);
	}
	if (token_get(q)->type != ID) {
		test_fail(current_test, __LINE__ - 1);
	}
}

int main() {
	test1();
	test2();
	test3();
	test4();
	printf("all tests pass!\n");
	return 0;
}
