#include "symbol-table.h"
#include <stddef.h>
#include <stdio.h>

int main() {
	struct table* head = NULL;
	push_table(&head);
	push_table(&head);
	add_symbol(head, create_symbol("foobar", LIT));
	push_table(&head);
	int counter = 0;
	struct table* i = head;
	while (i != NULL) {
		i = i->next;
		++counter;
	}
	printf("%d\n", counter);
	if(look_up(head, "foobar") != NULL) {
		printf("not null\n");
	} else {
		printf("null\n");
	}
	
	pop_table(&head);
	pop_table(&head);
	pop_table(&head);
	push_table(&head);
	push_table(&head);
	push_table(&head);
	push_table(&head);
	push_table(&head);
	free_table_list(&head);
	pop_table(&head);
	return 0;
}
