#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILE_LENGTH 2000 // maximum amnt of characters in the file

int main(int argc, char *argv[]) {
	// stores all input
	char all[MAX_FILE_LENGTH];
	
	// bool: tracks whether the previous char was a newline
	short afterNewLine = 0;
	// bool: tracks whether we are ignoring the current line we are on
	short ignoreLine = 0;
	// stores current char
	char ch;
	// used as index to place chars in appropriate place in all[]
	int current = 0;
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
		// previous character no longer a newline
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
		// place current char in the complete char[] of all input
		all[current] = ch;
		// then iterate
		current = current + 1;
	}
	printf("\nTotal characters: %i", current - 1);
	printf("\nCode: %s\n", all);
	
	// array is now set up, future expansion here
	
	
	
	return 0;
}