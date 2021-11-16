#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "pep.h"

//VARIABLES

int msgCount = 0;		// used to define the tag for referencing ASCII memory (msg[msgCount]: ASCII.)
char * tempString;		// temporary pointer for strings
struct varList vars;	// varList holds a list of strings for varaible declaration at the bottom of the code

// Function: pho-constructor function for the varList
void varListInit(struct varList * self) {
	self->varList = (char **)malloc(sizeof(char *) * 10);
    self->allocSize = 10;
    self->size = 0;
};

// Function: add string pointer to the varList
// varList will resize itself if neccesary
void addVar(struct varList * self, char * newVar) {
    // check for allocation size and realloc if needed
	if (self->size == self->allocSize) {
        self->varList = (char **)realloc(self->varList, sizeof(char *) * self->allocSize * 2);
        self->allocSize = self->allocSize * 2;
    }
	// add pointer to the varList and increment size
    self->varList[self->size++] = newVar;
};

// Function: begin printing Pep9 conversion.
void pepProgramTree(struct program * root) {
	// construct varList vars
	varListInit(&vars);
	// header with # of decls
	printf(";C Program to Pep9 (%d declarations)\n",root->nDeclarations);
	// loop through each decl
	for (int i = 0; i < root->nDeclarations; i++) {
		pepDeclaration(&root->declarations[i]);
	}
}

// Function: Translate a declaration struct and its contents
void pepDeclaration(struct declaration * decl) { 
	printf(";Declaration\n");
	switch (decl->declarationType) {
        // write tag and go into main block
		// NOP0 is used to make auto commenting easier
		case DECL_MAIN:
			printf("main:\tNOP0");
			pepBlock(&decl->functionBlock);
			break;
        // for variable decl. outside of main
        case DECL_VARIABLE:
			// no need to check for allocation size as the below sprintf function can only have a max size of 16 chars
			// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
			//	the previous allocation (who's pointer is stored in the vars varList)
			tempString = (char *)malloc(sizeof(char) * 20);
			// insert a formatted string into the new allocation
			sprintf(tempString, "%s:\t.WORD\tx00\n", decl->identifier);
			// add pointer to list
			addVar(&vars, tempString);
			// check if declaration includes additional expression elements ie. x = 2 + 3
			if (decl->init!=NULL) {
				pepExpression(decl->init);
				printf("\tSTWA %s,d\n", decl->identifier);
			}
            break;
        // TODO for method/functions (if implemented)
        case DECL_FUNCTION:
            printf("%s:\t", decl->identifier);
            break;
	}
}

// Function: determine the type of contents in a block and call respective function(s)
void pepBlock(struct block * block) {
	// header
	printf(";Block (%d elements)\n",block->nElements);
	for (int i = 0; i < block->nElements; i++) {
		struct blockElement * elem = &block->elements[i];
		switch (elem->type) {
			case BLCK_DECLARATION:
				pepDeclaration(elem->element);
				break;
			case BLCK_STATEMENT:
				pepStatement(elem->element);
				break;
		}
		printf("\n");
	}
}

// Function: Translate a statment struct and its contents
void pepStatement(struct statement * stmt) {
	printf(";Statement\n");
	switch (stmt->statementType) {
		case STMT_VAR_ASSIGNMENT:
			// call and print expression contents and then store
			pepExpression(&stmt->rhs);
			printf("\tSTWA %s,d\n", stmt->identifier);
			break;
		case STMT_RETURN:
			// only current use for return is to end the program
			printVars(&vars);
			printf(".end");
			break;
		case STMT_PRINTF_CALL:
			// make a STRO statment
			printf("\tSTRO msg%d,d\n", msgCount);
			// call pepExpression to store a string literal in memory
			pepExpression(&stmt->rhs);
			break;
		default:
			error("invalid statement type");
			break;
	}
}

// Function: Print out an expression struct in pep9 syntax
void pepExpression(struct expression * expr) {
		switch (expr->leftType) {
		case EXPR_VAL_NUMBER:
			{
				// load left hand expr into accumulator
				int * val = (int *) expr->left;
				printf("\tLDWA 0x%x,i",*val);
			}
			break;
		case EXPR_VAL_STRING:
			// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory
			// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
			//	the previous allocation (who's pointer is stored in the vars varList)
			tempString = (char *)malloc(sizeof(char) * (30 + strlen((char *)expr->left)));
			// insert a formatted string into the new allocation
			sprintf(tempString, "msg%d:\t.ASCII\t\"%s\\x00\"\n", msgCount++, (char *)expr->left);
			// add pointer to list
			addVar(&vars, tempString);
			break;

		case EXPR_VAL_EXPRESSION:
			// call nested expression
			pepExpression(expr->left);
			break;
		default:
			error("invalid left expression");
			break;
	}
	// check for operator type and print 
	switch (expr->operator) {
		case EXPR_OP_ADD:
			printf("\tADDA");
			break;
		case EXPR_OP_SUB:
			printf("\tSUBA");
			break;
		case EXPR_OP_NEG:
			printf("\tNOTA");
			break;
		case EXPR_OP_AND:
			printf("\tANDA");
			break;
		case EXPR_OP_OR:
			printf("\tORA");
			break;
		case EXPR_OP_NOP:
			break;
		default:
		error("invalid expression operator");
			break;
	}
	switch (expr->rightType) {
		case EXPR_VAL_NUMBER:
			{
				// print value t ouse in operation
				int * val = (int *) expr->left;
				printf("\ti,%x\n",*val);
			}
			break;
		case EXPR_VAL_STRING:
			error("cannot have string as right hand expression value");
			break;
		case EXPR_VAL_EXPRESSION:
			printf("\n");
			pepExpression(expr->right);
			break;
		case EXPR_VAL_UNARY:
			break;
		default:
			error("invalid right expression");
			break;
	}
	printf("\n");
}

// print all the strings stored in varList from pointers
void printVars(struct varList * vars) {
	printf(";Variables/Memory\nSTOP\n");
	for (int i = 0; i < vars->size; i++)
	{
		printf("%s\n", vars->varList[i]);
		free(vars->varList[i]);
	}
}

void error(char * msgOut)
{
	printf("ERROR: %s", msgOut);
	exit(1);
}