//#define _GNU_SOURCE
//#define _XOPEN_SOURCE 700

#include <stdio.h>
#include "http.h"
#include "cities.h"
#include "input.h"
#include "weather.h"

int main()
{
    http_init();
    
    Cities* cities = NULL;
    cities_init(&cities);
    
    while (1) {
        cities_print(cities);
        
        char* cityName = NULL;
        int result = input_select_city(&cityName);

        City* city = NULL;
        cities_get_name(cities, cityName, &city);

        printf("\n");

        if (result == 0){
            if (weather_exists(cityName) == 1) {
                printf("City not found locally. Fetching from API...\n");
                weather_write(cityName, http_fetch(city->latitude, city->longitude));
            } else {
                if (weather_is_stale(cityName) == 1) {
                    printf("Local data is stale. Fetching updated data from API...\n");
                    weather_write(cityName, http_fetch(city->latitude, city->longitude));
                } else {
                    printf("Local data is fresh. Using cached data.\n");
                }
            }

            weather_print(cityName, 1); // Print time as an example
            weather_print(cityName, 3); // Print temperature as an example
        } else if (result == 1) {
            printf("Exiting program.\n");
            return 0;
        } else {
            printf("An error occurred while selecting city.\n");
            return -1;
        }

        printf("\n");
    }

    return 0;
}
