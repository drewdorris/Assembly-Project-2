#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILE_LENGTH 2000 // maximum amnt of characters in the file

int main(int argc, char *argv[]) {
	char all[MAX_FILE_LENGTH];
	
	char ch;
	int i = 0;
	// https://stackoverflow.com/questions/15883568/reading-from-stdin
	// iterate each char in input
	while(read(STDIN_FILENO, &ch, 1) > 0)
	{
		// remove tab and newline chars
		if (ch == '\t' || ch == '\n') {
			continue;
		}
		all[i] = ch;
		i = i + 1;
	}
	printf("\nArray length: %i", i - 1);
	printf("\nCode: %s\n", all);
	
	// array is now set up, future expansion here
	
	
	
	return 0;
}