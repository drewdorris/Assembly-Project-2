#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "token.h"
#include "pep.h"

// Run a parser debug program
void parserDebug(void) {
	/*
	[ int ][ main ][ ( ][ ) ][ { ]
		[ int ][ x ][ ; ]
		[ short ][ y ][ = ][ 10 ][ ; ]
		[ x ][ = ][ 42 ][ ; ]
		[ printf ][ ( ][ "Hello World!\n" ][ ) ][ ; ]
		[ return ][ 0 ][ ; ]
	[ } ]
	*/
	// Don't mind this hardcoded tokenised output- the tokeniser did not exist yet
	struct token tokens[32];
	tokens[0].type = TYPE_KW_INT;
	tokens[1].type = TYPE_KW_MAIN;
	tokens[2].type = TYPE_LEFT_PAREN;
	tokens[3].type = TYPE_RIGHT_PAREN;
	tokens[4].type = TYPE_LEFT_BRACE;

	tokens[5].type = TYPE_KW_INT;
	tokens[6].type = TYPE_IDENTIFIER;
	tokens[6].payload = "x";
	tokens[7].type = TYPE_SEMI;

	tokens[8].type = TYPE_KW_SHORT;
	tokens[9].type = TYPE_IDENTIFIER;
	tokens[9].payload = "y";
	tokens[10].type = TYPE_ASSIGN;
	tokens[11].type = TYPE_NUMBER;
	tokens[11].payload = malloc(sizeof(int));
	*((int *)tokens[11].payload) = 10;
	tokens[12].type = TYPE_SEMI;

	tokens[13].type = TYPE_IDENTIFIER;
	tokens[13].payload = "x";
	tokens[14].type = TYPE_ASSIGN;
	tokens[15].type = TYPE_NUMBER;
	tokens[15].payload = malloc(sizeof(int));
	*((int *)tokens[15].payload) = 42;
	tokens[16].type = TYPE_SEMI;

	tokens[17].type = TYPE_KW_PRINTF;
	tokens[18].type = TYPE_LEFT_PAREN;
	tokens[19].type = TYPE_STRING;
	tokens[19].payload = "Hello World!\\n";
	tokens[20].type = TYPE_RIGHT_PAREN;
	tokens[21].type = TYPE_SEMI;
	tokens[22].type = TYPE_KW_RETURN;
	tokens[23].type = TYPE_NUMBER;
	int ret = 42;
	tokens[23].payload = &ret;
	tokens[24].type = TYPE_SEMI;
	tokens[25].type = TYPE_RIGHT_BRACE;

	struct parser testParser;
	testParser.tokenArray = tokens;
	testParser.tokenArrayLength = 26;
	parserInit(&testParser);
	struct program prgm = program(&testParser);
	printf("===Parsed===\n\n");
	printProgramTree(&prgm);
	printf("===PEP9===\n\n");
	pepProgramTree(&prgm);
}

// Initialise state of the parser. User should set the tokenArray and tokenArrayLength fields afterward.
void parserInit(struct parser * self) {
	self->currentToken = 0;
}

// Returns true value if there are tokens left in the array.
int parserHasNext(struct parser * self) {
	return self->currentToken != self->tokenArrayLength;
}

// Advance to the next token, or emit an error at the end of the sequence.
int parserNext(struct parser * self) {
	if (self->currentToken == self->tokenArrayLength) {
		parserError(self);
	}
	self->currentToken++;
	return 1;
}

// Return true if the current token is of the provided type.
int parserLookaheadIs(struct parser * self, int type) {
	return parserLookahead(self)->type == type;
}
// Return the current token.
struct token * parserLookahead(struct parser * self) {
	return &self->tokenArray[self->currentToken];
}

// Consume the current token if the current token is the provided type, otherwise emit an error. After returning the parser will be on the next
// token.
void parserExpectOrError(struct parser * self, int type) {
	if (parserLookaheadIs(self,type)) {
		parserNext(self);
	} else {
		fprintf(stderr,"Syntax error, needed toktype %s, tokidx %d", tokenTypeString(type), self->currentToken);
		exit(1);
	}
}

