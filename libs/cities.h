#ifndef CITIES_H
#define CITIES_H

#include "linked_list.h"
#include "city.h"
typedef struct cities_t
{
	LinkedList list;
	
} cities_t;

int cities_init(cities_t** cities_ptr);
int cities_add_city(cities_t* cities, city_t* city);
int cities_remove_city(cities_t* cities, const char* name);

int cities_get_city_by_name(cities_t* cities, const char* name, city_t** city_ptr);
int cities_get_city_by_index(cities_t* cities, int index, city_t** city_ptr);

int cities_sort_by_name(cities_t* cities);
int cities_sort_by_distance(cities_t *cities, const char *city_name);

int cities_print(cities_t* cities);
int cities_print_pretty(cities_t* cities);

int cities_dispose(cities_t** cities_ptr);

#endif