#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

typedef enum {
    Input_Command_Error = -1,
    Input_Command_OK = 0,
    Input_Command_Exit = 1,
    Input_Command_Invalid = 2
} Input_Command;

static inline Input_Command input_select_city(char** cityName)
{
    printf("Enter city name (or 'exit' to quit): ");
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return Input_Command_Error;
    }

    // Remove newline character
    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp(buffer, "exit") == 0) {
        return Input_Command_Exit;
    } else if (strlen(buffer) == 0) {
        return Input_Command_Invalid;
    } else {
        *cityName = strdup(buffer);
        return Input_Command_OK;
    }
}