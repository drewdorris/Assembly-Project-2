#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILE_LENGTH 2000 // maximum amnt of characters in the file

int main(int argc, char *argv[]) {
	char all[MAX_FILE_LENGTH];
	
	short afterNewLine = 0;
	short ignoreLine = 0;
	char ch;
	int i = 0;
	// https://stackoverflow.com/questions/15883568/reading-from-stdin
	// iterate each char in input
	while(read(STDIN_FILENO, &ch, 1) > 0)
	{
		// if we're ignoring this line and not on a new line, continue ignoring it
		if (ignoreLine == 1 && ch != '\n') {
			continue;
		// stop ignoring once we hit a new line
		} else if (ignoreLine == 1 && ch == '\n') {
			ignoreLine = 0;
			afterNewLine = 1;
			continue;
		}
		// if we hit a C directive, we're ignoring this next line
		if (ch == '#' && afterNewLine == 1) {
			ignoreLine = 1;
			afterNewLine = 0;
			continue;
		}
		afterNewLine = 0;
		// remove tab and newline chars
		if (ch == '\t') {
			continue;
		}
		if (ch == '\n') {
			// if a newline, keep track of that for when checking the next char
			afterNewLine = 1;
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