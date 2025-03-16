//
// utils.c
// The Utilities
//

#include "utils.h"

static bool is_separator(const char c) {
    for (int i = 0; SEP_CHARS[i] != '\0'; i++) {
        if (c == SEP_CHARS[i]) {
            return true;
        }
    }
    return false;
}

char* strip(const char* str) {
    if (str == NULL || *str == '\0') {
        return NULL;
    }

    char* _str = strdup(str);
    size_t len = strlen(_str);

    // strip the head and add a new head
    int start = 0;
    while (start < len && is_separator(_str[start])) {
        start++;
    }

    if (start == len) {
        free(_str);
        return NULL;
    }

    // strip the end and add a new end
    int end = (int)len - 1;
    while (end >= start && is_separator(_str[end])) {
        end--;
    }

    _str[end + 1] = '\0';
    return _str;
}

char** split(const char* input, size_t* argc) {
    if (input == NULL || *input == '\0') {
        return NULL;
    }

    *argc = 0;
    char* _input = strdup(input);
    char* _input_orig = _input;  // save a copy of the strdup-ed string. In the end of this func, free _input_orig.

    if (_input == NULL) {
        return NULL;
    }

    size_t len = strlen(_input);

    bool in_arg = false;
    for (int i = 0; i < len; i++) {
        if (is_separator(_input[i])) {
            in_arg = false;
        } else if (!in_arg) { // the beginning of a new argument
            in_arg = true;
            (*argc)++;
        }
    }

    char** result = (char**)malloc(sizeof(char*) * (*argc));

    if (result == NULL) {
        return NULL;
    }

    int pargc = 0; // processed argument count
    char* delimiters = strtok(_input, SEP_CHARS);

    while (delimiters != NULL) {
        result[pargc++] = strdup(delimiters);
        delimiters = strtok(NULL, SEP_CHARS);
    }

    free(_input_orig);
    return result;
}
