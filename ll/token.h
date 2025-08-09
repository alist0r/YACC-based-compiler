#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdint.h>

enum token_type {DEC, ID, ADD, SUB, MUL, DIV, LSH, RSH, MOV, FOR, IF, SEM, NOT, AND,
	OR, LPR, RPR, LBR, RBR, END, LIT, RET, COMMA};

union token_data {
	uint64_t u64;
	double f64;
	char *label;
};

struct token {
	enum token_type type;
	union token_data data;
};

struct token_queue {
	struct token **array;
	size_t size;
	size_t head;
	size_t tail;
};

void print_queue(struct token_queue *q);
struct token_queue* token_queue_init(size_t size);
void token_queue_free(struct token_queue *q);
void token_add(struct token_queue **q, struct token* t);
struct token* token_get(struct token_queue *q);
struct token* token_peak(struct token_queue *q);
struct token* make_label(char *name);
void free_label(struct token *label);
struct token* make_u64_litteral(uint64_t val);
struct token* make_token(enum token_type type);
#endif
