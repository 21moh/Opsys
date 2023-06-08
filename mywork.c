#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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
    char *word = calloc(1000, sizeof(char));
    int i = 0;
    while (fscanf(file, "%s", word) == 1 && i < n) {
        // Process the word (e.g., print it)
        //printf("test %s\n", word);
        int length = strlen(word);
        if (length > 1) {
             *(hash + i) = calloc(length+1, sizeof(char));
            strcpy(*(hash + i), word);
            printf("%s\n", *(hash + i));
            i = i + 1;
            printf("\n");
        }
       
    }

    for (int i = 0; i < n; i++) {
        free(*(hash + i));
    }
    free(word);

    fclose(file);
    return 0;
}