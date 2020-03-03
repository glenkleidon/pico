#include "httpd.h"
#include <stdio.h>
#include <stdarg.h>
#include "httpresponse.h"

#ifndef _TESTING_
int main(int c, char** v)
{
    char *portNo = DEFAULT_PORT_NO;
    if (c>1) portNo=v[1];
    serve_forever(portNo);
    return 0;
}
#endif

void route()
{
    ROUTE_START()
    fprintf(stderr, "starting route\r\n");

    ROUTE_GET("/")
    {
        fprintf(stderr, "In GET\r\n");
        OK("Hello! You are using ", request_header("User-Agent"));
    }
    ROUTE_GET("/chunkme")
    {
        OK("chunked line 1","chunked line 2","chunked line 3\r\n and 4 in 1 chunk");
    }

    ROUTE_POST("/")
    {
        fprintf(stderr, "In Post handling %u\r\n",payload_size);
        char size[50];
        sprintf(size,"%u",payload_size);
        fprintf(stderr, "Received %s bytes\r\n",size);
        OK("Wow, seems that you POSTed ",size," bytes. \r\n", 
         "Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}
