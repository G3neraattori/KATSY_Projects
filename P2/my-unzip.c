#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void unzip(char *filename);

int main(int argc, char *argv[]) {
    if (argc < 2) { //wrong amount of args
        printf("my-unzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    //multiple files
    for (int i = 1; i < argc; i++){
        unzip(argv[i]);
    }

    return 0;
}

void unzip(char *filename) {
    FILE *file;
    file = fopen(filename, "rb"); //rb because it is binary
    if (file == NULL) {
        printf("my-unzip: cannot open file\n");
        exit(EXIT_FAILURE);
    }

    int concurrentCount = 0;
    char character;

	//Since my-zip always saves in fomat [count][char] we can assume there is a number first and char second
    while(fread(&concurrentCount, 4, 1, file) == 1) { //ptr, size, elem count, file size was always 4. fread from task
        fread(&character, sizeof(char), 1, file); //after the amount is read (previous line) read the character
        for(int i = concurrentCount; i > 0; i--){ //then print amount times the character
            //task says use printf
            printf("%c", character);
        }
    }


    fclose(file);
}