#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"

void parserInit(struct parser * self) {
	self->currentToken = 0;
}

int parserNext(struct parser * self) {
	if (self->currentToken == self->tokenArrayLength) {
		return 0;
	}
	self->currentToken++;
	return 1;
}

int parserLookaheadIs(struct parser * self, int type) {
	return parserLookahead(self)->type == type;
}

struct token * parserLookahead(struct parser * self) {
	return self->tokenArray[self->currentToken];
}

void parserExpectOrError(struct parser * self, int type) {
	if (parserLookaheadIs(parser,type)) {
		parserNext(parser);
	} else {
		fprintf(stderr,"Syntax error");
		exit(1);
	}
}