// Emit an error and stop.
void parserError(struct parser * self) {
	fprintf(stderr,"Syntax error, tokidx %d",self->currentToken);
	exit(1);
}

//rules

//Top rule for a C program. Includes global declarations and functions.
struct program program(struct parser * self) {
	// Declare the declaration array
	struct declaration * decls = malloc(sizeof(struct declaration) * 256);
	int nDecls = 0;
	// Every time we run across int/short/void, a declaration is started.
	while (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT) || parserLookaheadIs(self,TYPE_KW_VOID)) {;
		// start of declaration, parse it
		struct declaration decl = declaration(self);
		// add declaration to list
		if (nDecls == 256) {
			parserError(self);
		}
		decls[nDecls] = decl;
		nDecls++;
		// If there are no more tokens at this point, the program is over.
		if (!parserHasNext(self)) {
			break;
		}
	}
	// Return the struct
	struct program prgm;
	prgm.nDeclarations = nDecls;
	prgm.declarations = decls;
	return prgm;
}

// Declarations, such as variables and functions.
struct declaration declaration(struct parser * self) {
	// what is this vartype? hold onto it for later when we see what this actually is
	int varType = parserLookahead(self)->type;
	// Skip to next and make a guess.
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_KW_MAIN)) {
		parserNext(self);
		// Main function, skip through the arguments header
		parserExpectOrError(self,TYPE_LEFT_PAREN);
		if (parserLookaheadIs(self,TYPE_KW_VOID)) parserNext(self); // skip void, as in int fun(void)
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_LEFT_BRACE);
		// Function internals
		struct block blockData = block(self);
		// Form and return declaration
		struct declaration decl;
		decl.declarationType = DECL_MAIN;
		decl.nArgTypes = 0;
		decl.argTypes = NULL;
		decl.functionBlock = blockData;
		return decl;
	} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
		// identifier, could be a variable, could be a function
		// save the identifier name
		char * identifier = parserLookahead(self)->payload;
		if (strlen(identifier) > 8) {
			parserError(self); // prohibit identifier names longer than 8 chars
		}
		parserNext(self);
		// semicolon, assignment, or function? next token ;, =, or ( determines it
		switch (parserLookahead(self)->type) {
			case TYPE_SEMI:
				{
					// int x;
					// I'll make an implementation-defined decision to zero-initialise anyway despite null being allowed to indicate no initialiser
					parserNext(self);
					// allocate space for expression
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
								// It's int x; make the decl struct
								struct declaration decl;
								decl.identifier = identifier;
								decl.declarationType = DECL_VARIABLE;
								decl.variableType = varType;
								decl.init = zeroinitExpr;
								return decl;
							}
						default:
							// void x; makes no sense
							parserError(self);
					}
				}
				break;
			case TYPE_ASSIGN:
				// int x = ...
				parserNext(self);
				// expression follows now
				struct expression exprData = expression(self);
				if (exprData.leftType == EXPR_VAL_STRING) {
					// Can't assign a string to an int/short as this makes no sense
					parserError(self);
				}
				// otherwise this is fine (probably)
				struct declaration decl;
				decl.identifier = identifier;
				decl.declarationType = DECL_VARIABLE;
				decl.variableType = varType;
				decl.init = malloc(sizeof(struct expression));
				*decl.init = exprData; // assign data via pointer
				// consume semicolon that should be here before returning
				parserExpectOrError(self,TYPE_SEMI);
				return decl;
			case TYPE_LEFT_PAREN:
				// Function like int x(...) {}
				parserNext(self);
				// right now ignoring possibility of function arguments and balking
				// I assume it's only ever empty or void
				if (parserLookaheadIs(self,TYPE_KW_VOID)) parserNext(self); //skip void, as in int fun(void)
				parserExpectOrError(self,TYPE_RIGHT_PAREN);
				parserExpectOrError(self,TYPE_LEFT_BRACE);
				// get the function data
				struct block blockData = block(self);
				// prepare declaration to return
				struct declaration fdecl;
				fdecl.declarationType = DECL_FUNCTION;
				fdecl.nArgTypes = 0;
				fdecl.argTypes = NULL;
				fdecl.functionBlock = blockData;
				return fdecl;
		}
	} else {
		// What followed the type keyword is something that makes no sense
		parserError(self);
	}
	// We should never get here. Just here to stop gcc.
	struct declaration noUse;
	noUse.declarationType = DECL_FUNCTION;
	return noUse;
}

