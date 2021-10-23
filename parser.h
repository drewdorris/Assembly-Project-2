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

//Nodes

//Root node for a program containing a series of declarations, one of which is main
struct program {
	int nDeclarations;
	struct declaration * array;
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
	struct expression * initialiser; // if present otherwise null e.g. "int x;" is a null pointer, "int x = 5;" has pointer to expression

	//Functions (MAIN ignores return type, identifier, and arguments. Only functionBlock is used.)
	int returnType; // may be TYPE_KW_SHORT or TYPE_KW_INT or TYPE_KW_VOID
	int nArgs; // number of arguments
	int * argTypes; // argument types, only TYPE_KW_SHORT or TYPE_KW_INT
	struct block * functionBlock; // pointer to code block
};

//A set of {} and the statements within.
struct block {
	int nStatements;
	struct statement * statements;
};

//A statement in C, which is that stuff before the ;
#define STMT_VAR_DECLARATION 1
struct statement {

};

#endif