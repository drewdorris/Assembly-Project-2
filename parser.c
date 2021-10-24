#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"

void parserDebug(void) {
	/*
	[ int ][ main ][ ( ][ ) ][ { ]
		[ printf ][ ( ][ "Hello World!\n" ][ ) ][ ; ]
		[ return ][ 0 ][ ; ]
	[ } ]
	*/
	struct token tokens[16];
	tokens[0].type = TYPE_KW_INT;
	tokens[1].type = TYPE_KW_MAIN;
	tokens[2].type = TYPE_LEFT_PAREN;
	tokens[3].type = TYPE_RIGHT_PAREN;
	tokens[4].type = TYPE_LEFT_BRACE;
	tokens[5].type = TYPE_KW_PRINTF;
	tokens[6].type = TYPE_LEFT_PAREN;
	tokens[7].type = TYPE_STRING;
	tokens[7].payload = "Hello World!\n";
	tokens[8].type = TYPE_RIGHT_PAREN;
	tokens[9].type = TYPE_SEMI;
	tokens[10].type = TYPE_KW_RETURN;
	tokens[11].type = TYPE_NUMBER;
	int ret = 0;
	tokens[11].payload = &ret;
	tokens[12].type = TYPE_SEMI;
	tokens[13].type = TYPE_RIGHT_BRACE;

	struct parser testParser;
	testParser.tokenArray = tokens;
	testParser.tokenArrayLength = 14;
	parserInit(&testParser);
	struct program prgm = program(&testParser);
}

void parserInit(struct parser * self) {
	self->currentToken = 0;
}

int parserHasNext(struct parser * self) {
	return self->currentToken != self->tokenArrayLength;
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
	while (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT) || parserLookaheadIs(self,TYPE_KW_VOID)) {
		//start of declaration
		struct declaration decl = declaration(self);
		//add declaration to list
		if (nDecls == 256) {
			parserError();
		}
		decls[nDecls] = decl;
		nDecls++;
		if (!parserHasNext(self)) break;
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
			struct statement stmt = printfParse(self);
			
			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			//add data to list
			if (nBlockElements == 256) {
				parserError();
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_KW_RETURN)) {
			//Return statement
			parserNext(self);
			parserExpectOrError(self,TYPE_NUMBER);
			parserExpectOrError(self,TYPE_SEMI);

			struct statement stmt;
			stmt.statementType = STMT_RETURN;
			stmt.rhs.leftType = EXPR_VAL_NUMBER;
			stmt.rhs.operator = EXPR_OP_NOP;
			stmt.rhs.rightType = EXPR_VAL_UNARY;
			
			int * code = malloc(sizeof(int));
			*code = 0;
			stmt.rhs.left = code;

			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			//add data to list
			if (nBlockElements == 256) {
				parserError();
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
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
struct statement printfParse(struct parser * self) {
	parserExpectOrError(self,TYPE_LEFT_PAREN);
	if (!parserLookaheadIs(self,TYPE_STRING)) {
		parserError(); //string must be here
	}

	struct statement printfStmt;
	printfStmt.statementType = STMT_PRINTF_CALL;

	struct token * formatStringToken = parserLookahead(self);
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_RIGHT_PAREN)) {
		//String output
		char * formatString = ((char *)(formatStringToken->payload));
		printfStmt.rhs.left = formatString;
		printfStmt.rhs.leftType = EXPR_VAL_STRING;
		printfStmt.rhs.operator = EXPR_OP_NOP;
		printfStmt.rhs.rightType = EXPR_VAL_UNARY;
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