// Blocks introduced { }. Contains a sequence of semicolon-seperated statements which can include declarations
struct block block(struct parser * self) {
	// allocate block element array
	struct blockElement * blockElements = malloc(sizeof(struct blockElement) * 256);
	int nBlockElements = 0;
	// keep going until the block closes
	while (!parserLookaheadIs(self,TYPE_RIGHT_BRACE)) {
		if (parserLookaheadIs(self,TYPE_KW_INT) || parserLookaheadIs(self,TYPE_KW_SHORT)) {
			// start of declaration
			struct declaration decl = declaration(self);
			// add to block element list
			struct blockElement elem;
			elem.type = BLCK_DECLARATION;
			elem.element = malloc(sizeof(struct declaration));
			*((struct declaration *)(elem.element)) = decl;
			// add data to list
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_IDENTIFIER)) {
			// Usage of a variable. Probably an assignment statement, though it could also be a function call
			char * identifier = parserLookahead(self)->payload;
			// next symbol determines function call or assignment
			parserNext(self);
			if (parserLookaheadIs(self,TYPE_ASSIGN)) {
				// assignment
				parserNext(self);
				// get assignment expression and build statement struct
				struct expression expr = expression(self);
				struct statement stmt;
				stmt.statementType = STMT_VAR_ASSIGNMENT;
				stmt.identifier = identifier;
				stmt.rhs = expr;
				// expect the semicolon that should be here
				parserExpectOrError(self,TYPE_SEMI);
				struct blockElement elem;
				elem.type = BLCK_STATEMENT;
				elem.element = malloc(sizeof(struct statement));
				*((struct statement *)(elem.element)) = stmt;
				// add data to list
				if (nBlockElements == 256) {
					parserError(self);
				}
				blockElements[nBlockElements] = elem;
				nBlockElements++;

			} else if (parserLookaheadIs(self,TYPE_LEFT_PAREN)) {
				// function call
				// let's not handle this one for now as spec does not require it
				parserError(self);
			} else {
				// something we can't handle
				parserError(self);
			}
		} else if (parserLookaheadIs(self,TYPE_KW_PRINTF)) {
			// printf here, definitely a print call
			parserNext(self);
			// get printf internal data
			struct statement stmt = printfParse(self);
			// build block element, add to list
			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			// add data to list
			// choosing to balk if the user somehow has more than 256 decls/statements in a block
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_KW_SCANF)) {
			// scanf ( "ignored" , & ident ) ;
			// We handle the input string like it always is "%d" no matter what it actually is
			parserNext(self);
			parserExpectOrError(self,TYPE_LEFT_PAREN);
			parserExpectOrError(self,TYPE_STRING);
			parserExpectOrError(self,TYPE_COMMA);
			parserExpectOrError(self,TYPE_AND);
			// Identifier should be here
			if (!parserLookaheadIs(self,TYPE_IDENTIFIER)) {
				parserError(self);
			}
			// Grab identifier
			char * scanfTarget = (char *) parserLookahead(self)->payload;
			parserNext(self);
			// Expect the ); ending the scanf statement
			parserExpectOrError(self,TYPE_RIGHT_PAREN);
			parserExpectOrError(self,TYPE_SEMI);
			// Create statement structs
			struct statement stmt;
			stmt.statementType = STMT_SCANF_CALL;
			stmt.identifier = scanfTarget;

			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			// add data to list
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_KW_RETURN)) {
			// Return statement
			parserNext(self);
			// Expect a number and semicolon, we ignore the return value anyway
			parserExpectOrError(self,TYPE_NUMBER);
			parserExpectOrError(self,TYPE_SEMI);
			// build statement structs
			struct statement stmt;
			stmt.statementType = STMT_RETURN;
			stmt.rhs.leftType = EXPR_VAL_NUMBER;
			stmt.rhs.operator = EXPR_OP_NOP;
			stmt.rhs.rightType = EXPR_VAL_UNARY;
			
			int * code = malloc(sizeof(int));
			*code = 0; // hard code the return value to always be zero
			stmt.rhs.left = code;

			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			// add data to list
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_KW_IF)) {
			// If statement
			// I do not handle the `if (expr) stmt` form. A block must exist.
			// RHS is the conditional expression, block is the code to be executed
			parserNext(self);
			parserExpectOrError(self,TYPE_LEFT_PAREN);
			struct expression cond = expression(self);
			parserExpectOrError(self,TYPE_RIGHT_PAREN);
			parserExpectOrError(self,TYPE_LEFT_BRACE);
			struct block ifBlock = block(self);
			// default else block in case we do not have one
			struct block elseBlock;
			elseBlock.nElements = 0;
			elseBlock.elements = NULL;
			// try for else
			// fprintf(stderr,"%s",tokenTypeString(parserLookahead(self)->type));
			if (parserLookaheadIs(self,TYPE_KW_ELSE)) {
				// We got an else, get that block
				parserNext(self);
				parserExpectOrError(self,TYPE_LEFT_BRACE);
				elseBlock = block(self);
			}
			// set up block
			struct statement stmt;
			stmt.statementType = STMT_IF;
			stmt.rhs = cond;
			stmt.block = ifBlock;
			stmt.block2 = elseBlock;

			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			// add data to list
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		} else if (parserLookaheadIs(self,TYPE_KW_WHILE)) {
			// While statement
			// I do not handle the `while (expr) stmt` form. A block must exist.
			// RHS is the conditional expression, block is the code to be executed
			parserNext(self);
			parserExpectOrError(self,TYPE_LEFT_PAREN);
			struct expression cond = expression(self);
			parserExpectOrError(self,TYPE_RIGHT_PAREN);
			parserExpectOrError(self,TYPE_LEFT_BRACE);
			struct block whileBlock = block(self);
			// set up block
			struct statement stmt;
			stmt.statementType = STMT_WHILE;
			stmt.rhs = cond;
			stmt.block = whileBlock;

			struct blockElement elem;
			elem.type = BLCK_STATEMENT;
			elem.element = malloc(sizeof(struct statement));
			*((struct statement *)(elem.element)) = stmt;
			// add data to list
			if (nBlockElements == 256) {
				parserError(self);
			}
			blockElements[nBlockElements] = elem;
			nBlockElements++;
		}
	}
	// Right brace should be here
	parserExpectOrError(self,TYPE_RIGHT_BRACE);
	// Create block structure
	struct block block;
	block.elements = blockElements;
	block.nElements = nBlockElements;
	return block;
}

