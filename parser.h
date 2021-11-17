/*
Program Name: PepC
Program Authors: Andrew "Drew" Dorris, Dante Hays, Charles Kinsler, Matthew Lockard, Liam Whitelaw
Class: CSCI 2160-001
Lab: Project 2
Date: 11/16/2021
Purpose: Convert a small subset of C language source code into Pep/9 assembly source
*/

#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
#include "token.h"

// State for the token parser
struct parser {
	struct token * tokenArray; // array of tokens to handle from
	int tokenArrayLength; // length of the above array
	int currentToken; // current token parser is looking at
};

// Call a parser test
void parserDebug(void);
// Initialise default parser state. Must assign token array/length seperately.
void parserInit(struct parser * self);
// Returns true value if there are tokens left in the array.
int parserHasNext(struct parser * self);
// Advance to the next token, or emit an error at the end of the sequence.
int parserNext(struct parser * self);
// Return true if the current token is of the provided type.
int parserLookaheadIs(struct parser * self, int type);
// Return the current token.
struct token * parserLookahead(struct parser * self);
// Consume the current token if the current token is the provided type, otherwise emit an error. After returning the parser will be on the next
// token.
void parserExpectOrError(struct parser * self, int type);
// Emit an error.
void parserError(struct parser * self);

// Rules

// Process a full program at the lookahead.
struct program program(struct parser * self);
// Process a declaration at the lookahead.
struct declaration declaration(struct parser * self);
// Process a block at the lookahead.
struct block block(struct parser * self);
// Process a expression at the lookahead.
struct expression expression(struct parser * self);
// Process a printf-body at the lookahead.
struct statement printfParse(struct parser * self);

// Nodes

// Root node for a program containing a series of declarations, one of which is main
struct program {
	int nDeclarations;
	struct declaration * declarations;
};

// A set of {} and the declarations/statements within.
struct block {
	int nElements;
	struct blockElement * elements;
};

// An element in a block. Either a declaration or statement.
#define BLCK_DECLARATION 0
#define BLCK_STATEMENT 1
struct blockElement {
	int type; //whether this is a declaration or a statement
	void * element; //must be casted to (struct statement *) or (struct declaration *) depending on type
};

// An expression of variables, constants, and operators
#define EXPR_VAL_UNARY 0 // Left side is alone. There is no right hand. Only unary operators are permitted. Not valid in left side.
#define EXPR_VAL_NUMBER 1
#define EXPR_VAL_STRING 2
#define EXPR_VAL_IDENTIFIER 3 // Variable, hopefully
#define EXPR_VAL_EXPRESSION 4 // Nested expression.

#define EXPR_OP_NOP 0 // No operation. For constants defined alone i.e. "5"
#define EXPR_OP_ADD 1
#define EXPR_OP_SUB 2
#define EXPR_OP_AND 3
#define EXPR_OP_OR 4
#define EXPR_OP_NEG 5 // Unary operator. Right side not used. Bitwise negation.

struct expression {
	int leftType;
	void * left; // must be casted to (struct expression *), (int *), or (char *)
	int operator;
	int rightType;
	void * right; // must be casted to (struct expression *), (int *), or (char *)
};

// A declaration, which could be a variable or a function, especially the main function
#define DECL_MAIN 1 // Declaration of int main()
#define DECL_VARIABLE 2 // Declaration of a variable
#define DECL_FUNCTION 3 // Declaration of a function

struct declaration {
	int declarationType;
	char * identifier; // name of the identifier/var/func being declared.

	// Variable
	int variableType; // may be TYPE_KW_SHORT or TYPE_KW_INT
	struct expression * init; // if present otherwise null e.g. "int x;" is a null pointer, "int x = 5;" has pointer to expression

	// Functions (MAIN ignores return type, identifier, and arguments. Only functionBlock is used.)
	int returnType; // may be TYPE_KW_SHORT or TYPE_KW_INT or TYPE_KW_VOID
	int nArgTypes; // number of arguments
	int * argTypes; // argument types, only TYPE_KW_SHORT or TYPE_KW_INT
	struct block functionBlock; // pointer to code block
};

// A statement in C, which is that stuff before the ;
#define STMT_BLOCK 0 // A block is a grouping of statements/decls and is itself a statement as a whole
#define STMT_VAR_ASSIGNMENT 1 // Assignment to a variable, i.e. x = x + 1
#define STMT_FUNCTION_CALL 2 // A function call like subroutine()
#define STMT_PRINTF_CALL 3 // A call to printf
#define STMT_SCANF_CALL 4 // A call to scanf
#define STMT_RETURN 5 // A return statement
#define STMT_IF 6 // Doesn't end with ; but is functionally the same. If-block
#define STMT_WHILE 7 // Doesn't end with ; but is functionally the same. While-block
struct statement {
	int statementType;
	// Block statements
	struct block * block;
	// Types with identifiers (var assign, function call, scanf target)
	char * identifier;
	// Types with right-hand sides
	struct expression rhs; // Value of the variable assignment, return number, printf
	// Function calls
	// ignore for now- stretch goal
	// actually, should this be an expression? how should this appear here? should we even care?
};

// Print an indent level
void printIndent(int indent);
// Print the program tree
void printProgramTree(struct program * root);
// Print a declaration
void printDeclaration(struct declaration * decl, int indent);
// Print a block
void printBlock(struct block * block, int indent);
// Print a statement
void printStatement(struct statement * stmt, int indent);
// Print an expression
void printExpression(struct expression * expr);

#endif