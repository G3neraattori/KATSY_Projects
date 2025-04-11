#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//Assingment standard error message
#define ERROR_MSG "An error has occurred\n"

void handleInput(char *input);
char **parseCommandArgs(char *input, int *w_argc);
void executeSystem(char **args);
char *parsePath(char *arg);
void runBatch(char **argv);
void runInteractive();

//Globals should be ok for this use according to POSIX
char *paths[100];
int pathCount = 0;

int main(int argc, char *argv[]) {   
    if (argc > 2) {
        fprintf(stderr, "Usage: wish [file]\n"); //multiple error types
        exit(EXIT_FAILURE);
    }

    //the default start path
    paths[pathCount] = "/bin";
    pathCount++;

    if(argc == 2) { //if a batch file was provided
        runBatch(argv);
    }

    if(argc == 1) { //run the interactive mode
        runInteractive();
    }

    return 0;
}

void handleInput(char *input) {
    int w_argc = 0;
    char* mainSaveprt, *chainedInput;
    chainedInput = strtok_r(input, "&", &mainSaveprt); 
    int first = 1;

    while(chainedInput != NULL) {
        if (first == 1) { //this is because continues will otherwise couse problems. Definitely not the most elegant solution
            first = 0;
        } else {
            chainedInput = strtok_r(NULL, "&", &mainSaveprt); //next input       
        }

        if(chainedInput == NULL) {
            break;  //there was no more inputs   
        }        
    
        char **w_args = parseCommandArgs(chainedInput, &w_argc);
        if (w_args == NULL || w_args[0] == NULL || w_argc == 0) {
            //no args was returned
            free(w_args);
            continue;
        }    

        if(strcmp(w_args[0], "cd") == 0) {
            if (w_argc > 2) {
                fprintf(stderr, "Usage: cd [path]\n"); //multiple error types
                continue;
            }
            
            if (w_argc < 2) {
                fprintf(stderr, "Usage: cd [path]\n");//multiple error types
                continue;
            }

            if (chdir(w_args[1]) == -1) {
                fprintf(stderr, "Change Dir failed\n");//multiple error types
                continue;
            }
        } else if(strcmp(w_args[0], "exit") == 0) {
            //try to free memory
            free(w_args);
            free(chainedInput);
            exit(0);
        } else if(strcmp(w_args[0], "pwpaths") == 0) { //Extra command mostly for debugging. Shows paths similar to $PATH in linux           
            printf("Wish PATH(s): \n");
            for(int i = 0; i < pathCount; i++) {
                printf("%s\n", paths[i]);
            }  

        } else if(strcmp(w_args[0], "path") == 0) {
            pathCount = 0; //"if user sets to empty should not be able to run anything"            
            paths[pathCount] = "\0"; //https://stackoverflow.com/questions/2084094/reset-a-char-arraysize-contents empty char*[]
            
            for (int i = 1; i < w_argc; i++) {
                paths[pathCount] = w_args[i]; //https://stackoverflow.com/questions/20035475/coding-in-c-why-are-my-array-values-changing
                paths[pathCount] = strdup(w_args[i]);              
                pathCount++;         
            }  
        
        } else { // the "non built ins"
            char *path = parsePath(w_args[0]);    //get the path that excev will use
            if (path == NULL){
                fprintf(stderr, "Command not found\n");  //multiple error types
                //path was not found
                free(path);
                continue;
            }
            pid_t child = fork(); //fork so the program doesn't just stop after the command

            if (child == 0){
                int err = execv(path, w_args); //run the command with args

                if (err) {
                    //there was problem
                    fprintf(stderr, "Error running command\n"); //multiple error types
                }
                exit(0);
            } else if(child < 0) { //Child process could not be created
                fprintf(stderr, "Child process could not be created\n"); //multiple error types
            } else { //main
                waitpid(child, NULL, 0);
            }
    
            //free(path);
        }
        //free(w_args); 
    }
    
    while(wait(NULL) > 0); //wait any child process

}

char **parseCommandArgs(char *input, int *w_argc) {
    const char* delimiters = " \n\t\r\b\v"; //https://en.wikipedia.org/wiki/Escape_sequences_in_C
    char **w_args = malloc(1000 * sizeof(char *)); //wish_args
    char *saveprt; //https://linux.die.net/man/3/strtok_r works better
    char *arg;    
    int argc = 0;    
    arg = strtok_r(input, delimiters, &saveprt);
    if (arg == NULL){
        return NULL;    //there was no arg return null
    }
    w_args[argc] = arg; //the first arg (=command)
    argc++;

    while ((arg = strtok_r(NULL, delimiters, &saveprt)) != NULL) { //loop thorugh and add them to w_args if found
        w_args[argc] = arg;
        argc++;
    }

    *w_argc = argc; //set wish_argc using pointer

    return w_args; //return args
}

char* parsePath(char *arg){ 
    char *path = NULL; 
    for(int i = 0; i < pathCount; i++) {
        path = realloc(path, strlen(paths[i]) + strlen(arg) + 2); //https://stackoverflow.com/questions/14259540/c-programming-strcat-using-pointer + one because of the additional /
        //also changed to realloc because of loop        
        strcat(path, paths[i]);
        strcat(path, "/");
        strcat(path, arg);

        if(access(path, X_OK) == 0) { //from task
            return path;
        }
    }

    return NULL; //no path was found (this might cause errors)
}

//batch mode. Same as interactive but with file
void runBatch(char **argv) {
    FILE *file = fopen(argv[1], "r");
    char *buffer = NULL;
    size_t s = 0;

    if (file == NULL) {
        fprintf(stderr, "wish: cannot open file '%s'\n", argv[1]); //multiple error types
        exit(EXIT_FAILURE);
    }

    while (getline(&buffer, &s, file) != -1) {
        handleInput(buffer); //do what the input line says
    }
    
    free(buffer); //free memory
    fclose(file); // close file
    exit(0);
}

//interactive mode. Same as batch without opening the file and prints the wish>
void runInteractive() {
    FILE *input = stdin;   //Learned from previous tasks FILE in C is more like a stream than a FILE so we can use
    char *buffer = NULL;       //it for files AND input
    size_t s = 0;
    
    while (1) {
        printf("wish> ");

        if (getline(&buffer, &s, input) == -1) {
            free(buffer);
            exit(0);
        }
        handleInput(buffer); //do what the input line says
    }
}


















