#include "httpd.h"
#include <stdio.h>
#include <stdarg.h>
#include "httpresponse.h"


int main(int c, char** v)
{
    serve_forever("12913");
    return 0;
}

void route()
{
    ROUTE_START()
    fprintf(stderr, "starting route\r\n");

    ROUTE_GET("/")
    {
        fprintf(stderr, "In GET\r\n");
        ok("Hello! You are using ", request_header("User-Agent"));
    }

    ROUTE_POST("/")
    {
        fprintf(stderr, "In Post handling %u\r\n",payload_size);
        char size[50];
        sprintf(size,"%u",payload_size);
        fprintf(stderr, "Received %s bytes\r\n",size);
        ok("Wow, seems that you POSTed ",size," bytes. \r\n", 
         "Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}
