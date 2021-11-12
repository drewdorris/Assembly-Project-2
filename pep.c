#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "pep.h"

//VARIABLES
int msgCount = 0;
char * tempString;
int tempSize;
struct varList vars;

void varListInit(struct varList * self) {
	self->varList = (char **)malloc(sizeof(char *) * 10);
    self->allocSize = 10;
    self->size = 0;
};

void addVar(struct varList * self, char * newVar) {
    if (self->size == self->allocSize) {
        self->varList = (char **)realloc(self->varList, sizeof(char *) * self->allocSize * 2);
        self->allocSize = self->allocSize * 2;
    }
    self->varList[self->size++] = newVar;
};

// Function: begin printing Pep9 conversion.
void pepProgramTree(struct program * root) {
	tempString = (char *)malloc(sizeof(char *) * 50);
	int tempSize = 50;
	varListInit(&vars);
	printf(";C Program to Pep9 (%d declarations)\n",root->nDeclarations);
	for (int i = 0; i < root->nDeclarations; i++) {
		pepDeclaration(&root->declarations[i]);
	}
}

void pepDeclaration(struct declaration * decl) {
	printf(";Declaration\n");
	switch (decl->declarationType) {
        // write tag and go into main block
		case DECL_MAIN:
			printf("main:\tNOP0");
			pepBlock(&decl->functionBlock);
			break;
        // for variable decl. outside of main
        case DECL_VARIABLE:
			// no need to check for allocation size as the below sprintf function can only have a max size of 16 chars
			sprintf(tempString, "%s:\t.WORD\n", decl->identifier);
			addVar(&vars, tempString);
			if (decl->init!=NULL) {
				pepExpression(decl->init);
				printf("\tSTWA %s,d\n", decl->identifier);
			}
            break;
        // for method/functions if implemented
        case DECL_FUNCTION:
            printf("%s:\t", decl->identifier);
            break;
	}
}

void pepBlock(struct block * block) {
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

void pepStatement(struct statement * stmt) {
	printf(";Statement\n");
	switch (stmt->statementType) {
		case STMT_VAR_ASSIGNMENT:
			pepExpression(&stmt->rhs);
			printf("\tSTWA %s,d\n", stmt->identifier);
			break;
		case STMT_RETURN:
			printVars(&vars);
			printf("STOP\n.end");
			break;
		case STMT_PRINTF_CALL:
			printf("\tSTRO msg%d,d\n", msgCount);
			pepExpression(&stmt->rhs);
			break;
		default:
			error("invalid statement type");
			break;
	}
}

void pepExpression(struct expression * expr) {
	switch (expr->leftType) {
		case EXPR_VAL_NUMBER:
			{
				int * val = (int *) expr->left;

				printf("\tLDWA 0x%x,i",*val);
			}
			break;
		case EXPR_VAL_STRING:
			if (tempSize <= (30 + strlen((char *)expr->left))) {
				tempString = realloc(tempString, sizeof(char *) * tempSize);
				tempSize = tempSize * 2;
			}
			sprintf(tempString, "msg%d:\t.ASCII\t\"%s\\x00\"\n", msgCount++, (char *)expr->left);
			addVar(&vars, tempString);
			break;

		case EXPR_VAL_EXPRESSION:
			pepExpression(expr->left);
			break;
		default:
			error("invalid left expression");
			break;
	}
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

void printVars(struct varList * vars) {
	printf("STOP\n");
	for (int i = 0; i < vars->size; i++)
	{
		printf("%s\n", vars->varList[i]);
	}
}

void error(char * msgOut)
{
	printf("ERROR: %s", msgOut);
	exit(1);
}