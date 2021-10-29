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
	int ret = 42;
	tokens[11].payload = &ret;
	tokens[12].type = TYPE_SEMI;
	tokens[13].type = TYPE_RIGHT_BRACE;

	struct parser testParser;
	testParser.tokenArray = tokens;
	testParser.tokenArrayLength = 14;
	parserInit(&testParser);
	struct program prgm = program(&testParser);
	printProgramTree(&prgm);
}

void parserInit(struct parser * self) {
	self->currentToken = 0;
}

int parserHasNext(struct parser * self) {
	return self->currentToken != self->tokenArrayLength;
}

int parserNext(struct parser * self) {
	if (self->currentToken == self->tokenArrayLength) {
		parserError();
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
	int varType = parserLookahead(self)->type;
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_KW_MAIN)) {
		parserNext(self);
		//main function
		parserExpectOrError(self,TYPE_LEFT_PAREN);
		if (parserLookaheadIs(self,TYPE_KW_VOID)) parserNext(self); //skip void, as in int fun(void)
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_LEFT_BRACE);
		//function internals
		struct block blockData = block(self);
		struct declaration decl;
		decl.declarationType = DECL_MAIN;
		decl.nArgTypes = 0;
		decl.argTypes = NULL;
		decl.functionBlock = blockData;
		return decl;
	} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
		//identifier, could be a variable, could be a function
		char * identifier = parserLookahead(self)->payload;
		parserNext(self);
		//semicolon, assignment, or function? next token ;, =, or ( determines it
		switch (parserLookahead(self)->type) {
			case TYPE_SEMI:
				{
					//int x;
					//I'll make an implementation-defined decision to zero-initialise anyway despite null being allowed to indicate no initialiser
					struct expression * zeroinitExpr = malloc(sizeof(struct expression));
					switch (varType) {
						case TYPE_KW_INT:
						case TYPE_KW_SHORT:
							{
								int * left = malloc(sizeof(int));
								zeroinitExpr->left = left;
								*left = 0;
								zeroinitExpr->leftType = EXPR_VAL_NUMBER;
								zeroinitExpr->operator = EXPR_OP_NOP;
								zeroinitExpr->right = NULL;
								zeroinitExpr->rightType = EXPR_VAL_UNARY;
								//It's int x; make the decl struct
								struct declaration decl;
								decl.identifier = identifier;
								decl.declarationType = DECL_VARIABLE;
								decl.variableType = varType;
								decl.init = zeroinitExpr;
								return decl;
							}
						default:
							//void x; makes no sense
							parserError();
					}
				}
				break;
			case TYPE_ASSIGN:
				//int x = ...
				parserNext(self);
				break;
			case TYPE_LEFT_PAREN:
				//int x(...) {}
				parserNext(self);
				//right now ignoring possibility of function arguments and balking
				parserExpectOrError(self,TYPE_RIGHT_PAREN);
				if (parserLookaheadIs(self,TYPE_KW_VOID)) parserNext(self); //skip void, as in int fun(void)
				parserExpectOrError(self,TYPE_LEFT_BRACE);
				struct block blockData = block(self);
				struct declaration decl;
				decl.declarationType = DECL_FUNCTION;
				decl.nArgTypes = 0;
				decl.argTypes = NULL;
				decl.functionBlock = blockData;
				return decl;
		}
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
			//choosing to balk if the user somehow has more than 256 decls/statements in a block
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

