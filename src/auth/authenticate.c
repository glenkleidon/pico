#include "base64.h"
#include "authenticate.h"
#include "picoutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BASIC_TEXT "Basic"
char _auth_realm[1024] = "\0";

int basic(const char *token, user_credentials *user)
{
    user->error_code=1;
    char *c;
    if (token) c = strstr(token,BASIC_TEXT);
    if (c) 
    {
       // keep a copy of the token. 
       user->bearer=assign_string(user->bearer, token);

       // locate the b64
       while (*c != ' ' && c!='\0') c++;
       if (c) {
        c++; // skip over the space
        // decode the base64
        size_t sz = strlen(token); // larger than required;
        char *buffer = malloc(sz);
        size_t b64sz= (token+sz-c);
        buffer = base64_decode(c,b64sz,&sz);
        if (buffer)
        {
            // ensure b64 method terminates string
            *(buffer+sz+1) = '\0';
            // locate the :
            c = strstr(buffer,":");
            if (c)
            {
                // split the buffer into 2 strings using a null
                *c='\0';
                user->username = assign_string(user->username, buffer);
                user->password = assign_string(user->password, ++c);
                user->state=AUTH_HAS_CREDENTIALS;
                user->error_code=0;
            }
        }
        free(buffer);
       }
    } 
    return user->error_code;
}

user_credentials *credentials(const char * token)
{
   user_credentials *result = malloc(sizeof(user_credentials));
   result->bearer=NULL;
   result->password=NULL;
   result->username=NULL;
   result->state=AUTH_NOT_LOGGED_IN;
   int auth_result = basic(token, result);
   if (auth_result==0) return result;
   free(result);
   return NULL;
}

char *realm()
{
    if (!_auth_realm[0]) 
    {
       if (auth_realm)
       { 
         snprintf(_auth_realm,1023,"%s",auth_realm);
       }
       else if (auth_host)
       {
         snprintf(_auth_realm,1023,"pico@%s",auth_host);
       } else
       {
         strncpy(_auth_realm,"pico@",5);  
         strncpy(&_auth_realm[5],getenv("HOSTNAME"),999);  
       }
       auth_realm = &_auth_realm[0];
    }
    return auth_realm;
}


