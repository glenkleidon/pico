#define _TESTING_

#include "../include/httpresponse.h"
#include <stdlib.h>
#include <stdio.h>

int main(int c, char** v)
{
    OK("this is my response");

    printf("\r\n");

    OK("this is chunk 1\r\n",
       "and the second line of the chunked response",
       "and this is some more chunked responses\r\nwith some more stuff\r\n");

    UNAUTHORIZED("MyRealm","not authorized to access this function");

    UNAUTHORIZED(NULL);

}