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

    // if an argument begins with -s, then the next argument will specify the file name suffix to be used when scanning directories
    for(int i = 0; i < (argc - 1); i++){
        if (strcmp(argv[i], "-s") == 0){
            char suffix[] = argv[i+1];
            break;
        }
    }
}