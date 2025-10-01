//#define _GNU_SOURCE
//#define _XOPEN_SOURCE 700

#include <stdio.h>
#include "http.h"
#include "input.h"

int main()
{
    http_init();

    char* cityName = NULL;
    if (input_select_city(&cityName) == 0) {
        printf("Selected city: %s\n", cityName);
    } else {
        printf("No city selected or an error occurred.\n");
    }

    // char* cityName;
    // run gui(*cityName)
    //
    // if local exists:
    //     if local is stale:
    //         charData = fetch from http(cityName)
    //         write char* to file with jansson(cityName, charData)
    //         goto print;
    //
    // charData = fetch from http(cityName)
    // write char* to file with jansson(cityName, charData)
    //
    // print(cityName)

    return 0;
}
