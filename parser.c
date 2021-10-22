#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"

void parserInit(struct parser * self) {
	int test = TYPE_WHITESPACE;
	self->currentToken = 0;
	self->stackPtr = 0;
	for (int i = 0; i < PARSER_TOKEN_STACK_MAX; i++) {
		tokenInit(&self->tokenStack[i]);
	}
}

void parserShift(struct parser * self) {
	struct token currToken = self->tokenArray[self->currentToken];
	if (self->stackPtr == PARSER_TOKEN_STACK_MAX) {
		fprintf(stderr,"Token stack limit exceeeded");
		exit(1);
	}
	self->tokenStack[self->stackPtr] = currToken;
	self->stackPtr++;
	self->currentToken++;
}

int parserElementNIs(struct parser * self, int n, int type) {
	return 0;
}

struct token * parserElementN(struct parser * self, int n) {
	return 0;
}

int parserLookaheadIs(struct parser * self, int type) {
	return 0;
}

struct token * parserLookahead(struct parser * self) {
	return 0;
}