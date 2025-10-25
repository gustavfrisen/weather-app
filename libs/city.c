#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "city.h"
#include "utils.h"

int city_init(const char* _Name, const char* _Latitude, const char* _Longitude, city_t** _CityPtr) {
    if(_Name == NULL || _CityPtr == NULL)
		return -1;

	city_t* _City = (city_t*)malloc(sizeof(city_t));
	if(_City == NULL)
	{
		printf("Failed to allocate memory for new City\n");
		return -1;
	}

	memset(_City, 0, sizeof(city_t));

	_City->name = strdup(_Name);
	if(_City->name == NULL)
	{
		printf("Failed to allocate memory for City name\n");
		free(_City);
		return -1;
	}

	if(_Latitude != NULL)
		_City->latitude = atof(_Latitude);
	else
		_City->latitude = 0.0f;
	
	if(_Longitude != NULL)
		_City->longitude = atof(_Longitude);
	else
		_City->longitude = 0.0f;

	*(_CityPtr) = _City;

	return 0;
}

void city_dispose(city_t **_cityPtr)
{
	if(_cityPtr == NULL || *_cityPtr == NULL)
		return;

	city_t* _City = *_cityPtr;

	if(_City->name != NULL)
		free(_City->name);

	free(_City);
	*_cityPtr = NULL;
}
