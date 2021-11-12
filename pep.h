#ifndef PEP_H
#define PEP_H
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

struct varList {
    char **varList;
    int size;
    int allocSize;
};

void pepIndent(int indent);

void pepProgramTree(struct program * root);

void pepDeclaration(struct declaration * decl);

void pepBlock(struct block * block);

void pepStatement(struct statement * stmt);

void pepExpression(struct expression * expr);

void printWords(struct varList * vars);

void printStrings(struct varList * vars);

void printVars(struct varList * vars);

void error(char * msgOut);

#endif