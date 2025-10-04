typedef struct City City;

typedef struct City {
    char* name;
    float latitude;
    float longitude;
} City;

int city_init(const char* _Name, const char* _Latitude, const char* _Longitude, City** _CityPtr);
void city_dispose(City** _CityPtr);