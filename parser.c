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

//Top rule for a C program. Includes global declarations and functions.
struct program program(struct parser * self) {
	struct declaration * decls = malloc(sizeof(struct declaration) * 256);
	int nDecls = 0;
	parserNext(self);
	while (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT) || parserLookaheadIs(self,TYPE_KW_VOID)) {
		//start of declaration
		struct declaration decl = declaration(self);
		//add declaration to list
		if (nDecls == 256) {
			parserError();
		}
		decls[nDecls] = decl;
		nDecls++;
	}
	struct program prgm;
	prgm.nDeclarations = nDecls;
	prgm.declarations = decls;
	return prgm;
}

//Declarations, such as variables and functions.
struct declaration declaration(struct parser * self) {
	//what is this vartype? skip for now
	fprintf(stderr,"got a vartype %d\n",parserLookahead(self)->type);
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_KW_MAIN)) {
		parserNext(self);
		//main function
		parserExpectOrError(self,TYPE_LEFT_PAREN);
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_LEFT_BRACE);
		//function internals
		struct block blockData = block(self);
		struct declaration decl;
		decl.declarationType = DECL_MAIN;
		decl.functionBlock = blockData;
		return decl;
	} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
		//identifier, could be a variable, could be a function
		//grab identifier here
		parserNext(self);
		//semicolon, assignment, or function? next token ;, =, or ( determines it
	}
}

//Blocks introduced { }. Contains a sequence of semicolon-seperated statements which can include declarations
struct block block(struct parser * self) {
	struct blockElement * blockElements = malloc(sizeof(struct blockElement) * 256);
	int nBlockElements = 0;

	while (!parserLookaheadIs(self,TYPE_RIGHT_BRACE)) {
		if (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT)) {
			//start of declaration
			struct declaration decl = declaration(self);
		} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
			//Usage of a variable. Probably an assignment statement, though it could also be a function call
		} else if (parserLookaheadIs(self,TYPE_KW_PRINTF)) {
			//printf here, definitely a print call
			parserNext(self);
			printfParse(self);
		} else if (parserLookaheadIs(self,TYPE_KW_RETURN)) {
			//Return statement
			parserNext(self);
			parserExpectOrError(self,TYPE_NUMBER);
			parserExpectOrError(self,TYPE_SEMI);
		} else if (parserLookaheadIs(self,TYPE_KW_IF)) {
			//If statement-not handling now
			parserError();
		} else if (parserLookaheadIs(self,TYPE_KW_WHILE)) {
			//While statement- not doing that now
			parserError();
		}
	}
	struct block block;
	block.elements = blockElements;
	block.nElements = nBlockElements;
	return block;
}

//Internals of a printf call.
//Parser is continuing from here
//      V
//printf("Hello World!\n");
struct statement * printfParse(struct parser * self) {
	parserExpectOrError(self,TYPE_LEFT_PAREN);
	if (!parserLookaheadIs(self,TYPE_STRING)) {
		parserError(); //string must be here
	}
	struct token * formatString = parserLookahead(self);
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_RIGHT_PAREN)) {
		//String output
		parserNext(self);
		parserExpectOrError(self,TYPE_SEMI);
	} else if (parserLookaheadIs(self,TYPE_COMMA)) {
		//Probably number output
		//printf("%d",x);
		parserNext(self);
		//Expression here
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_SEMI);
	} else {
		parserError();
	}
}