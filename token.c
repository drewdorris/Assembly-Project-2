#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// array of tokens found from the tokenization of the input file
struct token tokens[1000];
// count of the number of tokens in the token array
int tokencount = 0;

// Initialise token state
void tokenInit(struct token * self) {
	self->type = TYPE_WHITESPACE;
	self->payload = NULL;
}

// returns the token count
int getTokenCount() {
	return tokencount;
}

// loops through the char array stored from reading in the file and generates the appropriate tokens
void tokenize(char *argv, int size) {
	for (int i = 0; i <= size; i++) {
		switch (argv[i]) {
			
			// whitespace is discarded	
			case ' ':
			{
				break;
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'i': 
			{	
				// checks the input and compares to "int" to generate int kw token
				if (tryForString(argv,i,size,"int")) {
					pushTokenType(TYPE_KW_INT);
					i = i + 2;
					break;
				// checks the input and compares to "if" to generate if kw token	
				} else if (tryForString(argv,i,size,"if")) {
					struct token token;
					pushTokenType(TYPE_KW_IF);
					i = i + 1;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'm':
			{	
				// checks the input and compares to "main" to generate main kw token
				if (tryForString(argv,i,size,"main")) {
					pushTokenType(TYPE_KW_MAIN);
					i = i + 3;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'e':
			{
				// checks the input and compares to "else" to generate else kw token
				if (tryForString(argv,i,size,"else")) {
					pushTokenType(TYPE_KW_ELSE);
					i = i + 3;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'r':
			{
				// checks the input and compares to "return" to generate return kw token
				if (tryForString(argv,i,size,"return")) {
					pushTokenType(TYPE_KW_RETURN);
					i = i + 5;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'v':
			{
				// checks the input and compares to "void" to generate void kw token
				if (tryForString(argv,i,size,"void")) {
					pushTokenType(TYPE_KW_VOID);
					i = i + 3;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 's':
			{
				// checks the input and compares to "scanf" to generate scanf kw token
				if (tryForString(argv,i,size,"scanf")) {
					pushTokenType(TYPE_KW_SCANF);
					i = i + 4;
					break;
				}
				// checks the input and compares to "short" to generate short kw token
				if (tryForString(argv,i,size,"short")) {
					pushTokenType(TYPE_KW_SHORT);
					i = i + 4;
					break;
				}
			}
			
			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'w':
			{
				// checks the input and compares to "while" to generate while kw token
				if (tryForString(argv,i,size,"while")) {
					pushTokenType(TYPE_KW_WHILE);
					i = i + 4;
					break;
				}
			}

			// upon finding the first char of a keyword, then compares to see if the keyword was found	
			case 'p':
			{
				// checks the input and compares to "printf" to generate printf kw token
				if (tryForString(argv,i,size,"printf")) {
					pushTokenType(TYPE_KW_PRINTF);
					i = i + 5;
					break;
				}
			}

			// upon finding punctuation, generates the appropriate token  	
			case ',':
			{
				pushTokenType(TYPE_COMMA);
				break;
			}
			
			// upon finding punctuation, generates the appropriate token  	
			case ';':
			{
				struct token token;
				pushTokenType(TYPE_SEMI);
				break;
			}
			
			// upon finding an operator, generates the appropriate token	
			case '+':
			{
				pushTokenType(TYPE_ADD);
				break;
			}

			// upon finding an operator, generates the appropriate token	
			case '-':
			{
				pushTokenType(TYPE_SUB);
				break;
			}

			// upon finding an operator, generates the appropriate token	
			case '&':
			{
				pushTokenType(TYPE_AND);
				break;
			}

			// upon finding an operator, generates the appropriate token	
			case '|':
			{
				pushTokenType(TYPE_OR);
				break;
			}

			case '~':
			{
				pushTokenType(TYPE_NEG);
				break;
			}

			// upon finding an operator, generates the appropriate token	
			case '=':
			{	
				// if the equality operator '==' is found, generate the appropriate token
				if (tryForString(argv,i,size,"==")) {
					pushTokenType(TYPE_CD_EQUAL);
					i = i + 1;
					break;
				}
				// else if the assignment operator '=' is found, generate the appropriate token
				pushTokenType(TYPE_ASSIGN);
				break;
			}

			// upon finding a conditional operator, generates the appropriate token	
			case '<':
			{
				if (tryForString(argv,i,size,"<=")) {
					pushTokenType(TYPE_CD_LESS_OR_EQUAL);
					i = i + 1;
					break;
				}
				pushTokenType(TYPE_CD_LESS);
				break;
			}

			// upon finding a conditional operator, generates the appropriate token	
			case '>':
			{
				if (tryForString(argv,i,size,">=")) {
					pushTokenType(TYPE_CD_GREATER_OR_EQUAL);
					i = i + 1;
					break;
				}
				pushTokenType(TYPE_CD_GREATER);
				break;
			}
			
			// upon finding a conditional operator, generates the appropriate token	
			case '!':
			{
				//if the negate operator '!=', generate the appropriate token
				if (tryForString(argv,i,size,"!=")) {
					pushTokenType(TYPE_CD_NOT_EQUAL);
					i = i + 1;
					break;
				}
			}

			// upon finding punctuation, generates the appropriate token  	
			case '(':
			{
				pushTokenType(TYPE_LEFT_PAREN);
				break;
			}

			// upon finding punctuation, generates the appropriate token  	
			case ')':
			{
				pushTokenType(TYPE_RIGHT_PAREN);
				break;
			}

			// upon finding punctuation, generates the appropriate token  	
			case '{':
			{
				pushTokenType(TYPE_LEFT_BRACE);
				break;
			}

			// upon finding punctuation, generates the appropriate token  	
			case '}':
			{
				pushTokenType(TYPE_RIGHT_BRACE);
				break;
			}
			
			// upon finding a double quote, all input will be captured until a second double quote is found,
			// this is then stored as the payload of a string token	
			case '"':
			{
				int index = findIndexOfNextChar(argv,i,size,'"');
				if (index != -1) {
					char * stringparse = malloc(sizeof(char) * (index - i - 1)); // array for substring. add 1 for null terminator and subtract 2 for no quotes
					memcpy(stringparse, &argv[i + 1], index - i - 1); // substrings argv and sets as stringparse
					stringparse[index - i - 2] = '\0'; // add null terminator to end of 

					// make Token and push it
					struct token Token;
					Token.payload = stringparse;
					Token.type = TYPE_STRING;
					pushToken(Token);

					i = index - 1;
					break;
				}
				break;
			}

			default:
			{
				// number parsing
				// if a number or an identifier aren't found, the char is simply ignored
				if (isdigit(argv[i])) {
					int index = findIndexOfNextNonNumericChar(argv,i,size);
					if (index != -1) {
						char * numberparse = malloc(sizeof(char) * (index - i)); // array for substring. add 1 for null terminator
						memcpy(numberparse, &argv[i], index - i); // substrings argv and sets as stringparse

						// make Token and push it
						struct token Token;
						int * intPayload = malloc(sizeof(int));
						*intPayload = atoi(numberparse);
						Token.payload = intPayload;
						Token.type = TYPE_NUMBER;
						pushToken(Token);

						i = index - 1;
						break;
					}
				// else, likely identifier parsing
				} else {
					int index = findIndexOfNextNonAlphanumericChar(argv,i,size);
					if (index != -1) {
						char * identifierparse = malloc(sizeof(char) * (index - i)); // array for substring. add 1 for null terminator
						memcpy(identifierparse, &argv[i], index - i); // substrings argv and sets as stringparse

						// make Token and push it
						struct token Token;
						Token.payload = identifierparse;
						Token.type = TYPE_IDENTIFIER;
						pushToken(Token);

						i = index - 1;
						break;
					}
				}
				break;
			}
		}
	}
}
	
// Getter for array of tokens
// https://stackoverflow.com/questions/9914122/getting-an-array-from-another-file-in-c/9914238
void get_token_array(int which, struct token **buffer) {
	if(which == 1) *buffer = tokens;
}


/*
* Dynamically sizing array
* https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
*/

/*
	void initArray(Array *a, size_t initialSize) {
		a->array = malloc(initialSize * sizeof(int));
		a->used = 0;
		a->size = initialSize;
	}

	void insertArray(Array *a, int element) {
		// a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
		// Therefore a->used can go up to a->size
		if (a->used == a->size) {
			a->size *= 2;
			a->array = realloc(a->array, a->size * sizeof(int));
		}
		a->array[a->used++] = element;
	}

	void freeArray(Array *a) {
		free(a->array);
		a->array = NULL;
		a->used = a->size = 0;
	}
	*/

// Print a human-readable token type from an ID
char * tokenTypeString(int typeId) {
	switch (typeId) {
		case TYPE_WHITESPACE: return "WHITESPACE";
		//Valued tokens
		case TYPE_NUMBER: return "NUMBER";
		case TYPE_IDENTIFIER: return "IDENTIFIER";
		case TYPE_STRING: return "STRING";
		//Punctuations
		case TYPE_LEFT_PAREN: return "LEFT_PAREN";
		case TYPE_RIGHT_PAREN: return "RIGHT_PAREN";
		case TYPE_LEFT_BRACE: return "LEFT_BRACE";
		case TYPE_RIGHT_BRACE: return "RIGHT_BRACE";
		case TYPE_COMMA: return "COMMA";
		case TYPE_SEMI: return "SEMI";
		case TYPE_QUOTE: return "QUOTE";
		//Operators
		case TYPE_ADD: return "ADD";
		case TYPE_SUB: return "SUB";
		case TYPE_AND: return "AND";
		case TYPE_OR: return "OR";
		case TYPE_NEG: return "NEG";
		case TYPE_ASSIGN: return "ASSIGN";
		//Conditions
		case TYPE_CD_EQUAL: return "CD_EQUAL";
		case TYPE_CD_LESS: return "CD_LESS";
		case TYPE_CD_GREATER: return "CD_GREATER";
		case TYPE_CD_NOT_EQUAL: return "CD_NOT_EQUAL";
		case TYPE_CD_LESS_OR_EQUAL: return "CD_LESS_OR_EQUAL";
		case TYPE_CD_GREATER_OR_EQUAL: return "CD_GREATER_OR_EQUAL";
		//Keywords and identifiers that we can cheat as keywords
		case TYPE_KW_INT: return "KW_INT";
		case TYPE_KW_SHORT: return "KW_SHORT";
		case TYPE_KW_VOID: return "KW_VOID";
		case TYPE_KW_RETURN: return "KW_RETURN";
		case TYPE_KW_IF: return "KW_IF";
		case TYPE_KW_ELSE: return "KW_ELSE";
		case TYPE_KW_MAIN: return "KW_MAIN";
		case TYPE_KW_SCANF: return "KW_SCANF";
		case TYPE_KW_PRINTF: return "KW_PRINTF";
		case TYPE_KW_WHILE: return "KW_WHILE";
		default: return "UNKNOWN";
	}

}

//will attempt to assemble and compare a target string for equality to see if it is found in the passed input
int tryForString(char * buffer, int position, int size, char * target) {
	int targetLen = strlen(target);
	for (int i = 0; i < targetLen; i++) {
		if (position+1 >= size) return 0; //would bounds the array so cannot succeed in any case
		if (buffer[position+i] != target[i]) return 0;
	}
	return 1;
}

//will return the index of the next specified char from the passed input
int findIndexOfNextChar(char * buffer, int position, int size, char target) {
	while ((position + 1) < size) {
		position = position + 1;
		if (buffer[position] == target) return (position + 1);
	}
	return -1;
}

//will return the index of the next Non-Alphanumeric char from the passed input
int findIndexOfNextNonAlphanumericChar(char * buffer, int position, int size) {
	while ((position + 1) < size) {
		position = position + 1;
		if (!isdigit(buffer[position]) && !isalpha(buffer[position]) && buffer[position] != '_') {
			return position;
		}
	}
	return -1;
}

//will return the index of the next Non-Numeric char from the passed input
int findIndexOfNextNonNumericChar(char * buffer, int position, int size) {
	while ((position + 1) < size) {
		position = position + 1;
		if (!isdigit(buffer[position])) {
			return position;
		}
	}
	return -1;
}

//pushes the passed token to the token array and increments the count
void pushToken(struct token tok) {
	tokens[tokencount] = tok;
	tokencount++;
}

//generates a new token of the passed type before adding it to the token array and incrementing the count
void pushTokenType(int type) {
	struct token token;
	token.type = type;
	tokens[tokencount] = token;
	tokencount++;
}

