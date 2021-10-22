#ifndef PARSER_H
#define PARSER_H
#include "token.h"

struct parser {
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

//Rules

void program(struct parser * self);

void declaration(struct parser * self);

void block(struct parser * self);

void printfParse(struct parser * self);

#endif