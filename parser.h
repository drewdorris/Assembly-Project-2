#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
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

struct program program(struct parser * self);

struct declaration declaration(struct parser * self);

struct block block(struct parser * self);

struct statement printfParse(struct parser * self);

//Nodes

//Root node for a program containing a series of declarations, one of which is main
struct program {
	int nDeclarations;
	struct declaration * declarations;
};

//A declaration, which could be a variable or a function, especially the main function
#define DECL_MAIN 1 // Declaration of int main()
#define DECL_VARIABLE 2 // Declaration of a variable
#define DECL_FUNCTION 3 // Declaration of a function

struct declaration {
	int declarationType;
	char * identifier; //name of the identifier/var/func being declared.

	//Variable
	int variableType; // may be TYPE_KW_SHORT or TYPE_KW_INT
	struct expression * init; // if present otherwise null e.g. "int x;" is a null pointer, "int x = 5;" has pointer to expression

	//Functions (MAIN ignores return type, identifier, and arguments. Only functionBlock is used.)
	int returnType; // may be TYPE_KW_SHORT or TYPE_KW_INT or TYPE_KW_VOID
	int nArgTypes; // number of arguments
	int * argTypes; // argument types, only TYPE_KW_SHORT or TYPE_KW_INT
	struct block functionBlock; // pointer to code block
};

//A set of {} and the declarations/statements within.
struct block {
	int nElements;
	struct blockElement * elements;
};

//An element in a block. Either a declaration or statement.
#define BLCK_DECLARATION 0
#define BLCK_STATEMENT 1
struct blockElement {
	int type;
	void * element; //must be casted to (struct statement *) or (struct declaration *) depending on type
};

//A statement in C, which is that stuff before the ;
#define STMT_BLOCK 0 // A block is a grouping of statements/decls and is itself a statement as a whole
#define STMT_VAR_ASSIGNMENT 1
#define STMT_FUNCTION_CALL 2
#define STMT_PRINTF_CALL 3
#define STMT_SCANF_CALL 4
#define STMT_RETURN 5
#define STMT_IF 6 // Doesn't end with ; but is functionally the same
#define STMT_WHILE 7 // Doesn't end with ; but is functionally the same
struct statement {
	int statementType;
	//Block statements
	struct block * block;
	//Types with identifiers (var assign, function call, scanf target)
	char * identifer;
	//Types with right-hand sides
	struct expression rhs; // Value of the variable assignment, return number, printf
	//Function calls
	//ignore for now- stretch goal
};

//An expression of variables, constants, and operators
#define EXPR_VAL_UNARY 0 // Left side is alone. There is no right hand. Only unary operators are permitted. Not valid in left side.
#define EXPR_VAL_NUMBER 1
#define EXPR_VAL_STRING 2
#define EXPR_VAL_IDENTIFIER 3 // Variable, hopefully
#define EXPR_VAL_EXPRESSION 4

#define EXPR_OP_NOP 0 //No operation. For constants defined alone i.e. "5"
#define EXPR_OP_ADD 1
#define EXPR_OP_SUB 2
#define EXPR_OP_AND 3
#define EXPR_OP_OR 4
#define EXPR_OP_NEG 5 //Unary

struct expression {
	int leftType;
	void * left; // must be casted to (struct expression *), (int *), or (char *)
	int operator;
	int rightType;
	void * right; // must be casted to (struct expression *), (int *), or (char *)
};

#endif