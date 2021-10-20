#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
	FILE *file = fopen(argv[1], "r"); // "r" for read
	if (file == NULL) {
		printf("Unknown file!\n");
		return 0;
	}
	
	char all[2000]; // array containing all text from file
	char buff[50]; // temp 
	int i = 0;
	// https://stackoverflow.com/questions/16869467/command-line-arguments-reading-a-file
	while(fgets(buff, 50 - 1, file) != NULL) 
    {
		int n = 0;
		while (buff[n] != NULL) {
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
	while (all[i] != NULL) {
		printf("%c", all[i]);
		i = i + 1;
	}
	printf("Array length: %i", i);
	
	return 0;
}