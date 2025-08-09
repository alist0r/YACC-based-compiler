#include <stdio.h>
#include <stdlib.h>
#include "token.h"

void print_queue(struct token_queue *q) {
	for (int i = 0; i < q->size; ++i) {
		if (q->array[i] == NULL) {
			printf("NULL\n");
		} else {
			switch (q->array[i]->type) {
			case DEC:
				printf("DEC\n");
				break;
			case ID:
				printf("ID\n");
				break;
			case ADD:
				printf("ADD\n");
				break;
			case SUB:
				printf("SUB\n");
				break;
			case MUL:
				printf("MUL\n");
				break;
			case DIV:
				printf("DIV\n");
				break;
			case LSH:
				printf("LSH\n");
				break;
			case RSH:
				printf("RSH\n");
				break;
			case MOV:
				printf("MOV\n");
				break;
			case FOR:
				printf("FOR\n");
				break;
			case IF:
				printf("IF\n");
				break;
			case SEM:
				printf("SEM\n");
				break;
			case NOT:
				printf("NOT\n");
				break;
			case AND:
				printf("AND\n");
				break;
			case OR:
				printf("OR\n");
				break;
			case LPR:
				printf("LPR\n");
				break;
			case RPR:
				printf("RPR\n");
				break;
			case LBR:
				printf("LBR\n");
				break;
			case RBR:
				printf("RBR\n");
				break;
			case END:
				printf("END\n");
				break;
			case LIT:
				printf("LIT\n");
				break;
			case RET:
				printf("RET\n");
				break;
			case COMMA:
				printf("COMMA\n");
				break;
			}
		}
	}
}

struct token_queue* token_queue_init(size_t size) {
	struct token_queue *q = (struct token_queue*)malloc(sizeof(struct token_queue));
	if (q == NULL) {
		printf("FATAL: could not (m)allocate queue");
		exit(1);
	}
	q->array = (struct token**)malloc(sizeof(struct token*) * size);
	if (q->array == NULL) {
		printf("FATAL: could not (m)allocate array in queue");
		exit(1);
	}
	q->size = size;
	q->head = 0;
	q->tail = 0;
	return q;
}

void token_queue_free(struct token_queue *q) {
	free(q->array);
	q->array = NULL;
	free(q);
	q = NULL;
}

void token_add(struct token_queue **q, struct token* t) {
	if ((*q)->head >= (*q)->size) {
		struct token_queue *new_q = token_queue_init((*q)->size * 2);
		new_q->size = (*q)->size * 2;
		new_q->head = (*q)->head;
		new_q->tail = (*q)->tail;
		for (int i = (*q)->tail; i < (*q)->size; ++i) {
			new_q->array[i] = (*q)->array[i];
		}
		token_queue_free(*q);
		*q = new_q;
	}
	(*q)->array[(*q)->head++] = t;
}

struct token* token_get(struct token_queue *q) {
	return (q->tail >= q->head) ? NULL : q->array[q->tail++];
}

struct token* token_peak(struct token_queue *q) {
	return (q->tail >= q->head) ? NULL : q->array[q->tail];
}

struct token* make_label(char *name) {
	struct token *t = (struct token*)malloc(sizeof(struct token));
	t->type = ID;
	t->data.label = name;
	return t;
}

void free_label(struct token *label) {
	free(label->data.label);
	label->data.label = NULL;;
	free(label);
	label = NULL;
}

struct token* make_u64_litteral(uint64_t val) {
	struct token *t = (struct token*)malloc(sizeof(struct token));
	t->type = LIT;
	t->data.u64 = val;
	return t;
} 

struct token* make_token(enum token_type type) {
	struct token *t = (struct token*)malloc(sizeof(struct token));
	t->type = type;
	t->data.u64 = 0;
	return t;
}
