#include "symbol-table.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

//helper functions
void free_symbol(struct symbol *s) {
	free(s->name);
	s->name = NULL;
	free(s);
}

void free_node(struct node *n) {
	free_symbol(n->s);
	n->s = NULL;
	free(n);
}

void free_table(struct table *t) {
	for (int i = 0; i < TABLE_SIZE; ++i) {
		if (t->array[i] != NULL) {
			free_node(t->array[i]);
			t->array[i] = NULL;
		}
	}
	free(t);
}


unsigned long long hash(char *s) {
	return (unsigned long long)s[0] % TABLE_SIZE; //TODO make more robust hash
}

struct table* make_table() {
	struct table *t = (struct table*)malloc(sizeof(struct table));
	t->next = NULL;
	for (int i = 0; i < TABLE_SIZE; ++i) {
		t->array[i] = NULL;
	}
	return t;
}

//main functions
void free_table_list(struct table **head) {
	for (struct table *i = (*head); i != NULL;) {
		struct table *prv = i;
		i = i->next;
		free_table(prv);
		prv = NULL;
	}
	(*head) = NULL;
}

void push_table(struct table **head) {
	if ((*head) == NULL) {
		(*head) = make_table();
		(*head)->stack_offset = 0;	
	} else { 
		for (struct table *i = (*head); ; i = i->next) {
			if (i->next == NULL) {
				i->next = make_table();
				i->next->stack_offset = i->stack_offset;
				break;
			}
		}
	}
}

void pop_table(struct table **head) {
	if ((*head) == NULL) {
		printf("pop_table called on empty list\n");
		return;
	}
	if ((*head)->next == NULL) {
		free_table((*head));
		(*head) = NULL;
		return;
	}
	for (struct table *i = (*head); ;i = i->next) {
		if (i->next->next == NULL) {
			free_table(i->next);
			i->next = NULL;
			break;
		}
	}
}

struct symbol* look_up(struct table *head, char *name) {
	for (struct table *i = head; i != NULL; i = i->next) {
		if (i->array[hash(name)] != NULL) {
			for (struct node *j = i->array[hash(name)]; j != NULL;j = j->next) {
				if (!strcmp(name, j->s->name)) {
					return j->s;
				}
			}
		}
	}
	return NULL;
}

void add_symbol(struct table *head, struct symbol *s) {
	struct table *current_t;
	for (current_t = head; current_t->next != NULL; current_t = current_t->next);
	if (current_t->array[hash(s->name)] == NULL) {
		s->stack_offset = current_t->stack_offset;
		current_t->stack_offset += 8; //TODO global scope items are currently effecting this offset
		current_t->array[hash(s->name)] = (struct node*)malloc(sizeof(struct node));
		current_t->array[hash(s->name)]->s = s;
		current_t->array[hash(s->name)]->next = NULL;

	} else {
		struct node *current_n;
		for (current_n = current_t->array[hash(s->name)]; current_n->next == NULL; current_n = current_n->next);
		current_n->next = (struct node*)malloc(sizeof(struct node));
		current_n->next->s = s;
		current_n->next->next = NULL;
	}
}

struct symbol* create_symbol(char *name, enum value_type t) {
	struct symbol *s = malloc(sizeof(struct symbol));
	s->name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(s->name, name);
	s->v_type = t;
	return s;
}