//Expressions i.e. "Hello World", 1, 1 + 2, x + 4
struct expression expression(struct parser * self) {
	//I won't handle recursive expressions for the moment.
	struct expression expr;
	//Negative handling ~
	{
		if (parserLookaheadIs(self,TYPE_NEG)) {
			expr.operator = EXPR_OP_NEG;
			expr.rightType = EXPR_VAL_UNARY;
			expr.right = NULL;
			parserNext(self);
		}
	}
	//Left
	{
		struct token * leftToken = parserLookahead(self);
		switch (leftToken->type) {
			case TYPE_NUMBER:
				expr.leftType = EXPR_VAL_NUMBER;
				expr.left = leftToken->payload;
				break;
			case TYPE_STRING:
				expr.leftType = EXPR_VAL_STRING;
				expr.left = leftToken->payload;
				break;
			case TYPE_IDENTIFIER:
				expr.leftType = EXPR_VAL_IDENTIFIER;
				expr.left = leftToken->payload;
				break;
			default:
				parserError();
				return expr;
		}
	}
	//Neg handling part 2
	if (expr.operator == EXPR_OP_NEG) {
		parserExpectOrError(self,TYPE_SEMI);
		return expr;
	}
	//Operator other than neg, or ;
	{
		parserNext(self);
		struct token * opToken = parserLookahead(self);
		if (parserLookaheadIs(self,TYPE_SEMI)) {
			expr.operator = EXPR_OP_NOP;
			expr.rightType = EXPR_VAL_UNARY;
			expr.right = NULL;
			return expr;
		}
		switch (opToken->type) {
			case TYPE_ADD:
				expr.operator = EXPR_OP_ADD;
				break;
			case TYPE_SUB:
				expr.operator = EXPR_OP_SUB;
				break;
			case TYPE_OR:
				expr.operator = EXPR_OP_OR;
				break;
			case TYPE_AND:
				expr.operator = EXPR_OP_AND;
				break;
			default:
				parserError();
				return expr;
		}
	}
	//Right
	{
		struct token * rightToken = parserLookahead(self);
		switch (rightToken->type) {
			case TYPE_NUMBER:
				expr.rightType = EXPR_VAL_NUMBER;
				expr.right = rightToken->payload;
				break;
			case TYPE_STRING:
				expr.rightType = EXPR_VAL_STRING;
				expr.right = rightToken->payload;
				break;
			case TYPE_IDENTIFIER:
				expr.rightType = EXPR_VAL_IDENTIFIER;
				expr.right = rightToken->payload;
				break;
			default:
				parserError();
				return expr;
		}
	}
	return expr;
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
	return printfStmt;
}

void printIndent(int indent) {
	while (indent > 0) {
		printf("  ");
		indent--;
	}
}

void printProgramTree(struct program * root) {
	printf("C Program (%d declarations)\n",root->nDeclarations);
	for (int i = 0; i < root->nDeclarations; i++) {
		printDeclaration(&root->declarations[i],1);
	}
}

void printDeclaration(struct declaration * decl, int indent) {
	printIndent(indent);
	printf("Declaration ");
	switch (decl->declarationType) {
		case DECL_MAIN:
			printf("MAIN\n");
			printBlock(&decl->functionBlock,indent+1);
			break;
	}
}

void printBlock(struct block * block, int indent) {
	printIndent(indent);
	printf("Block (%d elements)\n",block->nElements);
	for (int i = 0; i < block->nElements; i++) {
		struct blockElement * elem = &block->elements[i];
		switch (elem->type) {
			case BLCK_DECLARATION:
				printDeclaration(elem->element,indent+1);
				break;
			case BLCK_STATEMENT:
				printStatement(elem->element,indent+1);
				break;
		}
		printf("\n");
	}
}

void printStatement(struct statement * stmt, int indent) {
	printIndent(indent);
	printf("Statement ");
	switch (stmt->statementType) {
		case STMT_RETURN:
			printf("RETURN ");
			printExpression(&stmt->rhs);
			break;
		case STMT_PRINTF_CALL:
			printf("PRINTF ");
			printExpression(&stmt->rhs);
			break;
		default:
			printf("unknown");
			break;
	}
}

void printExpression(struct expression * expr) {
	printf("(");
	switch (expr->leftType) {
		case EXPR_VAL_NUMBER:
			{
				int * val = (int *) expr->left;
				printf("%d",*val);
			}
			break;
		case EXPR_VAL_STRING:
			printf("\"%s\"",expr->left);
			break;
		case EXPR_VAL_EXPRESSION:
			printExpression(expr->left);
			break;
		default:
			printf(" <unknown>");
	}
	switch (expr->operator) {
		case EXPR_OP_ADD:
			printf(" ADD");
			break;
		case EXPR_OP_SUB:
			printf(" SUB");
			break;
		case EXPR_OP_NEG:
			printf(" NEG");
			break;
		case EXPR_OP_AND:
			printf(" AND");
			break;
		case EXPR_OP_OR:
			printf(" OR");
			break;
		case EXPR_OP_NOP:
			printf(" <not used>");
			break;
		default:
			printf(" <unknown>");
			break;
	}
	switch (expr->rightType) {
		case EXPR_VAL_NUMBER:
			printf("%d",expr->right);
			break;
		case EXPR_VAL_STRING:
			printf("\"%s\"",expr->right);
			break;
		case EXPR_VAL_EXPRESSION:
			printExpression(expr->right);
			break;
		case EXPR_VAL_UNARY:
			printf(" <not used>");
			break;
		default:
			printf(" <unknown>");
			break;
	}
	printf(")");
}