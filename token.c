#include "token.h"
#include <stdlib.h>
#include <string.h>

struct token tokens[1000];
int tokencount = 0;

void tokenInit(struct token * self) {
    self->type = TYPE_WHITESPACE;
    self->payload = NULL;
}

void tokenize(char *argv) {

    for (int i = 0; i < sizeof(argv); i++) {

        switch (argv[i]) {

            case ' ':
	    {
                break;
	    }

            case 'i':
            {
                int isInt = 0;
                char intStr[3];

                for (int j = 0; j < 3; j++) {
                    strcat(intStr, (char *) &argv[i]);
                }

                while (strcmp("int", intStr) != 0) {
                    isInt = 1;
                }

                if (isInt == 1){
                    struct token token;
                    token.type = TYPE_KW_INT;
                }

                break;
            }

            case 'm':
	    {
		//TODO finish method for concat and comparing keywords, see case 'i'
                break;
	    }

            case ',':
            {
                struct token token;
                token.type = TYPE_COMMA;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case ';':
            {
                struct token token;
                token.type = TYPE_SEMI;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '+':
            {
                struct token token;
                token.type = TYPE_ADD;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '-':
            {
                struct token token;
                token.type = TYPE_SUB;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '&':
            {
                struct token token;
                token.type = TYPE_AND;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '|':
            {
                struct token token;
                token.type = TYPE_OR;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '(':
            {
                struct token token;
                token.type = TYPE_LEFT_PAREN;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case ')':
            {
                struct token token;
                token.type = TYPE_RIGHT_PAREN;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '{':
            {
                struct token token;
                token.type = TYPE_LEFT_BRACE;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case '}':
            {
                struct token token;
                token.type = TYPE_RIGHT_BRACE;
		tokens[tokencount] = token;
		tokencount++;
                break;
            }

            case 'p':
            {
		//TODO finish implementation of keyword comparitor
                break;
            }

            case '"':
            {
		//TODO concat from first double quote till second and store as string
                break;
            }

        }
    }
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
