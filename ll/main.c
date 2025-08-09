#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "token.h"
#include "lex.h"
#include "parse.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: parse <filepath>");
		exit(1);
	}

	FILE *fp = fopen(argv[1], "r");

	if (errno) {
		perror("fopen error (line 14)");
		exit(1);
	}

	parse(fp);
	return 0;
}
