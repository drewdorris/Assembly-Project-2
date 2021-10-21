#include "parser.h"
#include "token.h"



void parser_init(struct parser * self) {
	int test = TYPE_WHITESPACE;
	self->currentToken = 0;
	self->stack_ptr = 0;
	for (int i = 0; i < PARSER_TOKEN_STACK_MAX; i++) {

	}
}

void parser_shift(struct parser * self) {
	self->currentToken++;
	int i = 0;
	switch (i) {
		case TYPE_ADD:
			break;
	}
}