#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pep.h"

//VARIABLES
int msgCount = 0;

void pepProgramTree(struct program * root) {
	printf("#C Program to Pep9 (%d declarations)\n",root->nDeclarations);
	for (int i = 0; i < root->nDeclarations; i++) {
		pepDeclaration(&root->declarations[i]);
	}
}

void pepDeclaration(struct declaration * decl) {
	printf("#Declaration\n");
	switch (decl->declarationType) {
        // write tag and go into main block
		case DECL_MAIN:
			printf("main:\tNOP0");
			pepBlock(&decl->functionBlock);
			break;
        // for variable decl. outside of main
        case DECL_VARIABLE:
            printf("%s:\t.WORD\n", decl->identifier);
            break;
        // for method/functions if implemented
        case DECL_FUNCTION:
            printf("%s:\t", decl->identifier);
            break;
	}
}

void pepBlock(struct block * block) {
	printf("#Block (%d elements)\n",block->nElements);
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
	printf("#Statement\n");
	switch (stmt->statementType) {
		case STMT_RETURN:
			printf(".end");
			pepExpression(&stmt->rhs);
			break;
		case STMT_PRINTF_CALL:
			printf("\tSTRO msg%d\n", msgCount);
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

				printf("\tLDWA i,%x\n",*val);
			}
			break;
		case EXPR_VAL_STRING:
			printf("msg%d: \"%s\\x00\"\n",msgCount++, (char *)expr->left);
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
			error("expression right not implemented");
			break;
		default:
			error("invalid right expression");
			break;
	}
	printf("\n");
}

void error(char * msgOut)
{
	printf("ERROR: %s", msgOut);
	exit(1);
}