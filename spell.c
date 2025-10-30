#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

//finding and opening all the specified files, including directory traversal

//reading the file and generating a sequence of position-annotated words

//checking whether a word is contained in the dictionary

// take in arguments from command line
int main(int argc, char *argv[]) { // spell [-s {suffix}] {dictionary} [{file or directory}]*, suffix is optional
    // dictionary has word list
    // one or more inputs

    if(argc < 2){
        fprintf(stderr, "Not enough arguments. Please specify a dictionary file and a file/directory to be read from.");
        exit(EXIT_FAILURE);
    }

    bool suffixExists = false 
    char *suffix = ".txt"; // default suffix
    int index = 1; 

    if(strcmp(argv[1], "-s") == 0){
        if(argc < 4){
            fprintf(stderr, "Not enough arguments. Please also specify a dictionary file and a file/directory to be read from.");
            exit(EXIT_FAILURE);
        }
        suffix = argv[2]; // if an argument begins with -s, then the next argument will specify the file name suffix to be used when scanning directories
        suffixExists = true;
        index = 3; // if there is a suffix argument, -s and suffix take up first two arguments, so dictionary starts at third
    }

    char *dictPath = argv[index];

    int dict_fd = open(dictPath, O_RDONLY);
    if (dict_fd < 0) {
        perror(dictPath);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0){
            printf("ERROR: -s can only be first argument"); // forbid -s from occurring later than first argument
            return 1;
        }
    }

}