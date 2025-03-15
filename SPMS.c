/*
 * COMP2432 Operating Systems Group Project
 * SPMS - Smart Parking Management System
 * Project Pica Magpie [https://en.wikipedia.org/wiki/Magpie]
 *
 * WANG Yuqi
 * YANG Xikun
 * LIU Siyuan
 * JIN Yixiao
 *
 * ... This program is under construction
 *
 */

#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 512

// ============================== Input Module ============================== //

/*
 * Command: endProgram
 * This command does nothing but to return a "Bye!" message.
 * The real endProgram logic should be implemented by the console module.
 * Required discussion.
 */
char* endProgram() {
    return "Bye!\n";
}

/*
 * Parser
 * The parser parses the input, that is, split the raw input into several segments.
 * The parser recognizes the first segment as the command, and the rests are the arguments.
 * For example, if the input is "addParking –member_A 2025-05-16 10:00 3.0 battery;",
 * the parser firstly split the input by spacer, and then recognizes "addParking" as the command,
 * and "-memberA", "2025-05-16", "10:30", "3.0" and "battery" as the arguments of the "addParking" command.
 */
char* parser(char* input) {
    if (input == NULL) {
        return "Error: Empty command.\n";
    }

    size_t len = strlen(input);

    if (!len) {
        return "Error: Empty command.\n";
    }

    if (input[len - 1] != ';') {
        return "Error: The command should end with ';'.\n";
    }

    // placeholder
    if (strcmp(input, "endProgram;") == 0) {
        return endProgram();
    }

    // placeholder
    return "Error: Invalid command!\n";
}

/**
 * Console
 */
int console() {
    printf("~~ Welcome to PolyU ~~\n");

    while (1) {
        printf("Please enter booking:\n");

        char input[BUFFER_SIZE];
        scanf("%s", &input);

        // Should status be a struct? required discussion here.
        char* status = parser(input); // ... process the command and set the status string

        printf("-> %s\n", status);

        if (strcmp(input, "endProgram;") == 0) {
            break;
        }
    }

    return 1; // Any fatal error occurred.
}

int main(void) {
    console();
    return 0;
}
