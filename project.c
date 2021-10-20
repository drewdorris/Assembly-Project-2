#include <stdio.h>
#include <string.h>

#define MAX_FULL_LENGTH 2000
#define MAX_LINE_LENGTH 50

int main(int argc, char *argv[]) {
	FILE *file = fopen(argv[1], "r"); // "r" for read
	if (file == NULL) {
		printf("Unknown file!\n");
		return 0;
	}
	
	char all[MAX_FULL_LENGTH]; // array containing all text from file
	char buff[MAX_LINE_LENGTH]; // temp 
	int i = 0;
	// https://stackoverflow.com/questions/16869467/command-line-arguments-reading-a-file
	while(fgets(buff, MAX_LINE_LENGTH - 1, file) != NULL) 
    {
		int n = 0;
		while (buff[n] != '\0') {
			if (buff[n] == '\t') {
				n = n + 1;
				continue;
			}
			all[i] = buff[n];
			i = i + 1;
			n = n + 1;
		}
        /* buff has one line of the file, do with it what you will... */

        /*printf ("%s\n", buff);  ...such as show it on the screen */
		i = i - 1; // remove null terminator at end
    }
	fclose(file);  /* close the file */ 
	
	i = 0;
	while (all[i] != '\0') {
		printf("%c", all[i]);
		i = i + 1;
	}
	printf("\nArray length: %i\n", i);
	
	return 0;
}