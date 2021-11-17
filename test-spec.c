#include <stdio.h>

int gVar1;
int gVar2 = 42;

int main() {
	printf("Hello World!\n");

	int x;
	int y = 42;

	scanf("%d",&x);
	printf("%d",x);

	x = y + x;
	x = x + 1;

	printf("%d",x);

	short a = 1;
	short b = 2;
	short c = a | b;
	printf("%d",c);

	x = x * 2;

	printf("%d",x);

	gVar1 = 41;
	gVar1 = gVar2 - gVar1;
	gVar1 = ~gVar1;
	printf("%d",gVar1);

	printf("Thats all folks\n");
	return 0;
}