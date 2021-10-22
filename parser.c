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
	return &self->tokenArray[self->currentToken];
}

void parserExpectOrError(struct parser * self, int type) {
	if (parserLookaheadIs(self,type)) {
		parserNext(self);
	} else {
		fprintf(stderr,"Syntax error");
		exit(1);
	}
}

void parserError(void) {
	fprintf(stderr,"Syntax error");
	exit(1);
}

//rules

void program(struct parser * self) {
	parserNext(self);
	while (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT) || parserLookaheadIs(self,TYPE_KW_VOID)) {
		//start of declaration
		declaration(self);
	}
}

void declaration(struct parser * self) {
	//what is this vartype? skip for now
	fprintf(stderr,"got a vartype %d\n",parserLookahead(self)->type);
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_KW_MAIN)) {
		//main function
		parserExpectOrError(self,TYPE_LEFT_PAREN);
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_LEFT_BRACE);
		//function internals
		parserExpectOrError(self,TYPE_RIGHT_BRACE);
	} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
		//identifier, could be a variable, could be a function
		//grab identifier here
		parserNext();
	}
}