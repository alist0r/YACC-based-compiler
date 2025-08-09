#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

char *keywords[] = {       "return", "for", "if", "u64"};
enum token_type tokens[] = {RET,      FOR,   IF,   DEC};

int file_pos;
int c;
struct token_queue **queue;
FILE *file;

void init_lex(FILE *fp, struct token_queue **q) {
	file_pos = 0;
	c = 0;
	queue = q;
	file = fp;
}

void lex_error(char *str, int line) {
	printf("%d: %s\n",line,str);
	exit(1);
}

void skip_whitespace() {
	while (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
		c = fgetc(file);
	}
	file_pos = ftell(file) - 1;
}

void number() {
	long long accumulator = 0;
	while (c >= '0' && c <= '9') {
		long long num = c - '0';
		accumulator = accumulator * 10;
		accumulator = accumulator + num;
		c = fgetc(file);
	}
	file_pos = ftell(file) - 1;
	token_add(queue, make_u64_litteral(accumulator));
}

char* get_str() {
	int str_pos = file_pos;
	int strlen = 0;
	while (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9') {
		c = fgetc(file);
		++strlen;
	}
	char *str = malloc(sizeof(char) * strlen + 1);
	if (str == NULL) {
		printf("get_str: failed to allocate string");
		exit (1);
	}
	fseek(file, str_pos, SEEK_SET);
	for (int i = 0; i < strlen; ++i) {
		str[i] = fgetc(file);
	}
	str[strlen] = '\0';
	c = fgetc(file);
	return str;
}

bool check_keyword(char *word, enum token_type type) {
	int is_key = false;
	int old_pos = file_pos;
	char *str = get_str();
	if (!strcmp(str, word)) {
		token_add(queue, make_token(type));
		is_key = true;
	} else {
		file_pos = old_pos;
		fseek(file, file_pos, SEEK_SET);
		c = fgetc(file);
	}
	free(str);
	return is_key;

}

void label() {
	char *str = get_str();
	token_add(queue, make_label(str));
}

void operator() {
	switch (c) {
	case '+':
		token_add(queue, make_token(ADD));
		c = fgetc(file);
		break;
	case '-':
		token_add(queue, make_token(SUB));
		c = fgetc(file);
		break;
	case '*':
		token_add(queue, make_token(MUL));
		c = fgetc(file);
		break;
	case '/':
		token_add(queue, make_token(DIV));
		c = fgetc(file);
		break;
	case '(':
		token_add(queue, make_token(LPR));
		c = fgetc(file);
		break;
	case ')':
		token_add(queue, make_token(RPR));
		c = fgetc(file);
		break;
	case '{':
		token_add(queue, make_token(LBR));
		c = fgetc(file);
		break;
	case '}':
		token_add(queue, make_token(RBR));
		c = fgetc(file);
		break;
	case ';':
		token_add(queue, make_token(SEM));
		c = fgetc(file);
		break;
	case '<':
		c = fgetc(file);
		if (c != '<') {
			lex_error("unexpected charecter after first <", __LINE__);
		}
		token_add(queue, make_token(LSH));
		c = fgetc(file);
		break;
	case '>':
		c = fgetc(file);
		if (c != '>') {
			lex_error("unexpected charecter after first >", __LINE__);
		}
		token_add(queue, make_token(RSH));
		c = fgetc(file);
		break;
	case '=':
		token_add(queue, make_token(MOV));
		c = fgetc(file);
		break;
	case '~':
		token_add(queue, make_token(NOT));
		c = fgetc(file);
		break;
	case '&':
		token_add(queue, make_token(AND));
		c = fgetc(file);
		break;
	case '|':
		token_add(queue, make_token(OR));
		c = fgetc(file);
		break;
	case ',':
		token_add(queue, make_token(COMMA));
		c = fgetc(file);
		break;
	default:
		printf("\n'%c'\n", c);
		lex_error("unexpected charecter", __LINE__);
		break;
	}
}


void lex() {
	c = fgetc(file);
	for(;;) {
		skip_whitespace();
		if (c == EOF) {
			token_add(queue, make_token(END));
			return;
		}
		if (c >= '0' && c <= '9') {
			number();
		} else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
			bool is_key = false;
			for (int i = 0; i < sizeof(keywords)/sizeof(char*) && !is_key; ++i) {
				is_key = check_keyword(keywords[i], tokens[i]);
				skip_whitespace();
			}
			if (!is_key) {
				label();
			}
		} else {
			operator();
		}
	}
}
