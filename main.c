#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <jansson.h>

const char* cities = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n";

struct memory_struct {
    char *memory;
    size_t size;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct memory_struct *mem = (struct memory_struct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0; // null-terminate

    return realsize;
}

int main()
{
    // Skriv ut städer
    char citiesCpy[2000];
    strcpy(citiesCpy, cities);

    char *line = strtok(citiesCpy, "\n");;
    char city[50];
    while (line != NULL) {
        sscanf(line, "%49[^:]:%*s:%*s", city);
        printf("%s\n", city);
        line = strtok(NULL, "\n");
    }

    while (1) {
        // Välj stad
        printf("Välj stad (Eller \"x\" för att avsluta): ");
        char input[50];
        if (scanf("%49s", input) != 1) {
            fprintf(stderr, "Felaktik inmatning. Försök igen.\n");
            continue;
        }
 
        if (strcmp(input, "x") == 0) return 0;

        // Hitta stad
        strcpy(citiesCpy, cities);
        line = strtok(citiesCpy, "\n");

        char lon[20], lat[20], url[200];
        int found = 0;
        while (line != NULL) {
            sscanf(line, "%49[^:]:%19[^:]:%19s", city, lon, lat);
            if (strcmp(input, city) == 0) { // Skapa url
                snprintf(url, sizeof(url), "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current_weather=true", lon, lat);
                found = 1;
                break;
            }
            line = strtok(NULL, "\n");
        }

        // Börja om ifall staden inte hittades
        if (found == 0) {
            printf("Staden hittades inte.\n");
            continue;
        }

        char cityFile[55];
        snprintf(cityFile, sizeof(cityFile), "%s.json", input);

        // Finns staden lokalt?
        json_error_t error;
        json_t *root = json_load_file(cityFile, 0, &error);
        if (!root) { // Staden finns inte lokalt
            fprintf(stderr, "Error loading JSON: %s (line %d, col %d)\n", error.text, error.line, error.column);
        }
        else { // Staden finns lokalt
            json_t *current_weather = json_object_get(root, "current_weather");
            if (!json_is_object(current_weather)) {
                json_decref(root);
            }

            json_t *time_val = json_object_get(current_weather, "time");
            if (!json_is_string(time_val)) {
                json_decref(root);
            }
            const char *time_str = json_string_value(time_val);

            struct tm tm_time = {0};
            if (strptime(time_str, "%Y-%m-%dT%H:%M", &tm_time) == 0) {
                fprintf(stderr, "Failed to parse time string: %s\n", time_str);
                json_decref(root);
            }

            time_t weather_time = timegm(&tm_time);
            time_t now = time(NULL);

            json_t *interval_val = json_object_get(current_weather, "interval");
            int interval = (json_is_integer(interval_val)) ? (int)json_integer_value(interval_val) : 900;

            double diff = difftime(now, weather_time);
            
            // Är vädret gammalt?
            if (diff > interval) { // Vädret är gammalt
                printf("Vädret är gammalt. Hämtar nytt från meteo...\n");
                json_decref(root);
            }
            else { // Vädret är inte gammalt
                json_t *temp_val = json_object_get(current_weather, "temperature");
                json_t *wind_val = json_object_get(current_weather, "windspeed");

                if (json_is_number(temp_val) && json_is_number(wind_val)) {
                    double temp = json_number_value(temp_val);
                    double wind = json_number_value(wind_val);

                    printf("Vädret är inte gammalt!\n");
                    printf("  Temperatur: %.1f °C\n", temp);
                    printf("  Vind: %.1f km/h\n", wind);
                }
                json_decref(root);
                continue;
            }
        }

        // Hämta information från meteo
        curl_global_init(CURL_GLOBAL_ALL);
        CURL *curl = curl_easy_init();
        CURLcode res;

        if (curl) {
            struct memory_struct chunk = {malloc(1), 0};
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            
            res = curl_easy_perform(curl);

            // Spara till json fil
            if (res == CURLE_OK) {
                printf("%s\n", chunk.memory);

                json_error_t error;
                json_t *root = json_loads(chunk.memory, 0, &error);

                if (!root) {
                    fprintf(stderr, "JSON parsing error: %s (line %d, column %d)\n", error.text, error.line, error.column);
                }
                else {
                    if (json_dump_file(root, cityFile, JSON_INDENT(4)) != 0) {
                        fprintf(stderr, "Error writing JSON to file\n");
                    }
                    else {
                        printf("JSON sparad till %s\n", cityFile);
                    }
                    json_decref(root);
                }
            }
            else {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
            free(chunk.memory);
        }

        curl_global_cleanup();
    }

    return 0;
}
