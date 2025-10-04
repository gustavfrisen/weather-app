#include "linkedlist.h"
#include "city.h"

typedef struct Cities Cities;

typedef struct Cities {
    LinkedList list;
} Cities;

int cities_init(Cities** _CitiesPtr);
void cities_add_from_cities_folder(Cities* _cities);
void cities_add_from_string_list(Cities* _cities, const char* _StringList);
void cities_save_to_cities_folder(Cities* _cities);
int cities_create(Cities* _Cities, const char* _Name, const char* _Latitude, const char* _Longitude, City** _City);
int cities_get_name(Cities* _Cities, const char* _Name, City** _CityPtr);
int cities_get_index(Cities* _Cities, int _Index, City** _CityPtr);
void cities_destroy(Cities* _Cities, City** _CityPtr);

void cities_print(Cities* _Cities);

void cities_dispose(Cities** _CitiesPtr);