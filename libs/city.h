#ifndef CITY_H
#define CITY_H

/**
 * @brief Represents a city with a name and geographic coordinates.
 */
typedef struct city_t
{
	char* name;
	float latitude;
	float longitude;
} city_t;

int city_init(const char* _Name, const char* _Latitude, const char* _Longitude, city_t** _CityPtr);
void city_dispose(city_t** _cityPtr);

#endif