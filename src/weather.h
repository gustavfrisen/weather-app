#define weather_h
#ifdef weather_h

typedef struct {
  char time[32]; /* todo how long can a ISO 8601 time string be? 32 might be to small */
  int interval;
  double temperature;
  double windspeed;
  int winddirection;
  int is_day;
  int weathercode;
} current_weather;

// Jansson:
int jansson_weather_exists(char *cityName);
int jansson_weather_is_stale(char *cityName);
int jansson_weather_write(char *cityName, const char *data);
int jansson_weather_print(char *cityName, int parameter);
current_weather jansson_weather_fetch(char *cityName);

#endif