// Expressions i.e. "Hello World", 1, 1 + 2, x + 4
struct expression expression(struct parser * self) {
	// I won't handle recursive expressions for the moment.
	struct expression expr;
	// Negative handling ~
	{
		// This being here changes the rest of the further parsing.
		if (parserLookaheadIs(self,TYPE_NEG)) {
			expr.operator = EXPR_OP_NEG;
			expr.rightType = EXPR_VAL_UNARY;
			expr.right = NULL;
			parserNext(self);
		}
	}
	// Left side/unary target
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
				parserError(self);
				return expr;
		}
	}
	// Neg handling part 2
	if (expr.operator == EXPR_OP_NEG) {
		// Cannot handle anything else here, just return to caller hoping they know what to do
		parserNext(self);
		return expr;
	}
	// Operator other than neg, or ;
	// The expression should be binary or unary non-negative at this point.
	{
		parserNext(self);
		struct token * opToken = parserLookahead(self);
		// if (parserLookaheadIs(self,TYPE_SEMI) || parserLookaheadIs(self,TYPE_RIGHT_PAREN)) {
		// 	expr.operator = EXPR_OP_NOP;
		// 	expr.rightType = EXPR_VAL_UNARY;
		// 	expr.right = NULL;
		// 	return expr;
		// }
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

			case TYPE_CD_EQUAL:
				expr.operator = EXPR_OP_EQ;
				break;
			case TYPE_CD_NOT_EQUAL:
				expr.operator = EXPR_OP_NE;
				break;
			case TYPE_CD_GREATER:
				expr.operator = EXPR_OP_GT;
				break;
			case TYPE_CD_GREATER_OR_EQUAL:
				expr.operator = EXPR_OP_GE;
				break;
			case TYPE_CD_LESS:
				expr.operator = EXPR_OP_LT;
				break;
			case TYPE_CD_LESS_OR_EQUAL:
				expr.operator = EXPR_OP_LE;
				break;
			
			default:
				// Not an operator, must be the next sequence. Return what we have now.
				expr.operator = EXPR_OP_NOP;
				expr.rightType = EXPR_VAL_UNARY;
				expr.right = NULL;
				return expr;
				// parserError(self);
				// return expr;
		}
	}
	// Right
	{
		parserNext(self);
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
				parserError(self);
				return expr;
		}
		parserNext(self);
	}
	// Return binary expression
	return expr;
}

