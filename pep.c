#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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
			printf("main:\t");
			pepBlock(&decl->functionBlock);
			break;
        // for variable decl. outside of main
        case DECL_VARIABLE:
            //TODO
            break;
        // for method/functions if implemented
        case DECL_FUNCTION:
            //TODO
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
				printStatement(elem->element);
				break;
		}
		printf("\n");
	}
}

void printStatement(struct statement * stmt) {
	
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