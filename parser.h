#ifndef PARSER_H
#define PARSER_H
#include "token.h"

#define PARSER_TOKEN_STACK_MAX 256

struct parser {
	//Input format not specified right now. Probably an array of tokens.
	struct token * tokenArray;
	int tokenArrayLength;
	int currentToken;
	struct token tokenStack[PARSER_TOKEN_STACK_MAX];
	int stackPtr;
};

void parserInit(struct parser * self);

void parserShift(struct parser * self);

int parserElementNIs(struct parser * self, int n, int type);

struct token * parserElementN(struct parser * self, int n);

int parserLookaheadIs(struct parser * self, int type);

struct token * parserLookahead(struct parser * self);

#endif