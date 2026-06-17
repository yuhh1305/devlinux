#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string_to_process>\n", argv[0]);
        return 1;
    }

    char *input_str = strdup(argv[1]);
    if (!input_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    printf("Original String : %s\n", argv[1]);
    printf("Character Count : %zu\n", count_chars(input_str));

    to_uppercase(input_str);
    printf("Uppercase Output: %s\n", input_str);

    reverse_string(input_str);
    printf("Reversed Output : %s\n", input_str);

    free(input_str);
    return 0;
}