#include "city.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

int city_init(const char* _Name, const char* _Latitude, const char* _Longitude, City** _CityPtr) {
    if(_Name == NULL || _CityPtr == NULL)
		return -1;

	City* _City = (City*)malloc(sizeof(City));
	if(_City == NULL)
	{
		printf("Failed to allocate memory for new City\n");
		return -1;
	}

	memset(_City, 0, sizeof(City));

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

void city_dispose(City** _CityPtr) {
    if(_CityPtr == NULL || *(_CityPtr) == NULL)
		return;

	City* _City = *(_CityPtr);

	if(_City->name != NULL)
		free(_City->name);

	free(_City);
	*(_CityPtr) = NULL;
}