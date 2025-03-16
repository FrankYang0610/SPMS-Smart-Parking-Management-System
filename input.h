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

typedef struct Time {
    int y, m, d;
    int hour, min;
} Time;

typedef enum RequestType {
    PARKING,
    RESERVATION,
    EVENT,
    ESSENTIALS,
    BATCH
} RequestType;

typedef enum EssentialType {
    BATTERY_CABLE,
    LOCKER_UMBRELLA,
    INFLATION_VALET
} EssentialType;

typedef struct Request { 
    Time start, end;
    RequestType type;
    bool parking; // need parking or not
    EssentialType essential;
} Request;


Request fetch_input();

// ========== The Parser ========== //
char** parse(char*);

// ========== The Console ========== //
void console_init();
void console();

#endif //INPUT_H
