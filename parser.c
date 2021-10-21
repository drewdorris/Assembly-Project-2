#include "parser.h"



void parser_init(struct parser * self) {
	self->currentToken = 0;
	self->stack_ptr = 0;
	for ()
}

void parser_shift(struct parser * self) {
	self->currentToken++;
}