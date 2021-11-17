#include <stdio.h>
int main() {
	int y = 5;
	int x;
	while (y > 0) {
		printf("%d",y);
		printf("\n");
		x = y;
		while (x > 0) {
			printf("Hello World\n");
			x = x - 1;
		}
		y = y - 1;
	}
	return 0;
}