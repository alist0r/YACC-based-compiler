#include <stdbool.h>
enum regs {NIL, ACC, R1, R2, R3, R4, R5, R6, R7, R8, R9};

struct reg_stack {
	bool acc_lock;
	enum regs array[9];
	int top;
};

enum regs alloc_acc(struct reg_stack *s);
void free_acc(struct reg_stack *s);
struct reg_stack* init_reg_stack();
enum regs alloc_reg(struct reg_stack *s);
void free_reg(struct reg_stack *s, enum regs reg);
