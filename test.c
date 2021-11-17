
#include <stdio.h>
int main() {
	printf("Hello, world!\n");
	int x = 0;
	if (1) {
		if (x <= 1) {
			if (x != 0) {
				printf("Not zero!\n");
			} else {
				printf("Zero!\n");
			}
		} else {
			if (x > 5) {
				printf("Greater than 5!\n");
			}
		}
	}
	return 0;
}