// Internals of a printf call.
// Parser is continuing from here
//       V
// printf("Hello World!\n");
struct statement printfParse(struct parser * self) {
	// Expect basic printf internals
	parserExpectOrError(self,TYPE_LEFT_PAREN);
	if (!parserLookaheadIs(self,TYPE_STRING)) {
		parserError(self); // string must be here
	}
	// Set up statement data
	struct statement printfStmt;
	printfStmt.statementType = STMT_PRINTF_CALL;

	// Save the format string
	struct token * formatStringToken = parserLookahead(self);
	// The next token determines whether this is a string output (right paren) or decimal output (comma)
	// If it is a decimal output, just assume the format string was "%d" regardless of what it actually is
	parserNext(self);
	if (parserLookaheadIs(self,TYPE_RIGHT_PAREN)) {
		// String output
		char * formatString = ((char *)(formatStringToken->payload));
		printfStmt.rhs.left = formatString;
		printfStmt.rhs.leftType = EXPR_VAL_STRING;
		printfStmt.rhs.operator = EXPR_OP_NOP;
		printfStmt.rhs.rightType = EXPR_VAL_UNARY;
		parserNext(self);
		// consume semicolon that should be here
		parserExpectOrError(self,TYPE_SEMI);
	} else if (parserLookaheadIs(self,TYPE_COMMA)) {
		// Probably number output
		// printf("%d",x);
		parserNext(self);
		// Expression here
		struct expression expr = expression(self);
		if (expr.leftType == EXPR_VAL_STRING) {
			parserError(self); // can't have a string here
		}
		printfStmt.rhs = expr;
		// consume end ); that should be here
		parserExpectOrError(self,TYPE_RIGHT_PAREN);
		parserExpectOrError(self,TYPE_SEMI);
	} else {
		// unexpected token here
		parserError(self);
	}
	return printfStmt;
}

// Print an indent.
void printIndent(int indent) {
	while (indent > 0) {
		printf("  ");
		indent--;
	}
}

// Print the full program tree.
void printProgramTree(struct program * root) {
	printf("C Program (%d declarations)\n",root->nDeclarations);
	for (int i = 0; i < root->nDeclarations; i++) {
		printDeclaration(&root->declarations[i],1);
	}
}

