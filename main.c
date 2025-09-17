#include <stdio.h>
#include <string.h>
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

repeat:

    // Välj stad
    printf("Välj stad (Eller \"x\" för att avsluta): ");
    char input[50];
    scanf("%49s", input);

    if (strcmp(input, "x") == 0) {
        return 0;
    }

    // Hitta stad
    strcpy(citiesCpy, cities);
    line = strtok(citiesCpy, "\n");

    char lon[20];
    char lat[20];
    char url[200];
    int found = 0;
    while (line != NULL) {
        sscanf(line, "%49[^:]:%19[^:]:%19s", city, lon, lat);
        if (strcmp(input, city) == 0) { // Skapa url
            sprintf(url, "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current_weather=true", lon, lat);
            printf("%s\n", url);
            found = 1;
            break;
        }
        line = strtok(NULL, "\n");
    }

    // Börja om ifall staden inte hittades
    if (found == 0) {
        printf("Staden hittades inte.\n");
        goto repeat;
    }

    // Spara & skriv ut JSON strängen ifall staden hittades
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    CURLcode res;

    struct memory_struct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            printf("Response: %s\n", chunk.memory);
        }
        else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();

goto repeat;

    return 0;
}

/*
    // Skriv till fil
    FILE* f = fopen("data.txt", "w");
    fprintf(f, "Hej världen!\n");
    fclose(f);

    // Läs ifrån fil
    FILE* f = fopen("data.txt", "r");
    char buffer[100];
    fgets(buffer, 100, f);
    printf("%s", buffer);
    fclose(f);

    // Json parsing
    json_t* root = json_loads("{\"temp\": 22}", 0, NULL);
    int temp = json_integer_value(json_object_get(root, "temp"));
    printf("Temperatur: %d\n", temp);
    json_decref(root);
*/