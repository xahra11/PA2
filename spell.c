#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

#define BUFSIZE 128

// reading the file and generating a sequence of position-annotated words

// checking whether a word is contained in the dictionary

// finding and opening all the specified files, including directory traversal

// function  prototypes
void read_file(const char *filePath, int dict_fd);
void traverse_directory(const char *dirPath, const char *suffix, int dict_fd);

int main(int argc, char *argv[]) { // spell [-s {suffix}] {dictionary} [{file or directory}]*, suffix is optional
    // dictionary has word list
    // one or more inputs

    if(argc < 2){
        fprintf(stderr, "Not enough arguments. Please specify a dictionary file and a file/directory to be read from.");
        exit(EXIT_FAILURE);
    }

    bool suffixExists = false;
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
    int dict_size = 0;
    char **dictionary = load_dictionary(dictPath, &dict_size);

    if (!dictionary) {
        fprintf(stderr, "%s\n dictionary did not load.\n", dictPath);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0){
            fprintf(stderr, "ERROR: -s can only be the first argument.\n"); // forbid -s from occurring later than first argument
            return 1;
        }
    }

    for(int i = index + 1; i < argc; i++){
        char *filePath = argv[i];
        struct stat statinfo;

        if(stat(filePath, &statinfo) == -1){
            perror(filePath);
            continue;
        }

        if(S_ISREG(statinfo.st_mode)){ // is regular file, open file
            read_file(filePath, dictionary, dict_size);
            
        }else if(S_ISDIR(statinfo.st_mode)){ // is directory, traverse through it
            traverse_directory(filePath, suffix, dictionary, dict_size);
            
        }
    } 

    for (int i = 0; i < dict_size; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
    
    return 0;
}

bool is_number(const char *word){ // check if word is only a number
    for(int i = 0; word[i]; i++){
        if(!isdigit((unsigned char)word[i])){
            return false;
        }
    }
    return true;
}

void normalize(char *word){
    if(!word || word[0] == '\0'){
        return;
    }

    int start = 0;
    int end = strlen(word) - 1;

    // move start index forward to ignore beginning punctuation
    while(start <= end && ispunct((unsigned char)word[start]) && word[start] != '-'){
        start++;
    }

    // move end index backward to ignore trailing punctuation
    while(end >= start && ispunct((unsigned char)word[end]) && word[end] != '-'){
        end--;
    }

    int length = end - start + 1; // new word length 
    if(length < 0){
        length = 0;
    }

    // new string without punctuation
    if(start > 0 && length > 0){
        memmove(word, word + start, length); 
    }
    word[length] = '\0';

    // lowercase word
    for(int i = 0; word[i]; i++){
        word[i] = tolower(word[i]);
    }
}

bool check_dictionary(const char *word, int dict_fd){
    return false;
}

void read_file(const char *filePath, int dict_fd){
    int file_fd = open(filePath, O_RDONLY);
    if(file_fd < 0){
        perror(filePath);
        return;
    }

    char buf[BUFSIZE + 1];
    int bytes;
    char *word = NULL;
    int capacity = 0;
    int word_length = 0;
    int line = 1;
    int col = 1;
    int startCol = 1;

    while((bytes = read(file_fd, buf, BUFSIZE)) > 0){
        // tokenize and check each word in dictionary
        for(int i = 0; i < bytes; i++){
            char ch = buf[i];
            col++;

            if(isspace(ch)){ // end of word
                if(word_length > 0){
                    word[word_length] = '\0';
                    normalize(word);

                    if(strlen(word) > 0 && !is_number(word)){ 
                        // print misspelled words
                        if(!check_dictionary(word, dict_fd)){
                            printf("%s:%d:%d %s", filePath, line, startCol, word);
                        }
                    }
                    word_length = 0;
                }
                if(ch == '\n'){ // new line
                    line++;
                    col = 1;
                }
            }else{
                if(word_length == 0){
                    startCol = col;
                }

                if(word == NULL){
                    capacity = 16;
                    word = malloc(capacity);
                    if(!word){
                        perror("malloc");
                        close(file_fd);
                        return;
                    }
                }

                if(word_length + 1 >= capacity){
                    capacity *= 2;
                    char *temp = realloc(word, capacity);
                    if(!tmp){
                        perror("realloc");
                        free(word);
                        close(file_fd);
                        return;
                    }
                }

                word[word_length++] = ch;
            }
        }
    }
    free(word);
    close(file_fd);
}

void traverse_directory(const char *dirPath, const char *suffix, int dict_fd){
    DIR *dp = opendir(dirPath);
    if(dp == NULL){
        perror(dirPath);
        return;
    }

    struct dirent *de;
    while((de = readdir(dp)) != NULL){
        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirPath, de->d_name);

        struct stat subStat;
        if(stat(path, &subStat) == -1){
            perror(path);
            continue;
        }

        if(S_ISREG(subStat.st_mode)){
            if(strstr(de->d_name, suffix) != NULL){
                // matching suffix file
                read_file(path, dict_fd);
            }
        }else if(S_ISDIR(subStat.st_mode)){
            traverse_directory(path, suffix, dict_fd);
        }
    }
    closedir(dp);
}

