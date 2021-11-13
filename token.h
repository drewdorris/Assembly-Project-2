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
//    int *array;
//    size_t used;
//    size_t size;
//} Array;


//Token types. Not all of these are going to be used, but are reserved in case they are. If in any doubt, try implementing it- worst that happens is we can
//throw it out at the parsing stage and signal an error that we don't know how to handle this. -LW

//Also the number ordering will be off, but that doesn't matter

#define TYPE_WHITESPACE 0 // Whitespace. Can be discarded when encountered.
//Valued tokens
#define TYPE_NUMBER 1 // Constant number. Payload: int
#define TYPE_IDENTIFIER 2 // Identifier, such as a variable. Payload: char * (string)
#define TYPE_STRING 3 // String in "quotes". Payload: char * (string)
//Punctuations
#define TYPE_LEFT_PAREN 4 // (
#define TYPE_RIGHT_PAREN 5 // )
#define TYPE_LEFT_BRACE 6 // {
#define TYPE_RIGHT_BRACE 7 // }
#define TYPE_COMMA 8 // ,
#define TYPE_SEMI 9 // 
#define TYPE_QUOTE 10 // "
//Operators
#define TYPE_ADD 11 // +
#define TYPE_SUB 12 // -
#define TYPE_AND 13 // &
#define TYPE_OR 14 // |
#define TYPE_NEG 15 // ~
#define TYPE_ASSIGN 16 //
#define TYPE_EQUAL 17 //=. I haven't decided whether this should be a production of two tokens, or directly handled as one.
//Keywords and identifiers that we can cheat as keywords
#define TYPE_KW_INT 18 // int keyword
#define TYPE_KW_SHORT 19 // short keyword
#define TYPE_KW_VOID 20 // void keyword
#define TYPE_KW_RETURN 21 // return keyword
#define TYPE_KW_IF 22 // if keyword
#define TYPE_KW_ELSE 23 // else keyword ("else if" is two tokens)
#define TYPE_KW_MAIN 24 //"main" pseudokeyword. It's technically an identifier, but for our purposes it can be one.
#define TYPE_KW_SCANF 25 //"scanf" pseudokeyword. Inputs an integer
#define TYPE_KW_PRINTF 26 //"printf" pseudokeyword. Outputs either a string or an integer
#define TYPE_KW_WHILE 27 // while loops

void tokenInit(struct token * self);

#endif
