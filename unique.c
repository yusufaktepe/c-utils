#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

#define MAX_LINE_LENGTH 1024

typedef struct Line {
    char *text;
    UT_hash_handle hh;
} Line;

int main() {
    char buffer[MAX_LINE_LENGTH];
    Line *lines = NULL, *entry;

    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        HASH_FIND_STR(lines, buffer, entry);
        if (!entry) {
            entry = malloc(sizeof(Line));
            if (!entry) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            entry->text = strdup(buffer);
            if (!entry->text) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }
            HASH_ADD_KEYPTR(hh, lines, entry->text, strlen(entry->text), entry);
            printf("%s\n", entry->text);
        }
    }

    Line *current, *tmp;
    HASH_ITER(hh, lines, current, tmp) {
        HASH_DEL(lines, current);
        free(current->text);
        free(current);
    }

    return 0;
}

