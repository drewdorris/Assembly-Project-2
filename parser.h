#ifndef PARSER_H
#define PARSER_H
#include "token.h"

#define PARSER_TOKEN_STACK_MAX 256

struct parser {
	//Input format not specified right now. Probably an array of tokens.
	int currentToken;
	struct token token_stack[PARSER_TOKEN_STACK_MAX];
	int stack_ptr;
};

#endif