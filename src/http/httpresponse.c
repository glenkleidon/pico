#include "httpresponse.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "picoutils.h"

char * http_description(int response_code)
{
    http_response_code *h = HTTP_RESPONSE_CODES;
    while(h->code) {
        if (h->code==response_code) return h->value;
        h++;
    }
    return NULL;
}
/// locate the indicated header or return a pointer to the next available.
header_r *find_response_header(const char* header)
{
    int i=0;
    header_r *h = reshdr;
    while(h->name && strlen(h->name)>0 && i<MAX_RESPONSE_HEADERS) {
        if (strcmp(h->name, header) == 0) return h;
        h++;
        i++;
    }
    return (i>MAX_RESPONSE_HEADERS) ? NULL: h;
}

char *response_header(const char* header)
{
   header_r *h = find_response_header(header);
   return (h) ? h->value : NULL;
}

void release_response_header(header_r *responseheader)
{
    if (responseheader && (strlen(responseheader->name)>0 || strlen(responseheader->value)>0))
    {
        free(responseheader->name);
        free(responseheader->format);
        free(responseheader->value);
    }
}

void add_response_header(const char *header, const char *format, const char *value)
{
   header_r *h = find_response_header(header);
   if (!h)
   {    
       fprintf(stderr, "Response headers full - %s could not be added",header);
   } else
   {
      // if this header has been used before, release memory
      //release_response_header(h);
      // the name and format are fixed length, so just allocate the right amount of space
      h->name = assign_string(h->name, header);
      h->format = assign_string(h->format, format);
      
      // length of the value is a bit ambigous here, so assign to a temporary before copying it in.
      char lvalue[2048];
      snprintf(lvalue, 2048, format, value);
      h->value =assign_string(h->value, lvalue);
   }
}

//
void send_header(int code)
{
  char *description = http_description(code);
  printf("HTTP/1.1 %u %s\r\n",code, description);
  int i=0;
  header_r *h = reshdr;
  while( (h->value || h->name) && i<MAX_RESPONSE_HEADERS) {
      if (h->name && h->value)
      {
        printf("%s: %s\r\n", h->name, h->value);
      }
  }
  printf("\r\n");
}

void _ok(const char *content, ...) 
{
   send_header(200);
   OUTPUT_CONTENT;
}

void _notfound(const char *content, ...)
{
   send_header(404); 
   OUTPUT_CONTENT;
}

void _notauthorized(const char *content, ...)
{
   add_response_header("WWW-Authenticate","Basic realm='\"%s\"","my realm");
   send_header(401);
   OUTPUT_CONTENT;
}
