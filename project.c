#include <stdio.h>
#include <string.h>

#define MAX_FILE_LENGTH 2000 // maximum amnt of characters in the file
#define MAX_LINE_LENGTH 50 // maximum amnt of characters per line

int main(int argc, char *argv[]) {
	FILE *file = fopen(argv[1], "r"); // "r" for read
	if (file == NULL) {
		printf("Unknown file!\n");
		return 0;
	}
	
	char all[MAX_FILE_LENGTH]; // array will contain all text from file
	char buff[MAX_LINE_LENGTH]; // temp stores each line of the file
	
	int allIndex = 0;
	// https://stackoverflow.com/questions/16869467/command-line-arguments-reading-a-file
	// above link for retrieving data from file
	// while loop loops through the char[] received from each line and adds it to the end of all[]
	// fgets ALSO already divides very long lines into separate pieces, so we don't have to worry about very long lines of code
	while (fgets(buff, MAX_LINE_LENGTH - 1, file) != NULL) {
		int buffIndex = 0;
		// individually add characters from buff[] to all[]
		// \0 is null terminator. indicates end of line
		while (buff[buffIndex] != '\0') {
			// remove tabs
			if (buff[buffIndex] == '\t') {
				buffIndex = buffIndex + 1;
				continue;
			}
			all[allIndex] = buff[buffIndex];
			allIndex = allIndex + 1;
			buffIndex = buffIndex + 1;
			// check whether outside of all[] bounds
			// if so, we return, because we can't store any more of the program
			if (allIndex >= MAX_FILE_LENGTH) {
				printf("File size is too large!\nMaximum is %i characters\n", MAX_FILE_LENGTH);
				return 0;
			}
		}
		allIndex = allIndex - 1; // erase null terminator at end of each line
	}
	fclose(file);  // close the file after using it
	
	int count = -1;
	// count characters in all[] (loop until found null terminator)
	while (all[count + 1] != '\0') {
		count = count + 1;
	}
	// print array length, and the arrays contents as a string
	printf("\nArray length: %i\n", count);
	printf("%s", all);
	
	// array is now set up, future expansion here
	
	
	
	return 0;
}