#include <stdio.h>

int gVar1;
int gVar2 = 42;

int main() {
	printf("Hello World!\n");

	int x;
	int y = 42;

	scanf("%d",&x);
	printf("x = ");
	printf("%d",x);
	printf("\n");

	printf("y = ");
	printf("%d",y);
	printf("\n");

	x = x + 1;

	printf("x = x + 1 = ");
	printf("%d",x);
	printf("\n");

	x = y + x;

	printf("x = x + y = ");
	printf("%d",x);
	printf("\n");

	x = x * y;

	printf("x = x * y = ");	
	printf("%d",x);
	printf("\n");

	x = x / y;

	printf("x = x / y = ");	
	printf("%d",x);
	printf("\n");

	x = x % 39;

	printf("x = x % 39 = ");	
	printf("%d",x);
	printf("\n");

	x = x << 4;

	printf("x = x << 3 = ");	
	printf("%d",x);
	printf("\n");

	x = x >> 2;

	printf("x = x >> 2 = ");	
	printf("%d",x);
	printf("\n");

	x = ~x;
	printf("x = ~x = ");
	printf("%d",x);
	printf("\n");

	x = x >> 1;

	printf("x = x >> 1 = ");	
	printf("%d",x);
	printf("\n");

	short a = 1;
	short b = 2;
	short c = a | b;
	printf("a | b = ");
	printf("%d",c);
	printf("\n");

	gVar1 = 41;
	gVar1 = gVar2 - gVar1;
	gVar1 = ~gVar1;
	printf("%d",gVar1);

	printf("\nThats all folks\n");
	return 0;
}