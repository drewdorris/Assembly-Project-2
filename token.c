#include "token.h"
#include <stdlib.h>

void tokenInit(struct token * self) {
	self->type = TYPE_WHITESPACE;
	self->payload = NULL;
}