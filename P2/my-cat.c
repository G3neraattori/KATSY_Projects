#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cat(char *filename);

int main(int argc, char *argv[]){
    if (argc < 2){ //wrong amount of args
        exit(EXIT_SUCCESS);
    }

    //multiple files
    for (int i = 1; i < argc; i++){
        cat(argv[i]);
    }
    return 0;
}

void cat(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) { //file open error
        printf("my-cat: cannot open file\n");
        exit(EXIT_FAILURE);
    }

    char row[4096]; //Posix doesn't recommend going over this.
    //this could be dynamic but the instructions don't mention it should be
    while (fgets(row, 4096, file) != NULL) {
        printf("%s", row);
    }

    fclose(file); //close the file
}
