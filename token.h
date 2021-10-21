#ifndef TOKEN_H
#define TOKEN_H
struct token {
	int type; //The type of the token out of a set of all possible types
	void * payload; //The payload of that token, if one need exist, such as the string of the identifier or a pointer to int for a constant number. Null otherwise
};

#endif TOKEN_H