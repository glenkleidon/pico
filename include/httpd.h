#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string.h>
#include <stdio.h>

//Server control functions
#define DEFAULT_PORT_NO "12913"
#define MAXBUFFER 131070

void serve_forever(const char *PORT);

// Client request

char    *method,    // "GET" or "POST"
        *uri,       // "/index.html" things before '?'
        *querystring,        // "a=1&b=2"     things after  '?'
        *prot;      // "HTTP/1.1"

char    *payload;     // for POST
int      payload_size;

char* request_header(const char* name);
char* pico_hostname();
int get_bytes();
void reset_request();
void reset_headers();

// user shall implement this function

void route();


// some usefule macros for `route()`
#define ROUTE_START()       if (0) {
#define ROUTE(METHOD,URI)   } else if (strcmp(URI,uri)==0&&strcmp(METHOD,method)==0) {
#define ROUTE_GET(URI)      ROUTE("GET", URI) 
#define ROUTE_POST(URI)     ROUTE("POST", URI) 
#define ROUTE_END()         } else printf(\
                                "HTTP/1.1 404 Not Found\r\n\r\n" \
                                "The requested resource cannot be found.\r\n" \
                            );

#endif
