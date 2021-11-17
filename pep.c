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

short hasResultTemp = 0; // bool; used for mult/div; temp variable for storing result
short hasMultFunc = 0;	// bool; decides whether to add mult function to end or not. if * has already been called, this will be true
short hasDivFunc = 0; // bool; decides whether to add div func
short hasLeftShiftFunc = 0; // bool; decides whether to add << func
short hasRightShiftFunc = 0; // bool; decides whether to add >> func

const char * multString = ";********* MULTIPLY **********\nretVal:  .EQUATE 12          ;returned value #2d\nmult1:   .EQUATE 10          ;formal parameter #2d\nmult2:   .EQUATE 8           ;formal parameter #2d\nm1Sign:  .EQUATE 5           ;local variable #1d\nm2Sign:  .EQUATE 4           ;local variable #1d\nk:       .EQUATE 2           ;local variable #2d\nmresult: .EQUATE 0           ;local variable; calculated result #2d\nmultiply:SUBSP   6,i         ;push #m1Sign #m2Sign #k #mresult\n         LDWA    0,i         ;reset possible lingering values in the stack before doing operations\n         STWA    mresult,s   ;reset\n         LDBA    0,i         ;reset\n         STBA    m1Sign,s    ;reset\n         STBA    m2Sign,s    ;reset\n         LDWA    1,i         ;reset\n         STWA    k,s         ;reset\nchckM1:  LDWA    mult1,s     ;check mult1 if it is negative\n         CPWA    0,i         ;compare\n         BRGT    chckM2      ;move on if not\nabs1:    NOTA                ;but if so, note that the sign is negative and negate mult1\n         ADDA    1,i         ;negate\n         STWA    mult1,s     ;negate\n         LDBA    1,i         ;note it\n         STBA    m1Sign,s    ;note it\nchckM2:  LDWA    mult2,s     ;check mult2 if it is negative\n         CPWA    0,i         ;\n         BRGT    forM        ;move on if not\nabs2:    NOTA                ;here, note that the sign is negative and negate mult2\n         ADDA    1,i         ;negate\n         STWA    mult2,s     ;negate\n         LDBA    1,i         ;note it\n         STBA    m2Sign,s    ;note it\nforM:    LDWA    k,s         ;load k for comparison if not loaded already, to see if we are done looping yet\n         CPWA    mult2,s     ;see if k <= mult2, which means we have added mult1 to itself mult2 times\n         BRGT    endForM     ;if so, we're done! branch to endForM\n         LDWA    mresult,s   ;if not, we'll keep going! load the current added result to keep adding\n         ADDA    mult1,s     ;add mult1 again\n         STWA    mresult,s   ;store it to result\n         LDWA    k,s         ;load k to add one to it\n         ADDA    1,i         ;add one to it so we can see when we reach mult2 and stop adding\n         STWA    k,s         ;store it to k\n         BR      forM        ;do the loop again!\nendForM: LDBA    m1Sign,s    ;check if the signs of each num are different\n         CPBA    m2Sign,s    ;if so, we set the result as negative\n         BREQ    endForM2    ;\n         LDWA    mresult,s   ;\n         NOTA                ;\n         ADDA    1,i         ;\n         STWA    mresult,s   ;\nendForM2:LDWA    mresult,s   ;load result and store to result variable\n         STWA    retVal,s    ;\n         LDWA    0,i         ;cleanup\n         STWA    k,s         ;reset values\n         STWA    mresult,s   ;reset\n         LDBA    0,i         ;reset\n         STBA    m1Sign,s    ;reset\n         STBA    m2Sign,s    ;reset\n         ADDSP   6,i         ;pop #mresult #k #m1Sign #m2Sign\n         RET\n";
const char * divString = ";********* DIVIDE/MODULO **********\nremaind: .EQUATE 14          ;returned value; remainder/modulo #2d\nretDiv:  .EQUATE 12          ;returned value; the return value/quotient #2d\ndiv1:    .EQUATE 10          ;formal parameter; dividend #2d\ndiv2:    .EQUATE 8           ;formal parameter; divisor #2d\ndiv1Sign:.EQUATE 5           ;local variable #1d\ndiv2Sign:.EQUATE 4           ;local variable #1d\ndk:      .EQUATE 2           ;local variable #2d\ndresult: .EQUATE 0           ;local variable #2d\ndivide:  SUBSP   6,i         ;push #div1Sign #div2Sign #dk #dresult\n         LDWA    0,i         ;reset values to clean up\n         STWA    dresult,s   ;reset\n         STWA    dk,s        ;reset\n         STWA    remaind,s   ;reset\n         LDBA    0,i         ;reset\n         STBA    div1Sign,s  ;reset\n         STBA    div2Sign,s  ;reset\nchckD1:  LDWA    div1,s      ;check for negative values\n         CPWA    0,i         ;this is the exact same code as the mult function above\n         BRGT    chckD2      ;so comments can be viewed there\nabsD1:   NOTA\n         ADDA    1,i\n         STWA    div1,s\n         LDBA    1,i\n         STBA    div1Sign,s\nchckD2:  LDWA    div2,s\n         CPWA    0,i\n         BREQ    divZero\n         BRGT    forD\nabsD2:   NOTA\n         ADDA    1,i\n         STWA    div2,s\n         LDBA    1,i\n         STBA    div2Sign,s\nforD:    LDWA    dk,s        ;dk is div2 being added to itself\n         CPWA    div1,s      ;compare it to div1..\n         BRGE    checkRmd    ;if it is greater than div2, its time to movee on\n         LDWA    dk,s        ;else, add div2 to itself again\n         ADDA    div2,s      ;add\n         STWA    dk,s        ;add\n         LDWA    dresult,s   ;keep track of amnt of times div2 added to itself\n         ADDA    1,i         ;track\n         STWA    dresult,s   ;track\n         BR      forD        ;go back and loop again\ncheckRmd:LDWA    dk,s        ;load dk, greater or equal to div1\n         CPWA    div1,s      ;compare to div1\n         BREQ    endForD     ;if equal, no remainder!\n         LDWA    dresult,s   ;load real result\n         SUBA    1,i         ;subtract by 1\n         STWA    dresult,s   ;store\n         LDWA    dk,s        ;load dk again\n         SUBA    div2,s      ;subtract one instance of div2 from it\n         STWA    dk,s        ;store it\n         LDWA    div1,s      ;load div1 which is now greater than dk if there is a remainder\n         SUBA    dk,s        ;subtract dk for the difference; the remainder\n         STWA    remaind,s   ;store remainder\n         BR      endForD     ;go to end\ndivZero: LDWA    -1,i        ;set to -1\n         STWA    dresult,s   ;set\nendForD: LDBA    div1Sign,s  ;this section checks the signs and sets the result as\n         CPBA    div2Sign,s  ;negative if it should be negative. there are comments\n         BREQ    endForD2    ;about this already for the multiplication function above\n         LDWA    dresult,s\n         NOTA\n         ADDA    1,i\n         STWA    dresult,s\n         LDWA    remaind,s\n         NOTA\n         ADDA    1,i\n         STWA    remaind,s\nendForD2:LDWA    dresult,s   ;load result\n         STWA    retDiv,s    ;store in the result variable\n         LDWA    0,i         ;cleanup\n         STWA    dk,s        ;cleanup\n         STWA    dresult,s   ;cleanup\n         LDBA    0,i         ;cleanup\n         STBA    div1Sign,s  ;cleanup\n         STBA    div2Sign,s  ;cleanup\n         ADDSP   6,i         ;pop #dresult #dk #div2Sign #div1Sign\n         RET\n";
const char * leftShiftString = ";********* LEFT SHIFT ***********\nretAls:  .EQUATE 6           ;returned value from bitwise arith #2d\nals1:    .EQUATE 4           ;formal parameter #2d\nals2:    .EQUATE 2           ;formal parameter #2d\nals:     LDWA    als1,s      ;load first param to A\nalsLoop: LDWA    als1,s      ;load the value in als1\n         ASLA                ;aritmetic left shift with second param\n         STWA    als1,s      ;store to als1\n         LDWA    als2,s      ;load num of shifts to perform\n         SUBA    1,i         ;loop until desired shifts are done\n         STWA    als2,s      \n         CPWA    0,i         \n         BRGT    alsLoop     \n         LDWA    als1,s      ;load the value in als1\n         STWA    retAls,s    ;store result in retAls\n         RET";
const char * rightShiftString = ";********* ARITHMETIC RIGHT SHIFT ***********\nretArs:  .EQUATE 6           ;returned value from bitwise arith #2d\nars1:    .EQUATE 4           ;formal parameter #2d\nars2:    .EQUATE 2           ;formal parameter #2d\nars:     LDWA    ars1,s      ;load first param to A\narsLoop: LDWA    ars1,s      ;load the value in ars1\n         ASRA                ;aritmetic right shift\n         STWA    ars1,s      ;store to ars1\n         LDWA    ars2,s      ;load num of shifts to perform\n         SUBA    1,i         ;loop until desired shifts are done\n         STWA    ars2,s      \n         CPWA    0,i         \n         BRGT    arsLoop     \n         LDWA    ars1,s      ;load the value in ars1\n         STWA    retArs,s    ;store result in retArs\n         RET                 \n\n;********* LOGICAL RIGHT SHIFT ***********\nretLrs:  .EQUATE 6           ;returned value from bitwise arith #2d\nlrs1:    .EQUATE 4           ;formal parameter #2d\nlrs2:    .EQUATE 2           ;formal parameter #2d\nlrs:     LDWA    lrs1,s      ;load first param to A\n         CPWA    0,i         ;see if it is negative\n         BRGT    isPos       ;if not branch to perform the shift normally\n         NEGA                ;else negate it to positive\nisPos:   STWA    lrs1,s      ;store to lrs1\n\nlrsLoop: LDWA    lrs1,s      ;load the value in lrs1\n         ASRA                ;aritmetic right shift\n         STWA    lrs1,s      ;store to lrs1\n         LDWA    lrs2,s      ;load num of shifts to perform\n         SUBA    1,i         ;loop until desired shifts are done\n         STWA    lrs2,s      \n         CPWA    0,i         \n         BRGT    lrsLoop     \n         LDWA    lrs1,s      ;load the value in the lrs1\n         STWA    retLrs,s    ;store result in retLrs\n         RET";
const char * resultTempString = "resTemp: .BLOCK 2\n";


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
			// store first value in the stack
			printf("\tSTWA -4,s\n\tLDWA"); 
			if (!hasResultTemp) {
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(resultTempString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", resultTempString);
				// add pointer to list
				addVar(&vars, tempString);
				hasResultTemp = 1;
			}
			if (!hasMultFunc) {
				// add the whole mult part to the bottom if not added already, as well
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(multString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", multString);
				// add pointer to list
				addVar(&vars, tempString);
				hasMultFunc = 1;
			}
			break;
		case EXPR_OP_MOD:
		case EXPR_OP_DIV:
			// store first value in the stack
			printf("\tSTWA -6,s\n\tLDWA"); 
			if (!hasResultTemp) {
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(resultTempString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", resultTempString);
				// add pointer to list
				addVar(&vars, tempString);
				hasResultTemp = 1;
			}
			if (!hasDivFunc) {
				// add the whole mult part to the bottom if not added already, as well
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(divString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", divString);
				// add pointer to list
				addVar(&vars, tempString);
				hasDivFunc = 1;
			}
			break;
		case EXPR_OP_LS:
			// store first value in the stack
			printf("\tSTWA -4,s\n\tLDWA"); 
			if (!hasResultTemp) {
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(resultTempString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", resultTempString);
				// add pointer to list
				addVar(&vars, tempString);
				hasResultTemp = 1;
			}
			if (!hasLeftShiftFunc) {
				// add the whole mult part to the bottom if not added already, as well
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(leftShiftString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", leftShiftString);
				// add pointer to list
				addVar(&vars, tempString);
				hasLeftShiftFunc = 1;
			}
			break;
		case EXPR_OP_RS:
			// store first value in the stack
			printf("\tSTWA -4,s\n\tLDWA"); 
			if (!hasResultTemp) {
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(resultTempString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", resultTempString);
				// add pointer to list
				addVar(&vars, tempString);
				hasResultTemp = 1;
			}
			if (!hasRightShiftFunc) {
				// add the whole mult part to the bottom if not added already, as well
				// Add 30 to the length of the string (length of string with empty expr string) for n characters and allocate memory.
				// additionally, using malloc without free will give a new allocation to the tempString pointer without removing
				//	the previous allocation (who's pointer is stored in the vars varList)
				tempString = (char *)malloc(sizeof(char) * (30 + strlen(rightShiftString)));
				// insert a formatted string into the new allocation
				sprintf(tempString, "%s", rightShiftString);
				// add pointer to list
				addVar(&vars, tempString);
				hasRightShiftFunc = 1;
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
			printf(" %s,d\n", (char *)expr->right);
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
			// ending of calling multiply function
			printf("\tSTWA -6,s\n\tSUBSP 6,i\t;push #retVal #mult1 #mult2\n\tCALL multiply\n\tLDWA 4,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\tADDSP 6,i\t;pop #retVal #mult1 #mult2\n\tLDWA resTemp,d\n");
			break;
		case EXPR_OP_DIV:
			// end of div calling
			printf("\tSTWA -8,s\n\tSUBSP 8,i\t;push #remaind #retVal #div1 #div2\n\tCALL divide\n\tLDWA 4,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\t STWA 6,s\n\tADDSP 8,i\t;pop #remaind #retVal #mult1 #mult2\n\tLDWA resTemp,d");
			break;
		case EXPR_OP_MOD:
			// end of modulo calling
			printf("\tSTWA -8,s\n\tSUBSP 8,i\t;push #remaind #retVal #div1 #div2\n\tCALL divide\n\tLDWA 6,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\t STWA 6,s\n\tADDSP 8,i\t;pop #remaind #retVal #mult1 #mult2\n\tLDWA resTemp,d");
			break;
		case EXPR_OP_LS:
			// end of left shift calling
			printf("\tSTWA -6,s\n\tSUBSP 6,i\t;push #retVal #als1 #als2\n\tCALL als\n\tLDWA 4,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\t STWA 6,s\n\tADDSP 6,i\t;pop #retVal #mult1 #mult2\n\tLDWA resTemp,d");
			break;
		case EXPR_OP_RS:
			// end of right shift calling
			{
				printf("\tSTWA -6,s\n\tSUBSP 6,i\t;push #remaind #retVal #ars1 #ars2\n\t");
				int * val = (int *) expr->left;
				if (val < 0) {
					printf("CALL ars\n\t");
				} else {
					printf("CALL lrs\n\t");
				}
				printf("LDWA 4,s\n\tSTWA resTemp,d\n\tLDWA 0,i\n\tSTWA 2,s\n\tSTWA 4,s\n\t STWA 6,s\n\tADDSP 6,i\t;pop #retVal #ars1 #ars2\n\tLDWA resTemp,d");
			}
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