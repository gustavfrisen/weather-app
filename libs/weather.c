#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

#include "weather.h"
#include "utils.h"

int parse_openmeteo_json_to_weather(const json_t* json_obj, weather_t* weather);
int serialize_weather_to_json(const weather_t* weather, json_t** json_obj);

// ========== Helper Functions ==========

static void get_cache_file_path(const char* city_name, char* path, size_t path_size) {
    snprintf(path, path_size, "weather_cache/%s.json", city_name);
}

static char* create_lowercase_copy(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* lower_str = malloc(len + 1);
    if (!lower_str) return NULL;
    
    for (size_t i = 0; i <= len; i++) {
        lower_str[i] = tolower((unsigned char)str[i]);
    }
    
    return lower_str;
}

// ========== Basic Interface Functions ==========

int does_weather_cache_exist(const char* city_name) {
    // Ensure cache directory exists
    create_folder("weather_cache");

    if (!city_name) return 0;

    char* lower_city = create_lowercase_copy(city_name);
    if (!lower_city) return -1;

    char cache_path[512];
    get_cache_file_path(lower_city, cache_path, sizeof(cache_path));

    free(lower_city);
    
    json_error_t error;
    json_t* json_obj = json_load_file(cache_path, 0, &error);
    
    if (json_obj) {
        json_decref(json_obj);
        return 1;
    }
    
    return 0;
}

int is_weather_cache_stale(const char* city_name, int max_age_seconds) {
    if (!city_name || max_age_seconds < 0) return 1;

    char* lower_city = create_lowercase_copy(city_name);
    if (!lower_city) return -1;
    
    char cache_path[512];
    get_cache_file_path(lower_city, cache_path, sizeof(cache_path));

    free(lower_city);
    
    struct stat file_stat;
    if (stat(cache_path, &file_stat) != 0) {
        return 1;
    }
    
    time_t current_time = time(NULL);
    time_t file_age = current_time - file_stat.st_mtime;
    
    return file_age > max_age_seconds ? 1 : 0;
}

int load_weather_from_cache(const char* city_name, char** jsonStr) {
    if (!city_name || !jsonStr) return -1;

    char* lower_city = create_lowercase_copy(city_name);
    if (!lower_city) return -1;
    
    char cache_path[512];
    get_cache_file_path(lower_city, cache_path, sizeof(cache_path));

    free(lower_city);
    
    json_error_t error;
    json_t* json_obj = json_load_file(cache_path, 0, &error);
    if (!json_obj) {
        return -1;
    }
    
    *jsonStr = json_dumps(json_obj, JSON_COMPACT);
    json_decref(json_obj);
    
    return *jsonStr ? 0 : -1;
}

int save_weather_to_cache(const char* city_name, const char* jsonStr) {
    if (!city_name || !jsonStr) return -1;

    char* lower_city = create_lowercase_copy(city_name);
    if (!lower_city) return -1;
    
    char cache_path[512];
    get_cache_file_path(lower_city, cache_path, sizeof(cache_path));

    free(lower_city);
    
    json_error_t error;
    json_t* json_obj = json_loads(jsonStr, 0, &error);
    if (!json_obj) {
        return -1;
    }
    
    int result = json_dump_file(json_obj, cache_path, JSON_INDENT(2));
    json_decref(json_obj);
    
    return result == 0 ? 0 : -1;
}

int process_openmeteo_response(const char *api_response, char **client_response)
{
    weather_t weather;

    // Parse API response into weather structure
    json_error_t error;
    json_t* root_api = json_loads(api_response, 0, &error);
    if (!root_api) return -1;

    int result = parse_openmeteo_json_to_weather(root_api, &weather);
    json_decref(root_api);
    if (result != 0) return -1;

    // Serialize weather structure into client response JSON string
    json_t* root_client;
    if (serialize_weather_to_json(&weather, &root_client) != 0) {
        free_weather(&weather);
        return -1;
    }
    *client_response = json_dumps(root_client, JSON_COMPACT);
    json_decref(root_client);

    free_weather(&weather);
    return 0;
}

int deserialize_weather_response(const char *client_response, weather_t *weather)
{
    // Parse client response JSON string into weather structure
    json_error_t error;
    json_t* root = json_loads(client_response, 0, &error);
    if (!root) return -1;

    int result = parse_openmeteo_json_to_weather(root, weather);
    json_decref(root);
    if (result != 0) return -1;
   
    return 0;
}

// ========== OpenMeteo Conversion Helpers ==========

