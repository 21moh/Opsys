#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int hash(const char* word, int cacheSize) {
    int sum = 0;
    const char* ptr = word;
    while (*ptr != '\0') {
        sum += (int)(*ptr);
        ptr++;
    }
    int index = sum % cacheSize;
    return index;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s n filename\n", *argv);
        return 1;
    }
    int n = atoi(*(++argv));        // integer input
    if (n <= 0) {
        fprintf(stderr, "Error: Invalid Cache Size.\n");
        return 1;
    }
    char *filename = *(++argv);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening the file.\n");
        return 1;
    }
    char **hash = calloc(n, sizeof(char*));
    if (hash == NULL) {
        perror("Memory allocation failed.\n");
        return 1;
    }
    char *ptr;
    char *word = calloc(1000, sizeof(char));
    while (fscanf(file, "%s", word) == 1) {
        ptr = word;
        while (!isalpha(*ptr)) {
            ptr++;
        }
        if (*ptr == '"') {
            ptr++;
        }
        printf("%s", *ptr);
        while (*ptr != '\0') {
            if (!isalpha(*ptr)) {
                *ptr = '\0';
                break;
            }
            ptr++;
        }


        int length = strlen(word);
        if (length > 1) {
            int ASCII;
            int index;
            ASCII = 0;
            for (int j = 0; j < length; j++) {
                ASCII = ASCII + *(word + j);
            }
            index = ASCII % n;
            if (*(hash + index) == NULL) {
                // print statement for calloc
                *(hash + index) = calloc(length+1, sizeof(char));
                strcpy(*(hash + index), word);
                printf("Word \"%s\" ==> %d %ccalloc%c\n", word, index, 91, 93);
            }
            else if (*(hash + index) != NULL && strlen(*(hash + index)) == length) {
                // nop
                strcpy(*(hash + index), word);
                printf("Word \"%s\" ==> %d %cnop%c\n", word, index, 91, 93);
            }
            else {
                // print statement for realloc
                *(hash + index) = realloc(*(hash + index), length+1 + sizeof(char));
                strcpy(*(hash + index), word);
                printf("Word \"%s\" ==> %d %crealloc%c\n", word, index, 91, 93);

            }
            
        }
    }
    
    printf("Cache:\n");
    for (int i = 0; i < n; i++) {
        printf("index %d ==> \"%s\"\n", i, *(hash + i));
        free(*(hash + i));
    }
    free(word);
    free(hash);
    fclose(file);
    return 0;
}