// Print a declaration.
void printDeclaration(struct declaration * decl, int indent) {
	printIndent(indent);
	printf("Declaration ");
	switch (decl->declarationType) {
		case DECL_MAIN:
			printf("MAIN\n");
			printBlock(&decl->functionBlock,indent+1);
			break;
		case DECL_VARIABLE:
			printf("VAR %s",decl->identifier);
			if (decl->init != NULL) {
				printf(" INIT ");
				printExpression(decl->init);
			}
			printf("\n");
			break;
	}
}

// Print a code block.
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
		//printf("\n");
	}
}

// Print a statement.
void printStatement(struct statement * stmt, int indent) {
	printIndent(indent);
	printf("Statement ");
	switch (stmt->statementType) {
		case STMT_SCANF_CALL:
			printf("SCANF ");
			printf("%s",stmt->identifier);
			break;
		case STMT_VAR_ASSIGNMENT:
			printf("VAR_ASSIGNMENT ");
			printf("%s",stmt->identifier);
			printf(" <- ");
			printExpression(&stmt->rhs);
			break;
		case STMT_RETURN:
			printf("RETURN ");
			printExpression(&stmt->rhs);
			break;
		case STMT_PRINTF_CALL:
			printf("PRINTF ");
			printExpression(&stmt->rhs);
			break;
		case STMT_IF:
			printf("IF ");
			printExpression(&stmt->rhs);
			printf("\n");
			printBlock(&stmt->block,indent+1);
			if (stmt->block2.nElements != 0) {
				printIndent(indent);
				printf("ELSE\n");
				printBlock(&stmt->block2,indent+1);
			}
			break;
		case STMT_WHILE:
			printf("WHILE ");
			printExpression(&stmt->rhs);
			printf("\n");
			printBlock(&stmt->block,indent+1);
			break;
		default:
			printf("unknown");
			break;
	}
	printf("\n");
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
		case EXPR_VAL_IDENTIFIER:
			printf("%s",(char *) expr->left);
			break;
		case EXPR_VAL_STRING:
			printf("\"%s\"",(char *) expr->left);
			break;
		case EXPR_VAL_EXPRESSION:
			printExpression(expr->left);
			break;
		default:
			printf(" <unknown %d>",expr->leftType);
	}
	switch (expr->operator) {
		case EXPR_OP_ADD:
			printf(" ADD ");
			break;
		case EXPR_OP_SUB:
			printf(" SUB ");
			break;
		case EXPR_OP_NEG:
			printf(" NEG");
			break;
		case EXPR_OP_AND:
			printf(" AND ");
			break;
		case EXPR_OP_OR:
			printf(" OR ");
			break;
		case EXPR_OP_NOP:
			printf(" <not used>");
			break;
		case EXPR_OP_EQ:
			printf(" EQ ");
			break;
		case EXPR_OP_NE:
			printf(" NE ");
			break;
		case EXPR_OP_GT:
			printf(" GT ");
			break;
		case EXPR_OP_LT:
			printf(" LT ");
			break;
		case EXPR_OP_GE:
			printf(" GE ");
			break;
		case EXPR_OP_LE:
			printf(" LE ");
			break;
		default:
			printf(" <unknown %d>",expr->operator);
			break;
	}
	switch (expr->rightType) {
		case EXPR_VAL_NUMBER:
			{
				int * val = (int *) expr->right;
				printf("%d",*val);
			}
			break;
		case EXPR_VAL_IDENTIFIER:
			printf("%s",(char *) expr->right);
			break;
		case EXPR_VAL_STRING:
			printf("\"%s\"",(char *) expr->right);
			break;
		case EXPR_VAL_EXPRESSION:
			printExpression(expr->right);
			break;
		case EXPR_VAL_UNARY:
			printf(" <not used>");
			break;
		default:
			printf(" <unknown %d>",expr->rightType);
			break;
	}
	printf(")");
}