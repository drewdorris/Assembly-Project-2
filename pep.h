#ifndef PEP_H
#define PEP_H
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

void pepIndent(int indent);

void pepProgramTree(struct program * root);

void pepDeclaration(struct declaration * decl);

void pepBlock(struct block * block);

void pepStatement(struct statement * stmt);

void pepExpression(struct expression * expr);

void error(char * msgOut);

#endif