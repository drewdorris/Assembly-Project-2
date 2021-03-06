/*
Program Name: PepC
Program Authors: Andrew "Drew" Dorris, Dante Hays, Charles Kinsler, Matthew Lockard, Liam Whitelaw
Class: CSCI 2160-001
Lab: Project 2
Date: 11/16/2021
Purpose: Convert a small subset of C language source code into Pep/9 assembly source
*/

#ifndef TOKEN_H
#define TOKEN_H
struct token {
	int type; // The type of the token out of a set of all possible types
	void * payload; // The payload of that token, if one need exist, such as the string of the identifier or a pointer to int for a constant number. Null otherwise
};

	/*
	* Dynamically sizing array
	* https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
	*/

//typedef struct {
//	int *array;
//	size_t used;
//	size_t size;
//} Array;


// Token types. Not all of these are going to be used, but are reserved in case they are. If in any doubt, try implementing it- worst that happens is we can
// throw it out at the parsing stage and signal an error that we don't know how to handle this. -LW

// Also the number ordering will be off, but that doesn't matter

#define TYPE_WHITESPACE 0 // Whitespace. Can be discarded when encountered.
// Valued tokens
#define TYPE_NUMBER 1 // Constant number. Payload: int
#define TYPE_IDENTIFIER 2 // Identifier, such as a variable. Payload: char * (string)
#define TYPE_STRING 3 // String in "quotes". Payload: char * (string)
// Punctuations
#define TYPE_LEFT_PAREN 20 // (
#define TYPE_RIGHT_PAREN 21 // )
#define TYPE_LEFT_BRACE 22 // {
#define TYPE_RIGHT_BRACE 23 // }
#define TYPE_COMMA 24 // ,
#define TYPE_SEMI 25 // ;
#define TYPE_QUOTE 26 // "
// Operators
#define TYPE_ADD 40 // +
#define TYPE_SUB 41 // -
#define TYPE_AND 42 // &
#define TYPE_OR 43 // |
#define TYPE_NEG 44 // ~
#define TYPE_ASSIGN 45 // =
#define TYPE_MULT 46 // *
#define TYPE_DIV 47 // /
#define TYPE_MOD 48 // %
#define TYPE_LEFT_SHIFT 49 // <<
#define TYPE_RIGHT_SHIFT 50 // >>
// Conditions
#define TYPE_CD_EQUAL 60 // ==. I haven't decided whether this should be a production of two tokens, or directly handled as one.
#define TYPE_CD_LESS 61 // <
#define TYPE_CD_GREATER 62 // >
#define TYPE_CD_NOT_EQUAL 63 // !=
#define TYPE_CD_LESS_OR_EQUAL 64 // <=
#define TYPE_CD_GREATER_OR_EQUAL 65 // >=
// Keywords and identifiers that we can cheat as keywords
#define TYPE_KW_INT 80 // int keyword
#define TYPE_KW_SHORT 81 // short keyword
#define TYPE_KW_VOID 82 // void keyword
#define TYPE_KW_RETURN 83 // return keyword
#define TYPE_KW_IF 84 // if keyword
#define TYPE_KW_ELSE 85 // else keyword ("else if" is two tokens)
#define TYPE_KW_MAIN 86 // "main" pseudokeyword. It's technically an identifier, but for our purposes it can be one.
#define TYPE_KW_SCANF 87 // "scanf" pseudokeyword. Inputs an integer
#define TYPE_KW_PRINTF 89 // "printf" pseudokeyword. Outputs either a string or an integer
#define TYPE_KW_WHILE 90 // while loops

// initialise token state with default values
void tokenInit(struct token * self);
// get how many tokens are in the tokeniser
int getTokenCount();
// tokenize the input string with specified length
void tokenize(char *argv, int size);
// ...?
void get_token_array(int which,struct token **buffer);
// return human-readable typename for token type identifier
char * tokenTypeString(int typeId);

// return true if a given string is ahead
int tryForString(char * buffer, int position, int size, char * target);
// return index of given char; -1 if not found
int findIndexOfNextChar(char * buffer, int position, int size, char target);
// return index of next char after position that is not a-zA-Z0-9_; -1 if not found
int findIndexOfNextNonAlphanumericChar(char * buffer, int position, int size);
// return index of next char after position that is not 0-9; -1 if not found
int findIndexOfNextNonNumericChar(char * buffer, int position, int size);

// push token to array
void pushToken(struct token tok);
// push token type (no payload)
void pushTokenType(int type);

#endif

