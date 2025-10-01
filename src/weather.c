#define weather_h
#ifdef weather_h

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include "weather.h"
#include "jansson/jansson.h"
#include <time.h>
#include <string.h>

int jansson_weather_exists(char *cityName) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    return 1; /* Staden finns inte lokalt */
  }

  json_decref(root);
  return 0; /* Staden finns lokalt */
}

int jansson_weather_is_stale(char *cityName) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return -1;
  }

  json_t *time_val = json_object_get(current_weather, "time");
  if (!json_is_string(time_val)) {
    json_decref(root);
    return -1;
  }
  const char *time_str = json_string_value(time_val);

  struct tm tm_time = {0};
  if (strptime(time_str, "%Y-%m-%dT%H:%M", &tm_time) == 0) {
    fprintf(stderr, "Failed to parse time string: %s\n", time_str);
    json_decref(root);
    return -1;
  }

  time_t weather_time = timegm(&tm_time);
  time_t now = time(NULL);

  json_t *interval_val = json_object_get(current_weather, "interval");
  int interval = (json_is_integer(interval_val))
                     ? (int)json_integer_value(interval_val)
                     : 900;

  double diff = difftime(now, weather_time);

  if (diff > interval) {
    json_decref(root);
    return 0; /* Vädret är gammalt */
  }

  json_decref(root);
  return 1; /* Vädret är inte gammalt */
}

int jansson_weather_write(char *cityName, const char *data) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_loads(data, 0, &error);
  if (!root) {
    fprintf(stderr, "Error parsing JSON data: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return -1;
  }

  if (json_dump_file(root, cityFile, JSON_INDENT(2)) != 0) {
    fprintf(stderr, "Error writing JSON to file: %s\n", cityFile);
    json_decref(root);
    return -1;
  }

  json_decref(root);
  return 0;
}

int jansson_weather_print(char *cityName, int parameter) {
  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return -1;
  }

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return -1;
  }

  switch (parameter) {
    case 1: { // Time
      json_t *time_val = json_object_get(current_weather, "time");
      if (json_is_string(time_val)) {
        printf("Time: %s\n", json_string_value(time_val));
      }
      break;
    }
    case 2: { // Interval
      json_t *interval_val = json_object_get(current_weather, "interval");
      if (json_is_integer(interval_val)) {
        printf("Interval: %d seconds\n", (int)json_integer_value(interval_val));
      }
      break;
    }
    case 3: { // Temperature
      json_t *temp_val = json_object_get(current_weather, "temperature");
      if (json_is_number(temp_val)) {
        printf("Temperature: %.2f °C\n", json_number_value(temp_val));
      }
      break;
    }
    case 4: { // Windspeed
      json_t *wind_val = json_object_get(current_weather, "windspeed");
      if (json_is_number(wind_val)) {
        printf("Windspeed: %.2f km/h\n", json_number_value(wind_val));
      }
      break;
    }
    case 5: { // Winddirection
      json_t *winddir_val = json_object_get(current_weather, "winddirection");
      if (json_is_integer(winddir_val)) {
        printf("Winddirection: %d°\n", (int)json_integer_value(winddir_val));
      }
      break;
    }
    case 6: { // Is_day
      json_t *isday_val = json_object_get(current_weather, "is_day");
      if (json_is_integer(isday_val)) {
        printf("Is day: %s\n", (int)json_integer_value(isday_val) ? "Yes" : "No");
      }
      break;
    }
    case 7: { // Weathercode
      json_t *wcode_val = json_object_get(current_weather, "weathercode");
      if (json_is_integer(wcode_val)) {
        printf("Weathercode: %d\n", (int)json_integer_value(wcode_val));
      }
      break;
    }
    default:
      break;
  }

  json_decref(root);
  return 0;
}

current_weather jansson_weather_fetch(char *cityName) {
  current_weather cw = {0}; // initialize all fields to safe defaults

  char cityFile[55];
  snprintf(cityFile, sizeof(cityFile), "cache/%s.json", cityName);

  json_error_t error;
  json_t *root = json_load_file(cityFile, 0, &error);
  if (!root) {
    fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text,
            error.line, error.column);
    return cw; // return empty struct on error
  }

  json_t *current_weather = json_object_get(root, "current_weather");
  if (!json_is_object(current_weather)) {
    json_decref(root);
    return cw; // return empty struct if missing
  }

  // Extract values
  json_t *time_val = json_object_get(current_weather, "time");
  json_t *interval_val = json_object_get(current_weather, "interval");
  json_t *temp_val = json_object_get(current_weather, "temperature");
  json_t *wind_val = json_object_get(current_weather, "windspeed");
  json_t *winddir_val = json_object_get(current_weather, "winddirection");
  json_t *isday_val = json_object_get(current_weather, "is_day");
  json_t *wcode_val = json_object_get(current_weather, "weathercode");

  if (json_is_string(time_val)) {
    strncpy(cw.time, json_string_value(time_val), sizeof(cw.time) - 1);
  }
  if (json_is_integer(interval_val)) {
    cw.interval = (int)json_integer_value(interval_val);
  }
  if (json_is_number(temp_val)) {
    cw.temperature = json_number_value(temp_val);
  }
  if (json_is_number(wind_val)) {
    cw.windspeed = json_number_value(wind_val);
  }
  if (json_is_integer(winddir_val)) {
    cw.winddirection = (int)json_integer_value(winddir_val);
  }
  if (json_is_integer(isday_val)) {
    cw.is_day = (int)json_integer_value(isday_val);
  }
  if (json_is_integer(wcode_val)) {
    cw.weathercode = (int)json_integer_value(wcode_val);
  }

  json_decref(root);
  return cw;
}

#endif