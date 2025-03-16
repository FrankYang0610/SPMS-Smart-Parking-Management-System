//
// input.c
// The Input Module (Part I)
//

#include "input.h"

#include <stdbool.h>

Request fetch_input() {
    Request res;
    return res;
}

char** parse(char* input) {
    if (input == NULL || strlen(input) == 0) {
        return NULL;
    }

    return "";
}

void console_init() {
    printf("~~ WELCOME TO POLYU ~~");
    console();
}

void console() {
    while (1) {
        printf("Please enter booking:");
        // input
        // parse
    }
}
