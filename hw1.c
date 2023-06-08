#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int hash(const char* word, int cacheSize) {
    int sum = 0;
    const char* ptr = word;

    while (*ptr != '\0') {
        sum += (int)*ptr;
        ptr++;
    }

    int index = sum % cacheSize;
    return index;
}

void printCache(const char** cache, int cacheSize) {
    printf("Cache:\n");
    const char** cachePtr = cache;
    int i = 0;
    while (i < cacheSize) {
        if (*cachePtr != NULL) {
            printf("index %d ==> \"%s\"\n", i, *cachePtr);
        }
        cachePtr++;
        i++;
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s cacheSize filename\n", *argv);
        return 1;
    }
    int cacheSize = atoi(*(argv + 1));
    if (cacheSize <= 0) {
        fprintf(stderr, "ERROR: Invalid cache size\n");
        return 1;
    }
    FILE* file = fopen(*(argv + 2), "r");
    if (file == NULL) {
        perror("ERROR");
        return 1;
    }
    char** cache = (char**)calloc(cacheSize, sizeof(char*));
    if (cache == NULL) {
        perror("ERROR");
        fclose(file);
        return 1;
    }
    char *word = calloc(1000, sizeof(char));
    while (fscanf(file, "%s", word) == 1) {
        int len = strlen(word);
        if (len >= 2 && isalpha(*word) && isalpha(*(word + 1))) {
            int index = hash(word, cacheSize);
            if (*(cache + index) == NULL) {
                *(cache + index) = (char*)calloc(len + 1, sizeof(char));
                if (*(cache + index) == NULL) {
                    perror("ERROR");
                    fclose(file);
                    for (int i = 0; i < cacheSize; i++) {
                        free(*(cache + i));
                    }
                    free(cache);
                    return 1;
                }
                strcpy(*(cache + index), word);
                printf("Word \"%s\" ==> %d %ccalloc%c\n", word, index, 91, 93);
            } else {
                char* temp = (char*)realloc(*(cache + index), (len + 1) * sizeof(char));
                if (temp == NULL) {
                    perror("ERROR");
                    fclose(file);
                    for (int i = 0; i < cacheSize; i++) {
                        free(*(cache + i));
                    }
                    free(cache);
                    return 1;
                }
                *(cache + index) = temp;
                strcpy(*(cache + index), word);
                printf("Word \"%s\" ==> %d %crealloc%c\n", word, index, 91, 93);
            }
        }
    }
    fclose(file);
    printCache((const char**)cache, cacheSize);
    for (int i = 0; i < cacheSize; i++) {
        free(*(cache + i));
    }
    free(cache);
    return 0;
}