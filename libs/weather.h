#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

typedef struct {
    double latitude;
    double longitude;
    double generationtime_ms;
    int utc_offset_seconds;
    char* timezone;
    char* timezone_abbreviation;
    double elevation;

    char* unit_time;
    char* unit_interval;
    char* unit_temperature;
    char* unit_windspeed;
    char* unit_winddirection;
    char* unit_is_day;
    char* unit_weathercode;

    char* time;
    int interval;
    double temperature;
    double windspeed;
    int winddirection;
    int is_day;
    int weathercode;
} weather_t;

// ========== Cache Management Functions ==========
int does_weather_cache_exist(const char* city_name);
int is_weather_cache_stale(const char* city_name, int max_age_seconds);
int load_weather_from_cache(const char* city_name, char** jsonStr);
int save_weather_to_cache(const char* city_name, const char* jsonStr);

// ========== Serialization Functions ==========
int process_openmeteo_response(const char* api_response, char** client_response); // Used by server to process API response
int deserialize_weather_response(const char* client_response, weather_t* weather); // Used by client to parse response

void free_weather(weather_t* weather);

#endif // WEATHER_H