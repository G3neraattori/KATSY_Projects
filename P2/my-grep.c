#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grep(char *filename, char *searchString);
void grep_input(char *searchString);

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "my-grep: searchterm [file ...]\n"); //the grep task doesn't say what print to use
        exit(EXIT_FAILURE);
    }

    if (argc == 2) { //if no input file print user input
        grep_input(argv[1]);
    } else {
        for (int i = 2; i < argc; i++){
            grep(argv[i], argv[1]);
        }
    }

    return 0;
}

void grep(char *filename, char *searchString) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) { //reading the file failed
        fprintf(stderr, "my-grep: cannot open file\n"); //the grep task doesn't say what print to use
        exit(EXIT_FAILURE);
    }

    char *buffer = NULL;
    size_t bufSize = 0;
    ssize_t row;

    while ((row = getline(&buffer, &bufSize, file)) != -1) { //read line by line
        //https://stackoverflow.com/questions/12784766/check-substring-exists-in-a-string-in-c
        if(strstr(buffer, searchString) != NULL){//if string contains the search term print
            fprintf(stdout, "%s", buffer); //the grep task doesn't say what print to use
        }
    }

    free(buffer);
    fclose(file);
}

void grep_input(char *searchString) {
    char *buffer = NULL;
    size_t bufSize = 0;
    ssize_t text;

    while ((text = getline(&buffer, &bufSize, stdin)) != -1) { //get input from stdin (user input)
        if(strstr(buffer, searchString) != NULL){ //if string contains the search term print
            fprintf(stdout, "%s", buffer);
        }
    }

    free(buffer);
}
