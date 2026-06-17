#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <stddef.h>
#include <ctype.h>
#include <string.h>

void to_uppercase(char *str);
size_t count_chars(const char *str);
void reverse_string(char *str);

#endif // STRINGUTILS_H