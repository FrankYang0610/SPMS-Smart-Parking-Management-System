//
// input.h
// The Input Module (Part I)
// The input module includes the console ad the parser.
// To make the program structure well, the console and parser are combined into this header.
//

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 512

// ========== The Parser ========== //
char** parse(char*);

// ========== The Console ========== //
void console_init();
void console();

#endif //INPUT_H
