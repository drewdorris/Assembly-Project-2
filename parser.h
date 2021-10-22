#ifndef PARSER_H
#define PARSER_H
#include "token.h"

struct parser {
	//Input format not specified right now. Probably an array of tokens.
	struct token * tokenArray;
	int tokenArrayLength;
	int currentToken;
};

void parserInit(struct parser * self);

int parserNext(struct parser * self);

int parserLookaheadIs(struct parser * self, int type);

struct token * parserLookahead(struct parser * self);

void parserExpectOrError(struct parser * self, int type);

void parserError(void);

#endif