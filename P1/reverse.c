#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Linked list so the results are reversed "automatically"
typedef struct LINKEDLIST {
    char *text;
    struct LINKEDLIST *next;
} LINKEDLIST;

LINKEDLIST *readFile(char *input);
void writeFile(LINKEDLIST *head, char *output);
void printFile(LINKEDLIST *head);
void freeList(LINKEDLIST *head);

int main(int argc, char *argv[]) {
    //If wrong amount of arguments provided
    if (argc > 3 || argc < 2) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(EXIT_FAILURE);
    }

    //Filename from args
    char *input_str = argv[1];

    if (argc == 2) { //If filename one then read the file and print it
        LINKEDLIST *input_contents = readFile(input_str); //get the contents to linked list
        printFile(input_contents); //print output
        freeList(input_contents); //free the linked list
    }else if(argc == 3){ //if input and output safe to file
        char *output_str = argv[2];

        if(strcmp(input_str, output_str) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(EXIT_FAILURE);
        }

        LINKEDLIST *input_contents = readFile(input_str); //get the contents to linked list
        writeFile(input_contents, output_str); //save output
        freeList(input_contents); //free the linked list
    }

    return 0;
}

//Printing the linked list with basic while loop
void printFile(LINKEDLIST *head) {
    LINKEDLIST *node = head;
    int first = 1;
    while (node != NULL) {

        if(first) {
            fprintf(stdout, "%s\n", node->text); //Because the last line (=first) ends in \0 instead of linebreak
            first = 0;
        } else {
            fprintf(stdout, "%s", node->text);
        }
        node = node->next;
    }

}

//read the inputfile
LINKEDLIST *readFile(char *input) {
    FILE *file;
    file = fopen(input, "r");

    if (file == NULL) { //reading the file failed
        fprintf(stderr, "error: cannot open file '%s'\n", input);
        exit(EXIT_FAILURE);
    }

    char *buffer = NULL;
    size_t bufsize = 0;
    ssize_t row;
    LINKEDLIST *head = NULL;

    //Linked list causes reversed results when used like this
    while ((row = getline(&buffer, &bufsize, file)) != -1) { //read the file
        LINKEDLIST *node;
        if ((node = malloc(sizeof(struct LINKEDLIST))) == NULL) { //malloc memory for each node of the linked list
            fprintf(stderr, "malloc failed\n");
        }

        //https://man7.org/linux/man-pages/man3/getline.3.html
        //We get the needed size for malloc if we return save the getline returned value = numbers of chars read
        node->text = malloc(row + 1);
        if (node->text == NULL) {
            fprintf(stderr, "malloc failed\n");
        }

        strcpy(node->text, buffer); //copy the line
        node->next = head; //change next to head
        head = node; // change head to the new node
    }

    free(buffer); //free buffer
    fclose(file); //close the file that was read

    return head; //returns the linked list with file contents
}

//save to file
void writeFile(LINKEDLIST *head, char *output) {
    FILE *file;
    file = fopen(output, "w");
    if (file == NULL) { //if error opening(/creating) the file for saving
        fprintf(stderr, "error: cannot open file '%s'\n", output);
        exit(EXIT_FAILURE);
    }

    LINKEDLIST *node = head;
    int first = 1;
    //same as in print. The code could be be combined and optimized
    while (node != NULL) {
        if(first) {
            fprintf(file, "%s\n", node->text); //Because the last line (=first) ends in \0 instead of linebreak
            first = 0;
        } else {
            fprintf(file, "%s", node->text);
        }
        node = node->next;
    }


}

//Frees the list. Loops and frees so everything will get freed.
void freeList(LINKEDLIST *head){
    LINKEDLIST *node = head;
    while ((node = head) != NULL)
    {
        head = head->next;
        free(node->text);
        free(node);
    }
}