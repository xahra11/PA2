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
void read_file(const char *filePath, char **dictionary, int dict_size);
void traverse_directory(const char *dirPath, const char *suffix, char **dictionary, int dict_size);
char **load_dictionary(const char *dictPath, int *dict_size);
bool check_dictionary(const char *word, char **dictionary, int dict_size);
void normalize(char *word);
bool is_number(const char *word);

int main(int argc, char *argv[]) { // spell [-s {suffix}] {dictionary} [{file or directory}]*, suffix is optional
    // dictionary has word list
    // one or more inputs

    if(argc < 2){
        fprintf(stderr, "Not enough arguments. Please specify a dictionary file and a file/directory to be read from.");
        exit(EXIT_FAILURE);
    }

    char *suffix = ".txt"; // default suffix
    int index = 1; 

    if(strcmp(argv[1], "-s") == 0){
        if(argc < 4){
            fprintf(stderr, "Not enough arguments. Please also specify a dictionary file and a file/directory to be read from.");
            exit(EXIT_FAILURE);
        }
        suffix = argv[2]; // if an argument begins with -s, then the next argument will specify the file name suffix to be used when scanning directories
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

char **load_dictionary(const char *dictPath, int *dict_size){
    int fd = open(dictPath, O_RDONLY);
    if(fd < 0){
        perror(dictPath);
        return NULL;
    } 

    int capacity = 50;
    char **dictionary = malloc(capacity * sizeof(char*));
    if (!dictionary) {
        perror("malloc");
        close(fd);
        return NULL;
    }

    char buf[BUFSIZE + 1];
    int bytes;

    int word_capacity = 16;
    int word_length = 0;
    char *word = malloc(word_capacity);
    if(!word){
        perror("malloc");
        close(fd);
        free(dictionary);
        return NULL;
    }

    while((bytes = read(fd, buf, BUFSIZE)) > 0){
        // tokenize and check each word in dictionary
        for(int i = 0; i < bytes; i++){
            char ch = buf[i];

            if(isspace(ch)){ // end of word
                if(word_length > 0){
                    word[word_length] = '\0';
                    normalize(word);

                    if(strlen(word) > 0){
                        if(*dict_size >= capacity){
                            capacity *= 2;
                            dictionary = realloc(dictionary, capacity * sizeof(char *));
                            if(!dictionary){
                                perror("realloc");
                                free(word);
                                close(fd);
                                return NULL;
                            }
                        }
                        dictionary[*dict_size] = strdup(word);
                        (*dict_size)++;
                    }
                    word_length = 0;
                }
            }else{
                if(word_length + 1 >= word_capacity){
                    word_capacity *= 2;
                    word = realloc(word, word_capacity);
                    if(!word){
                        perror("realloc");
                        close(fd);
                        for(int j = 0; j < *dict_size; j++) free(dictionary[j]);
                        free(dictionary);
                        return NULL;
                    }
                }
                word[word_length++] = ch;
            }
        }
    }

    if (word_length > 0) {
        word[word_length] = '\0';
        normalize(word);
        if (strlen(word) > 0) {
            if (*dict_size >= capacity) {
                capacity *= 2;
                dictionary = realloc(dictionary, capacity * sizeof(char *));
                if (!dictionary) {
                    perror("realloc");
                    free(word);
                    close(fd);
                    return NULL;
                }
            }
            dictionary[*dict_size] = strdup(word);
            (*dict_size)++;
        }
    }

    free(word);
    close(fd);
    return dictionary;
}

bool is_number(const char *word){ // check if word is only a number
    if(!word || word[0] == '\0'){
        return false;
    }
    
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
    while (start <= end && (word[start] == '(' || word[start] == '[' ||
                            word[start] == '{' || word[start] == '\'' ||
                            word[start] == '"')) {
        start++;
    }

    // move end index backward to ignore trailing punctuation
    while (end >= start && !isalnum((unsigned char)word[end]) && word[end] != '-') {
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
}

bool check_dictionary(const char *word, char **dictionary, int dict_size){
    for (int i = 0; i < dict_size; i++) {
        const char *dict_word = dictionary[i];

        bool lowercase = true;
        for (int j = 0; dict_word[j]; j++) {
            if (isupper((unsigned char)dict_word[j])) {
                lowercase = false;
                break;
            }
        }

        if (lowercase) {
            if (strcasecmp(word, dict_word) == 0) return true;
        } else {
            if (strcmp(word, dict_word) == 0) return true;
        }
    }
    return false;
}

void read_file(const char *filePath, char **dictionary, int *dict_size){
    int fd = open(filePath, O_RDONLY);
    if(fd < 0){
        perror(filePath);
        return;
    }

    char buf[BUFSIZE + 1];
    int bytes;
    int capacity = 16, word_length = 0;
    char *word = malloc(capacity);
    if(!word){
        perror("malloc");
        close(fd);
        return;
    }
    int line = 1, col = 1, startCol = 1;

    while((bytes = read(fd, buf, BUFSIZE)) > 0){
        // tokenize and check each word in dictionary
        for(int i = 0; i < bytes; i++){
            char ch = buf[i];

            if(isspace(ch)){ // end of word
                if(word_length > 0){
                    word[word_length] = '\0';
                    normalize(word);

                    // print misspelled words
                    if(strlen(word) > 0 && !is_number(word) && !check_dictionary(word, dictionary, dict_size)){
                        printf("%s:%d:%d %s\n", filePath, line, startCol, word);
                    }

                    word_length = 0;
                }
                if(ch == '\n'){ // new line
                    line++;
                    col = 1;
                }
            }else{
                if(word_length == 0){
                    startCol = col; // column number of new word
                }

                if(word_length + 1 >= capacity){ // resize word
                    capacity *= 2;
                    word = realloc(word, capacity);
                    if(!word){
                        perror("realloc");
                        close(fd);
                        return;
                    }
                }

                word[word_length++] = ch;
            }
            col++;
        }
    }

    if(word_length > 0){
        word[word_length] = '\0';
        normalize(word);
        if(strlen(word) > 0 && !is_number(word) && !check_dictionary(word, dictionary, dict_size)){
            printf("%s:%d:%d %s\n", filePath, line, startCol, word);
        }
    }

    free(word);
    close(fd);
}

void traverse_directory(const char *dirPath, const char *suffix, char **dictionary, int dict_size){
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

        // file path 
        int size = strlen(dirPath) + 1 + strlen(de->d_name) + 1;
        char *path = malloc(size);
        if(!path){
            perror("malloc");
            closedir(dp);
            return;
        }
        snprintf(path, size, "%s/%s", dirPath, de->d_name);

        struct stat st;
        if(stat(path, &st) == -1){
            perror(path);
            free(path);
            continue;
        }

        int length = strlen(de->d_name);
        int suffixLength = strlen(suffix);
        if(S_ISREG(st.st_mode) && len >= suff_len && strcmp(de->d_name + len - suffixLength, suffix) == 0){
            // matching suffix file
            read_file(path, dictionary, dict_size);
        }else if(S_ISDIR(st.st_mode)){ // recursively go through directory
            traverse_directory(path, suffix, dictionary, dict_size);
        }
        free(path);
    }
    closedir(dp);
}

