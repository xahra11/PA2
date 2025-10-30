#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//finding and opening all the specified files, including directory traversal

//reading the file and generating a sequence of position-annotated words

//checking whether a word is contained in the dictionary

// take in arguments from command line
int main(int argc, char *argv[]) { // spell [-s {suffix}] {dictionary} [{file or directory}]*, suffix is optional
    // dictionary has word list
    // one or more inputs

    if(argc < 2){
        printf("Not enough arguments. Please specify a dictionary file and a file/directory to be read from.");
        return 1;
    }

    if(strcmp(argv[0], "-s") == 0){
        char suffix[] = argv[1]; // if an argument begins with -s, then the next argument will specify the file name suffix to be used when scanning directories
    }else{
        char suffix[] = ".txt";
    }

    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0){
            printf("ERROR: -s can only be first argument"); // forbid -s from occurring later than first argument
            return 1;
        }
    }
}