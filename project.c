#include "parser.h"
#include "token.h"
#include "pep.h"
#include <stdio.h>
#include <string.h>

#define MAX_FILE_LENGTH 2000 // maximum amnt of characters in the file

int main(int argc, char *argv[]) {
	if (1) {
		//parserDebug();
		//return 0;
	}

	// stores all input
	char all[MAX_FILE_LENGTH];
	
	// bool: tracks whether the previous char was a newline
	short afterNewLine = 1; //the start of the file might as well be a new line
	// bool: tracks whether we are ignoring the current line we are on
	short ignoreLine = 0;
	// stores current char
	char ch;
	// used as index to place chars in appropriate place in all[]
	int current = 0;
	// https://stackoverflow.com/questions/15883568/reading-from-stdin
	// iterate each char in input
	while(fread(&ch, sizeof(char), 1, stdin) > 0)
	{
		// if we're ignoring this line and not on a new line, continue ignoring it
		if (ignoreLine == 1 && ch != '\n') {
			continue;
		// stop ignoring once we hit a new line
		} else if (ignoreLine == 1 && ch == '\n') {
			ignoreLine = 0;
			afterNewLine = 1;
			continue;
		}
		// if we hit a C directive, we're ignoring this next line
		if (ch == '#' && afterNewLine == 1) {
			ignoreLine = 1;
			afterNewLine = 0;
			continue;
		}
		// previous character no longer a newline
		afterNewLine = 0;
		// remove tab and newline chars
		if (ch == '\t') {
			continue;
		}
		if (ch == '\n') {
			// if a newline, keep track of that for when checking the next char
			afterNewLine = 1;
			continue;
		}
		// place current char in the complete char[] of all input
		all[current] = ch;
		// then iterate
		current = current + 1;
	}
	all[current] = '\0'; //null insert
	printf("\nTotal characters: %i", current);
	printf("\nCode: %s\n", all);
	
	// array is now set up, future expansion here
	
	tokenize(all, (current-1));
	struct token *tokenarray;
	get_token_array(1, &tokenarray);
	for (int i = 0; i < getTokenCount(); i++ ){
		printf("%s\n", tokenTypeString(tokenarray[i].type));
	}
	
	//Invoke parser
	struct parser parser;
	parserInit(&parser);
	parser.tokenArray = tokenarray;
	parser.tokenArrayLength = getTokenCount();
	struct program prgm = program(&parser);
	printProgramTree(&prgm);

	//Invoke codegen
	pepProgramTree(&prgm);
	return 0;
}
