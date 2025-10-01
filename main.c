//#define _GNU_SOURCE
//#define _XOPEN_SOURCE 700

#include <stdio.h>
#include "http.h"
#include "input.h"
#include "weather.h"

int main()
{
    http_init();

    char* cityName = NULL;
    if (input_select_city(&cityName) == 0){
        if (jansson_weather_exists(cityName) == 1) {
            printf("City not found locally. Fetching from API...\n");
        } else {
            if (jansson_weather_is_stale(cityName) == 1) {
                printf("Local data is stale. Fetching updated data from API...\n");
            } else {
                printf("Local data is fresh. Using cached data.\n");
            }
        }
    } else {
        printf("No city selected or an error occurred.\n");
    }

    return 0;
}
