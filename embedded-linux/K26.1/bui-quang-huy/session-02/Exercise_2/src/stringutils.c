#include "stringutils.h"

void to_uppercase(char *str) {
    if (!str) return;
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

size_t count_chars(const char *str) {
    if (!str) return 0;
    size_t count = 0;
    while (*str++) {
        count++;
    }
    return count;
}

void reverse_string(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    if (len == 0) return;
    
    size_t i = 0;
    size_t j = len - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}