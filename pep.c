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
				pepStatement(elem->element);
				break;
		}
		printf("\n");
	}
}

void pepStatment(struct statement * stmt) {
	printf("#Statement\n");
	switch (stmt->statementType) {
		case STMT_RETURN:
			printf("RETURN ");
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

				printf("\tLDWA %x",*val);
			}
			break;
		case EXPR_VAL_STRING:
			printf("msg%d: \"%s\\x00\"\n",msgCount++, expr->left);
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
			printf(" ADDA");
			break;
		case EXPR_OP_SUB:
			printf(" SUBA");
			break;
		case EXPR_OP_NEG:
			printf(" NEGA"); //??? may need to account for 2's compliment
			break;
		case EXPR_OP_AND:
			printf(" ANDA");
			break;
		case EXPR_OP_OR:
			printf(" ORA");
			break;
		default:
		error("invalid expression operator");
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