#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinydir.h>
#include <jansson.h>

#include "cities.h"
#include "utils.h"

const char* cities_list = 	"Stockholm:59.3293:18.0686\n"
							"Göteborg:57.7089:11.9746\n"
							"Malmö:55.6050:13.0038\n"
							"Uppsala:59.8586:17.6389\n"
							"Västerås:59.6099:16.5448\n"
							"Örebro:59.2741:15.2066\n"
							"Linköping:58.4109:15.6216\n"
							"Helsingborg:56.0465:12.6945\n"
							"Jönköping:57.7815:14.1562\n"
							"Norrköping:58.5877:16.1924\n"
							"Lund:55.7047:13.1910\n"
							"Gävle:60.6749:17.1413\n"
							"Sundsvall:62.3908:17.3069\n"
							"Umeå:63.8258:20.2630\n"
							"Luleå:65.5848:22.1567\n"
							"Kiruna:67.8558:20.2253\n";

int cities_load_from_disk(cities_t* cities);
int cities_load_from_string_list(cities_t* cities);
int cities_save_to_disk(cities_t* cities);
int cities_add_city(cities_t* cities, city_t* city);

int cities_init(cities_t **cities_ptr)
{
    *cities_ptr = malloc(sizeof(cities_t));
    if (*cities_ptr == NULL) return -1;

    cities_t *cities = *cities_ptr;

    memset(&cities->list, 0, sizeof(LinkedList));

    create_folder("cities_cache"); // ensure the directory exists

    cities_load_from_disk(cities);
    cities_load_from_string_list(cities);

    cities_save_to_disk(cities);

    return 0;
}

int cities_load_from_disk(cities_t *cities)
{
    tinydir_dir dir;
    const char* folder_path = "cities_cache";

    if (tinydir_open(&dir, folder_path) == -1)
    {
        perror("Error opening cities directory");
        return -1;
    }

    while (dir.has_next)
    {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1)
        {
            perror("Error reading file in cities directory");
            tinydir_close(&dir);
            return -1;
        }

        if (!file.is_dir)
        {
            json_t *city_json = json_load_file(file.path, 0, NULL);
            if (city_json)
            {
                const char *name = json_string_value(json_object_get(city_json, "name"));
                double latitude = json_number_value(json_object_get(city_json, "latitude"));
                double longitude = json_number_value(json_object_get(city_json, "longitude"));
                
                char lat_buffer[32];
                char lon_buffer[32];
                snprintf(lat_buffer, sizeof(lat_buffer), "%.6f", latitude);
                snprintf(lon_buffer, sizeof(lon_buffer), "%.6f", longitude);

                city_t *city = NULL;
                city_init(name, lat_buffer, lon_buffer, &city);
                if (city) cities_add_city(cities, city);

                json_decref(city_json);
            }
        }

        if (tinydir_next(&dir) == -1)
        {
            perror("Error moving to next file in cities directory");
            tinydir_close(&dir);
            return -1;
        }
    }

    tinydir_close(&dir);

    return 0;
}

int cities_load_from_string_list(cities_t *cities)
{
    if (!cities) return -1;

    char* list_copy = strdup(cities_list);
    if (!list_copy) return -1;

    char* ptr = list_copy;

    char* name = NULL;
    char* lat_str = NULL;
    char* lon_str = NULL;

    do {
        name = ptr;
        lat_str = strchr(ptr, ':');
        if (!lat_str) break;
        *lat_str = '\0';
        lat_str++;

        lon_str = strchr(lat_str, ':');
        if (!lon_str) break;
        *lon_str = '\0';
        lon_str++;

        char* end_ptr = strchr(lon_str, '\n');
        if (end_ptr) {
            *end_ptr = '\0';
            ptr = end_ptr + 1;
        } else {
            ptr = NULL;
        }

        city_t *existing_city = NULL;
        if (cities_get_city_by_name(cities, name, &existing_city) == 0) {
            continue;
        }

        city_t* city = NULL;

        city_init(name, lat_str, lon_str, &city);

        if (city) {
            cities_add_city(cities, city);
        }

    } while (ptr);

    return 0;
}

