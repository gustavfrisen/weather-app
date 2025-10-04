#include <curl/curl.h>

int http_init();
char* http_fetch(double latitude, double longitude);
int http_cleanup();