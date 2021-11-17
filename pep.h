/*
Program Name: PepC
Program Authors: Andrew "Drew" Dorris, Dante Hays, Charles Kinsler, Matthew Lockard, Liam Whitelaw
Class: CSCI 2160-001
Lab: Project 2
Date: 11/16/2021
Purpose: Convert a small subset of C language source code into Pep/9 assembly source
*/

#ifndef PEP_H
#define PEP_H
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// varList holds a list of strings for varaible declaration at the bottom of the code
struct varList {
    char **varList;
    int size;
    int allocSize;
};

// see pep.c for proper function comments
void pepIndent(int indent);

void pepProgramTree(struct program * root);

void pepDeclaration(struct declaration * decl);

void pepBlock(struct block * block);

void pepStatement(struct statement * stmt);

void pepConExpression(struct expression * expr);

void pepPrintExpression(struct expression * expr);

void pepExpression(struct expression * expr);

void printWords(struct varList * vars);

void printStrings(struct varList * vars);

void printVars(struct varList * vars);

void error(char * msgOut);

#endif