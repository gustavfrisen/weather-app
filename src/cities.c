#include "cities.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "tinydir.h"
#include "jansson.h"

const char* cities = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n";

int cities_init(Cities** _CitiesPtr) {
    if (_CitiesPtr == NULL) {
        return -1;
    }

    Cities* _Cities = (Cities*)malloc(sizeof(Cities));
    if (_Cities == NULL) {
        return -2;
    }

    create_folder("cities");
    create_folder("cache");

    LinkedList_Initialize(&_Cities->list);

	cities_add_from_cities_folder(_Cities);
    cities_add_from_string_list(_Cities, cities);

	cities_save_to_cities_folder(_Cities);

    *(_CitiesPtr) = _Cities;

    return 0;
}

void cities_add_from_cities_folder(Cities* _cities) {
	if (_cities == NULL)
		return;

	tinydir_dir dir;
	if (tinydir_open(&dir, "cities") == -1)
	{
		printf("Failed to open 'cities' directory\n");
		return;
	}

	while (dir.has_next)
	{
		tinydir_file file;
		if (tinydir_readfile(&dir, &file) == -1)
		{
			printf("Failed to read file in 'cities' directory\n");
			tinydir_close(&dir);
			return;
		}

		if (!file.is_dir)
		{
			const char *ext = strrchr(file.name, '.');
			if (ext && strcmp(ext, ".json") == 0)
			{
				json_error_t error;
				json_t *root = json_load_file(file.path, 0, &error);
				if (!root) {
					fprintf(stderr, "JSON error in %s: %s (line %d, column %d)\n", file.path, error.text, error.line, error.column);
				} else if (json_is_object(root)) {
					json_t *jname = json_object_get(root, "name");
					json_t *jlatitude = json_object_get(root, "latitude");
					json_t *jlongitude = json_object_get(root, "longitude");

					if (json_is_string(jname) && json_is_number(jlatitude) && json_is_number(jlongitude)) {
						const char *name = json_string_value(jname);
						const char *latitude = json_string_value(jlatitude);
						const char *longitude = json_string_value(jlongitude);

						cities_create(_cities, name, latitude, longitude, NULL);
					} else {
						printf("Invalid city format in %s\n", file.path);
					}
					json_decref(root);
				}
			}
		}

		tinydir_next(&dir);
	}
}

void cities_add_from_string_list(Cities* _cities, const char* _StringList) {
    char* list_copy = strdup(_StringList);
	if(list_copy == NULL)
	{
		printf("Failed to allocate memory for list copy\n");
		return;
	}

	char* ptr = list_copy;

	char* name = NULL;
	char* lat_str = NULL;
	char* lon_str = NULL;
	do
	{
		if(name == NULL)
		{
			name = ptr;
		}
		else if(lat_str == NULL)
		{
			if(*(ptr) == ':')
			{
				lat_str = ptr + 1;
				*(ptr) = '\0';
			}
		}
		else if(lon_str == NULL)
		{
			if(*(ptr) == ':')
			{
				lon_str = ptr + 1;
				*(ptr) = '\0';
			}
		}
		else
		{
			if(*(ptr) == '\n')
			{
				*(ptr) = '\0';

				cities_create(_cities, name, lat_str, lon_str, NULL);

				name = NULL;
				lat_str = NULL;
				lon_str = NULL;
			}
		}

		ptr++;

	} while (*(ptr) != '\0');

	free(list_copy);
}

void cities_save_to_cities_folder(Cities* _cities) {
	if (_cities == NULL || _cities->list.head == NULL) {
		printf("No cities to write\n");
		return;
	}

	LinkedList_Node *current = _cities->list.head;
	while (current != NULL) {
		City *city = (City*)current->item;

		if (city && city->name) {
			json_t *root = json_object();
			json_object_set_new(root, "name", json_string(city->name));
			json_object_set_new(root, "latitude", json_real(city->latitude));
			json_object_set_new(root, "longitude", json_real(city->longitude));

			char filename[512];
			snprintf(filename, sizeof(filename), "cities/%s.json", city->name);

			if (json_dump_file(root, filename, JSON_INDENT(4)) == -1) {
				fprintf(stderr, "Failed to write JSON file: %s\n", filename);
			} else {
				//printf("Wrote city to %s\n", filename);
			}

			json_decref(root);
		}

		current = current->next;
	}
}

int cities_create(Cities* _Cities, const char* _Name, const char* _Latitude, const char* _Longitude, City** _City) {
    if(_Cities == NULL || _Name == NULL)
		return -1;

	int result = 0;
	City* new_City = NULL;

	result = city_init(_Name, _Latitude, _Longitude, &new_City);
	if(result != 0)
	{
		printf("Failed to initialize City struct! Errorcode: %i\n", result);
		return -2;
	}
	
	LinkedList_Push(&_Cities->list, new_City);
	
	if(_City != NULL)
		*(_City) = new_City;

	return 0;
}

int cities_get_name(Cities* _Cities, const char* _Name, City** _CityPtr) {
	if(_Cities == NULL || _Name == NULL || _CityPtr == NULL)
		return -1;

	City* city = NULL;
	LinkedList_ForEach(&_Cities->list, &city)
	{
		if(strcmp(city->name, _Name) == 0)
		{
			if(_CityPtr != NULL)
				*(_CityPtr) = city;

			return 0;
		}
	}
	
	return -2;
}

void cities_print(Cities* _Cities) {
    if (_Cities == NULL) {
        return;
    }

    int index = 1;
    City* city = NULL;
    
    LinkedList_ForEach(&_Cities->list, &city) {
        printf("[%i] - %s\n", index++, city->name);
    }
}