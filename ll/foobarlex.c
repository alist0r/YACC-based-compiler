#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "token.h"

int print_stack(struct token_stack *s) {
	for (int i = 0; i < s->top; ++i) {
		switch(s->array[i]) {
		case FOO:
			printf("<FOO>\n");
			break;
		case BAR:
			printf("<BAR>\n");
			break;
		}
	}
}

int input_fail() {
	printf("unexpected input");
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: foobarfoo <filepath>");
		exit(1);
	}

	FILE *fp = fopen(argv[1], "r");

	if (errno) {
		perror("fopen error (line 14)");
		exit(1);
	}

	struct token_stack *stack = token_stack_init(10);

	int x = 0;
	while (x != EOF) {
		x = fgetc(fp);
		while (x == ' ' || x == '\n' || x == '\r') {
			x = fgetc(fp);
		}
		printf("%c", x);
		switch (x) {
		case 'f':
			int y = fgetc(fp);
			int z = fgetc(fp);
			if (y == z && z == 'o') {
				token_push(&stack, FOO);
			} else {
				input_fail();
			}
			break;
		case 'b':
			if (fgetc(fp) == 'a') {
				if (fgetc(fp) == 'r') {
					token_push(&stack, BAR);
				} else {
					input_fail();
				}	
			} else {
				input_fail();
			}
			break;
		case EOF:
			break;
		default:
			printf("%c",x);
			input_fail();
			break;
		}
	}
	print_stack(stack);
	return 0;
}
