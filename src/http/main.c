#include "httpd.h"
#include "httpconst.h"
#include "httpresponse.h"
#include "authenticate.h"
#include <stdio.h>
#include <stdarg.h>
#include <strings.h>

user_credentials *user = NULL;

#ifndef _TESTING_
int main(int c, char** v)
{
    char *portNo = DEFAULT_PORT_NO;
    if (c>1) portNo=v[1];
    serve_forever(portNo);
    return 0;
}
#endif

int hasCredentials()
{
    int result = 0;
    // Already logged in?
    if (user && user->state>=AUTH_HAS_CREDENTIALS) return 1;
    char *token = request_header(HEADER_AUTHORIZATION);
    if (!token) 
    {
        UNAUTHORIZED(realm(),"Not authorized to access this function");
        auth_attempts++;
        keepalive=1;
    }
    else
    {
      user = credentials(token);
      if (user)
      {
          if (user->state=AUTH_HAS_CREDENTIALS) result=1;
      }
    }
    return result;
}

int isAuthenticated()
{
  int result = 0;
  if (!user) result = hasCredentials();
  if (result) 
  {
      // call authentication method;
      fprintf(stderr, "User <%s> has logged in\r\n",user->username);
      result = 1;
  }  
  return result;
}


void route()
{
    //Ensure the hostname is initalized for authentication
    auth_host = pico_hostname(); 
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

    ROUTE_GET("/secure")
    {
        fprintf(stderr, "In GET Secure\r\n");
        if (!isAuthenticated()) 
        {
            if (auth_attempts>2)
            {
                // message if you are unathenticated.
                char msg[1024] = "Not authorized to access function.";
                // message if you are authenticated, but not authorized.
                if (user) snprintf(msg, 1024, "Sorry %s, you do not have access to this function", user->username);
                FORBIDDEN(msg);
                keepalive=0;
            }
            return; 
        }
        else
        {
            char okmsg[1024];
            snprintf(okmsg,1024,"Welcome %s, nice to see you",user->username);
            OK(okmsg);
        }
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
