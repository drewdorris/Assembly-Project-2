/*
Program Name: PepC
Program Authors: Andrew "Drew" Dorris, Dante Hays, Charles Kinsler, Matthew Lockard, Liam Whitelaw
Class: CSCI 2160-001
Lab: Project 2
Date: 11/16/2021
Purpose: Convert a small subset of C language source code into Pep/9 assembly source
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "pep.h"

// VARIABLES

int msgCount = 0;		// used to define the tag for referencing ASCII memory (msg[msgCount]: ASCII.)
int conCount = 0;		// used to define the tag for referencing conditional jumps
int depth = 0;			// represents the # of layers the code is into nested structs
int depthMax = 0;		// represents the max depth reached before popping all the way back out
char * tempString;		// temporary pointer for strings
struct varList vars;	// varList holds a list of strings for varaible declaration at the bottom of the code
short hasMultFunc = 0;	// bool; decides whether to add mult function to end or not
const char * multString = ";********* MULTIPLY **********\nretVal:  .EQUATE 12          \nmult1:   .EQUATE 10          \nmult2:   .EQUATE 8           \nm1Sign:  .EQUATE 5           \nm2Sign:  .EQUATE 4           \nk:       .EQUATE 2           \nmresult: .EQUATE 0           \nmultiply:SUBSP   6,i         \n         LDWA    0,i         \n         STWA    result,s    \n         LDBA    0,i         \n         STBA    m1Sign,s    \n         STBA    m2Sign,s    \n         LDWA    1,i         \n         STWA    k,s         \nchckM1:  LDWA    mult1,s     \n         CPWA    0,i         \n         BRGT    chckM2      \nabs1:    NOTA                \n         ADDA    1,i         \n         STWA    mult1,s     \n         LDBA    1,i         \n         STBA    m1Sign,s    \nchckM2:  LDWA    mult2,s     \n         CPWA    0,i         \n         BRGT    forM        \nabs2:    NOTA                \n         ADDA    1,i         \n         STWA    mult2,s     \n         LDBA    1,i         \n         STBA    m2Sign,s    \nforM:    LDWA    k,s         \n         CPWA    mult2,s     \n         BRGT    endForM     \n         LDWA    result,s    \n         ADDA    mult1,s     \n         STWA    result,s    \n         LDWA    k,s         \n         ADDA    1,i         \n         STWA    k,s         \n         BR      forM        \nendForM: LDBA    m1Sign,s    \n         CPBA    m2Sign,s    \n         BREQ    endForM2    \n         LDWA    result,s    \n         NOTA                \n         ADDA    1,i         \n         STWA    result,s    \nendForM2:LDWA    result,s    \n         STWA    retVal,s    \n         LDWA    0,i         \n         STWA    k,s         \n         STWA    result,s    \n         LDBA    0,i         \n         STBA    m1Sign,s    \n         STBA    m2Sign,s    \n         ADDSP   6,i         \n         RET\n";



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
	printf("\n====================\nPep9 Generated Code\n====================\n\n");
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
			printf(";MAIN\nmain:\tNOP0\n");
			pepBlock(&decl->functionBlock);
			break;
        // for variable decl. outside of main
        case DECL_VARIABLE:
			// no need to check for allocation size as the below sprintf function can only have a max size of 16 chars
			// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
			//  the previous allocation (who's pointer is stored in the vars varList)
			tempString = (char *)malloc(sizeof(char) * 20);
			// insert a formatted string into the new allocation
			sprintf(tempString, "%s:\t.WORD\t0x00\n", decl->identifier);
			// add pointer to list
			addVar(&vars, tempString);
			// check if declaration includes additional expression elements ie. x = 2 + 3
			if (decl->init!=NULL) {
				pepExpression(decl->init);
				printf("\tSTWA %s,d\t\t; store word from assembler\n", decl->identifier);
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
			printf("\tSTWA %s,d\t\t; store word from assembler\n", stmt->identifier);
			break;
		case STMT_PRINTF_CALL:
			pepPrintExpression(&stmt->rhs);
			break;
		case STMT_SCANF_CALL:
			printf("\tDECI %s,d\t\t; take in decimal input from terminal, store to memory\n", stmt->identifier);
			break;
		case STMT_IF:
			if (stmt->block2.nElements == 0) {
				// if case
				// Reserve a jump label for our use
				int jumpAfterIf = conCount++;
				// Print jump after the code block if false
				pepConExpression(&stmt->rhs,jumpAfterIf);
				// Print the code inside if
				pepBlock(&stmt->block);
				// Print label to jump to
				printf("\ncon%d: NOP0\t\n", jumpAfterIf);
			} else {
				// if/else case
				// Allocate two jump labels: one for else, one for after the blocks
				int jumpToElse = conCount++;
				int jumpAfterIf = conCount++;
				// Print jump to else if condition is false
				pepConExpression(&stmt->rhs,jumpToElse);
				// Print the if block
				pepBlock(&stmt->block);
				// Jump after the else block so it doesn't run if condition was true
				printf("\n\tBR con%d\n", jumpAfterIf);
				// Print else block label
				printf("\ncon%d: NOP0\t\n", jumpToElse);
				// Print else block
				pepBlock(&stmt->block2);
				// Print label after if/else is done
				printf("\ncon%d: NOP0\t\n", jumpAfterIf);
			}
			break;
		case STMT_RETURN:
			// only current use for return is to end the program
			printVars(&vars);
			printf(".end");
			break;
		case STMT_WHILE:
			{
				int jumpToCondition = conCount++;
				int jumpAfterLoop = conCount++;
				printf("\ncon%d: NOP0\t\n", jumpToCondition);
				pepConExpression(&stmt->rhs,jumpAfterLoop);
				pepBlock(&stmt->block); // loop body
				printf("\n\tBR con%d", jumpToCondition);
				printf("\ncon%d: NOP0\t\n", jumpAfterLoop);
			}
			break;
		default:
			error("invalid statement type");
			break;
	}
}

// Function: condition expression print
// Reserved jump is the label the conditional branch will jump to if condition is false
void pepConExpression(struct expression * expr, int reservedJump) {
	switch (expr->leftType) {
		case EXPR_VAL_NUMBER:
			{
				// load left hand expr into accumulator
				int * val = (int *) expr->left;
				printf("\tLDWA 0x%x,i\n",*val);
			}
			break;
		case EXPR_VAL_STRING:
			error("attempted to pass a string payload into regular pepExpression");
			break;
		case EXPR_VAL_IDENTIFIER:
			printf("\tLDWA %s,d\n", (char *)expr->left);
			break;
		default:
			error("invalid left expression in con");
			break;
	}
	switch (expr->rightType) {
		case EXPR_VAL_NUMBER:
			{
				int * val = (int *) expr->right;
				printf("\tCPWA 0x%x,i\n",*val);
			}
			break;
		case EXPR_VAL_IDENTIFIER:
			printf("\tCPWA %s,d\n", (char *)expr->right);
			break;
		case EXPR_VAL_UNARY:
			printf("\tCPWA 0x1,i\n");
			break;
		default:
			error("invalid right expression in con");
			break;
	}
	switch (expr->operator) {
		//!! Conditional operators are opposite since branching takes place if something is true in Pep9, the opposite of standard if
		case EXPR_OP_EQ:
			printf("\tBRNE con%d\n", reservedJump);
			break;
		case EXPR_OP_NE:
			printf("\tBREQ con%d\n", reservedJump);
			break;
		case EXPR_OP_GE:
			printf("\tBRLT con%d\n", reservedJump);
			break;
		case EXPR_OP_LE:
			printf("\tBRGT con%d\n", reservedJump);
			break;
		case EXPR_OP_GT:
			printf("\tBRLE con%d\n", reservedJump);
			break;
		case EXPR_OP_LT:
			printf("\tBRGE con%d\n", reservedJump);
			break;
		case EXPR_OP_NOP:
			printf("\tBRNE con%d\n", reservedJump);
	}
}

// Function: generate Pep9 code for printing expression struct contents
void pepPrintExpression(struct expression * expr) {
	switch (expr->leftType) {
	case EXPR_VAL_STRING:
		// make a STRO statment for literal
		printf("\tSTRO msg%d,d\t\t; print string literal from STRO msg%d\n", msgCount, msgCount);

		// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
		// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
		//	the previous allocation (who's pointer is stored in the vars varList)
		tempString = (char *)malloc(sizeof(char) * (30 + strlen((char *)expr->left)));
		// insert a formatted string into the new allocation
		sprintf(tempString, "msg%d:\t.ASCII\t\"%s\\x00\"\n", msgCount++, (char *)expr->left);
		// add pointer to list
		addVar(&vars, tempString);
		break;
	case EXPR_VAL_IDENTIFIER:
		// make a DECO statment using identifier
		printf("\tDECO %s,d\t\t; output decimal value to terminal\n", (char *)expr->left);
		break;
	default:
		error("invalid left expression for print");
		break;
	}
	
	switch (expr->rightType) {
		case EXPR_VAL_EXPRESSION:
			pepPrintExpression(expr->right);
			break;
		case EXPR_VAL_UNARY:
			break;
		default:
		error("invalid right expression for print");
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
			error("attempted to pass a string payload into regular pepExpression");
			break;
		case EXPR_VAL_IDENTIFIER:
			printf("\tLDWA %s,d\n", (char *)expr->left);
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
		case EXPR_OP_MULT:
			printf("\tSTWA -4,s\n\tLDWA"); 
			if (!hasMultFunc) {
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(multString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", multString);
				// add pointer to list
				addVar(&vars, tempString);
				// add the whole mult part to the bottom if not added already, as well
				// remember to add resTemp!
				hasMultFunc = 1;
			}
			break;
		default:
		error("invalid expression operator");
			break;
	}
	switch (expr->rightType) {
		case EXPR_VAL_NUMBER:
			{
				// print value after operation
				int * val = (int *) expr->right;
				printf(" 0x%x,i\n",*val);
			}
			break;
		case EXPR_VAL_STRING:
			error("cannot have string as right hand expression value");
			break;
		case EXPR_VAL_IDENTIFIER:
			printf(" %s,d", (char *)expr->right);
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
	// handle additional pep code needed for multiplication usage
	switch (expr->operator) {
		case EXPR_OP_MULT:
			printf("\tSTWA -6,s\n\tSUBSP 6,i\n\tCALL multiply\n\tLDWA 4,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\tADDSP 6,i\n\tLDWA resTemp,d\n");
			break;
		default:
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