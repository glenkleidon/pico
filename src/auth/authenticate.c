#include "base64.h"
#include "authenticate.h"
#include "picoutils.h"
#include <stdlib.h>
#include <string.h>


#define BASIC_TEXT "Basic"

char * active_realm = "pico@";

int basic(const char *token, user_credentials *user)
{
    user->error_code=1;
    user->step=0;
    char *c = strstr(token,BASIC_TEXT);
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
                user->step=1;
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
   int auth_result = basic(token, result);
   if (auth_result==0) return result;
   free(result);
   return NULL;
}

user_credentials *require_credentials(char * realm)
{
  
}