int cities_save_to_disk(cities_t *cities)
{
    if (!cities) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "cities_cache/%s.json", city->name);
            FILE *file = fopen(filepath, "w");
            if (file) {
                json_t *city_json = json_object();
                json_object_set_new(city_json, "name", json_string(city->name));
                json_object_set_new(city_json, "latitude", json_real(city->latitude));
                json_object_set_new(city_json, "longitude", json_real(city->longitude));
                char *json_str = json_dumps(city_json, JSON_INDENT(4));
                fprintf(file, "%s", json_str);
                free(json_str);
                json_decref(city_json);
                fclose(file);
            }
        }
        node = node->front;
    }

    return 0;
}

int cities_add_city(cities_t *cities, city_t *city)
{
    LinkedList_append(&cities->list, city);
    return 0;
}

int cities_remove_city(cities_t *cities, const char *name)
{
    if (!cities || !name) return -1;
    Node *node = cities->list.head;
    while (node) {
        Node *next = node->front;
        city_t *city = (city_t *)node->item;
        if (city && city->name && strcmp(city->name, name) == 0) {
            LinkedList_remove(&cities->list, node, free);
            return 0;
        }
        node = next;
    }
    return -1;
}

int cities_get_city_by_name(cities_t *cities, const char *name, city_t **city_ptr)
{
    if (!cities || !name) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name && strcmp(city->name, name) == 0) {
            if (city_ptr) *city_ptr = city;
            return 0;
        }
        node = node->front;
    }

    return -1;
}

int cities_get_city_by_index(cities_t *cities, int index, city_t **city_ptr)
{
    if (!cities || index < 0) return -1;
    Node *node = cities->list.head;
    int current_index = 0;
    while (node) {
        if (current_index == index) {
            if (city_ptr) *city_ptr = (city_t *)node->item;
        }
        node = node->front;
        current_index++;
    }

    return 0;
}

int cities_sort_by_name(cities_t *cities)
{
    if (!cities) return -1;

    Node *i, *j;
    for (i = cities->list.head; i != NULL; i = i->front) {
        for (j = i->front; j != NULL; j = j->front) {
            city_t *city_i = (city_t *)i->item;
            city_t *city_j = (city_t *)j->item;
            if (city_i && city_j && strcmp(city_i->name, city_j->name) > 0) {
                // Swap items
                void *temp = i->item;
                i->item = j->item;
                j->item = temp;
            }
        }
    }

    return 0;
}

int cities_sort_by_distance(cities_t *cities, const char *city_name)
{
    if (!cities || !city_name) return -1;

    // Find the reference city
    city_t *ref_city = NULL;
    if (cities_get_city_by_name(cities, city_name, &ref_city) != 0 || !ref_city) {
        return -1; // City not found
    }

    float latitude = ref_city->latitude;
    float longitude = ref_city->longitude;

    Node *i, *j;
    for (i = cities->list.head; i != NULL; i = i->front) {
        for (j = i->front; j != NULL; j = j->front) {
            city_t *city_i = (city_t *)i->item;
            city_t *city_j = (city_t *)j->item;
            if (city_i && city_j) {
                float dist_i = (city_i->latitude - latitude) * (city_i->latitude - latitude) +
                               (city_i->longitude - longitude) * (city_i->longitude - longitude);
                float dist_j = (city_j->latitude - latitude) * (city_j->latitude - latitude) +
                               (city_j->longitude - longitude) * (city_j->longitude - longitude);
                if (dist_i > dist_j) {
                    // Swap items
                    void *temp = i->item;
                    i->item = j->item;
                    j->item = temp;
                }
            }
        }
    }

    return 0;
}

int cities_print(cities_t *cities)
{
    if (!cities) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name) {
            //printf("%s\n", city->name);
            printf("%s: Latitude: %.4f, Longitude: %.4f\n", city->name, city->latitude, city->longitude);
        }
        node = node->front;
    }

    return 0;
}

int cities_print_pretty(cities_t *cities)
{
    if (!cities) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name) {
            printf("City: %s\n", city->name);
            printf("  Latitude:  %.6f\n", city->latitude);
            printf("  Longitude: %.6f\n", city->longitude);
            printf("-----------------------\n");
        }
        node = node->front;
    }

    return 0;
}

int cities_dispose(cities_t **cities_ptr)
{
    if (!cities_ptr || !*cities_ptr) return -1;
    
    cities_t *cities = *cities_ptr;
    LinkedList_clear(&cities->list, free);
    free(cities);
    *cities_ptr = NULL;
    return 0;
}
