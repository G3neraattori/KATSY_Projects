#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void zip(char *filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "my-zip: file1 [file2 ...]\n"); //from task "all output to stdout" stderr would be better
        exit(EXIT_FAILURE);
    }

    //multiple files
    for (int i = 1; i < argc; i++){
        zip(argv[i]);
    }
    return 0;
}

void zip(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) { //reading the file failed
        fprintf(stdout, "my-zip: cannot open file\n"); //from task "all output to stdout" stderr would be better
        exit(EXIT_FAILURE);
    }

    char current, previous;
    int concurrentCount = 1; //keeps the count of connect characters
    int hadValues = 0;

    //https://stackoverflow.com/questions/12269125/reading-a-file-one-char-at-a-time read one at time
    //https://man7.org/linux/man-pages/man3/fgetc.3.html and the actual usage and return values
    while((current = fgetc(file)) != EOF) {
        if(hadValues == 0) {
            hadValues = 1; //needed for printing the writing the final result
            previous = current; //init
            continue;
        }

        if (current == previous) { //if same then just increase count
            concurrentCount++;
        }else{ //changed so write the amount
            //task said use fwrite()
            fwrite(&concurrentCount, 4, 1, stdout); //"a 4-byte integer"
            fwrite(&previous, sizeof(previous), 1, stdout);//then the character

            previous = current; //reset count and what char
            concurrentCount = 1;
        }
    }

    //write the final result if there were results
    if(hadValues == 1) {
        fwrite(&concurrentCount, 4, 1, stdout);
        fwrite(&previous, sizeof(previous), 1, stdout);
    }

    fclose(file); //close the fiel
}