int parse_openmeteo_json_to_weather(const json_t* json_obj, weather_t* weather) {
    if (!json_obj || !weather) return -1;
    
    memset(weather, 0, sizeof(weather_t));
    
    json_t* val;
    
    val = json_object_get(json_obj, "latitude");
    weather->latitude = json_is_real(val) ? json_real_value(val) : 0.0;
    
    val = json_object_get(json_obj, "longitude");
    weather->longitude = json_is_real(val) ? json_real_value(val) : 0.0;
    
    val = json_object_get(json_obj, "generationtime_ms");
    weather->generationtime_ms = json_is_real(val) ? json_real_value(val) : 0.0;
    
    val = json_object_get(json_obj, "utc_offset_seconds");
    weather->utc_offset_seconds = json_is_integer(val) ? (int)json_integer_value(val) : 0;
    
    val = json_object_get(json_obj, "timezone");
    weather->timezone = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
    
    val = json_object_get(json_obj, "timezone_abbreviation");
    weather->timezone_abbreviation = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
    
    val = json_object_get(json_obj, "elevation");
    weather->elevation = json_is_real(val) ? json_real_value(val) : 0.0;
    
    json_t* units_obj = json_object_get(json_obj, "current_weather_units");
    if (json_is_object(units_obj)) {
        val = json_object_get(units_obj, "time");
        weather->unit_time = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "interval");
        weather->unit_interval = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "temperature");
        weather->unit_temperature = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "windspeed");
        weather->unit_windspeed = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "winddirection");
        weather->unit_winddirection = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "is_day");
        weather->unit_is_day = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(units_obj, "weathercode");
        weather->unit_weathercode = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
    }
    
    json_t* current_obj = json_object_get(json_obj, "current_weather");
    if (json_is_object(current_obj)) {
        val = json_object_get(current_obj, "time");
        weather->time = (json_is_string(val) && json_string_value(val)) ? strdup(json_string_value(val)) : NULL;
        
        val = json_object_get(current_obj, "interval");
        weather->interval = json_is_integer(val) ? (int)json_integer_value(val) : 0;
        
        val = json_object_get(current_obj, "temperature");
        weather->temperature = json_is_real(val) ? json_real_value(val) : 0.0;
        
        val = json_object_get(current_obj, "windspeed");
        weather->windspeed = json_is_real(val) ? json_real_value(val) : 0.0;
        
        val = json_object_get(current_obj, "winddirection");
        weather->winddirection = json_is_integer(val) ? (int)json_integer_value(val) : 0;
        
        val = json_object_get(current_obj, "is_day");
        weather->is_day = json_is_integer(val) ? (int)json_integer_value(val) : 0;
        
        val = json_object_get(current_obj, "weathercode");
        weather->weathercode = json_is_integer(val) ? (int)json_integer_value(val) : 0;
    }
    
    return 0;
}

int serialize_weather_to_json(const weather_t* weather, json_t** json_obj) {
    if (!weather || !json_obj) return -1;
    
    *json_obj = json_object();
    if (!*json_obj) return -1;
    
    json_object_set_new(*json_obj, "latitude", json_real(weather->latitude));
    json_object_set_new(*json_obj, "longitude", json_real(weather->longitude));
    json_object_set_new(*json_obj, "generationtime_ms", json_real(weather->generationtime_ms));
    json_object_set_new(*json_obj, "utc_offset_seconds", json_integer(weather->utc_offset_seconds));
    json_object_set_new(*json_obj, "timezone", weather->timezone ? json_string(weather->timezone) : json_string("GMT"));
    json_object_set_new(*json_obj, "timezone_abbreviation", weather->timezone_abbreviation ? json_string(weather->timezone_abbreviation) : json_string("GMT"));
    json_object_set_new(*json_obj, "elevation", json_real(weather->elevation));
    
    json_t* units_obj = json_object();
    if (!units_obj) {
        json_decref(*json_obj);
        return -1;
    }
    json_object_set_new(units_obj, "time", weather->unit_time ? json_string(weather->unit_time) : json_string("iso8601"));
    json_object_set_new(units_obj, "interval", weather->unit_interval ? json_string(weather->unit_interval) : json_string("seconds"));
    json_object_set_new(units_obj, "temperature", weather->unit_temperature ? json_string(weather->unit_temperature) : json_string("°C"));
    json_object_set_new(units_obj, "windspeed", weather->unit_windspeed ? json_string(weather->unit_windspeed) : json_string("km/h"));
    json_object_set_new(units_obj, "winddirection", weather->unit_winddirection ? json_string(weather->unit_winddirection) : json_string("°"));
    json_object_set_new(units_obj, "is_day", weather->unit_is_day ? json_string(weather->unit_is_day) : json_string(""));
    json_object_set_new(units_obj, "weathercode", weather->unit_weathercode ? json_string(weather->unit_weathercode) : json_string("wmo code"));
    json_object_set_new(*json_obj, "current_weather_units", units_obj);
    
    json_t* current_obj = json_object();
    if (!current_obj) {
        json_decref(*json_obj);
        return -1;
    }
    json_object_set_new(current_obj, "time", weather->time ? json_string(weather->time) : json_null());
    json_object_set_new(current_obj, "interval", json_integer(weather->interval));
    json_object_set_new(current_obj, "temperature", json_real(weather->temperature));
    json_object_set_new(current_obj, "windspeed", json_real(weather->windspeed));
    json_object_set_new(current_obj, "winddirection", json_integer(weather->winddirection));
    json_object_set_new(current_obj, "is_day", json_integer(weather->is_day));
    json_object_set_new(current_obj, "weathercode", json_integer(weather->weathercode));
    json_object_set_new(*json_obj, "current_weather", current_obj);
    
    return 0;
}

// ========== Memory Management ==========

void free_weather(weather_t* weather) {
    if (!weather) return;
    
    free(weather->timezone);
    free(weather->timezone_abbreviation);
    free(weather->unit_time);
    free(weather->unit_interval);
    free(weather->unit_temperature);
    free(weather->unit_windspeed);
    free(weather->unit_winddirection);
    free(weather->unit_is_day);
    free(weather->unit_weathercode);
    free(weather->time);
    
    memset(weather, 0, sizeof(weather_t));
}