#include <curl/curl.h>

int http_init();
char* http_fetch(char* url);
int http_cleanup();