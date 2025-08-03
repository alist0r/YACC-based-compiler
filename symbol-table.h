#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#define TABLE_SIZE 10

enum value_type {U64, FLOAT};

enum symbol_type {CONDITION, LABEL, FUNCTIONCALL};

struct symbol {
	char *name;
	enum symbol_type s_type;
	enum value_type v_type;
	int stack_offset;
};

struct node {
	struct symbol *s;
	struct node *next;
};

struct table {
	struct node *array[TABLE_SIZE];
	struct table *next;
	int stack_offset;
};


void free_table_list(struct table **head);
void push_table(struct table **head);
void pop_table(struct table **head);
struct symbol* look_up(struct table *head, char *name);
void add_symbol(struct table *head, struct symbol *s);
struct symbol* create_symbol(char *name, enum value_type t);
#endif
