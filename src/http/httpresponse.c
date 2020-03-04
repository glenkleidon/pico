#include "httpresponse.h"
#include "httpconst.h"
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
    while(h->state==HAS_VALUE && i<MAX_RESPONSE_HEADERS) {
        if (strcmp(h->name, header) == 0) return h;
        h++;
        i++;
    }
    return (i>MAX_RESPONSE_HEADERS) ? NULL: h;
}

void init_response_headers()
{
    int i=0;
    header_r *h = reshdr;
    while(i<MAX_RESPONSE_HEADERS) {
        h->state=0;
        h++;
        i++;
    }
}

char *response_header(const char* header)
{
   header_r *h = find_response_header(header);
   return (h) ? h->value : NULL;
}

void reset_response_headers()
{
    release_response_header(&reshdr[0]);
    release_response_header(&reshdr[1]);
}

void release_response_header(header_r *responseheader)
{
    if (responseheader->state==HAS_VALUE)
    {
        if (responseheader->name) free(responseheader->name); responseheader->name=NULL;
        if (responseheader->format) free(responseheader->format); responseheader->format=NULL;
        if (responseheader->value) free(responseheader->value); responseheader->value=NULL;
        responseheader->state = 0;
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
      snprintf(lvalue, 2047, format, value);
      h->value =assign_string(h->value, lvalue);
      h->state = HAS_VALUE;
   }
}

/// Implementation of the send header macro
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
      h++;
  }
  printf("\r\n");
}

/// The function that actually outputs the content
/// determining if the data needs to be sent as a stream (chuncked)
/// or if it can be sent with a content length
void _internal_send_content(int response_code, const char *content, va_list content_args)
{
   
    int contentlength = 0;
    if (content) contentlength = strlen(content);
    
    const char *arg_content = va_arg(content_args, const char *);
    if (!arg_content) 
    {  // there is only a single arugment, so send content length and the data.
         if (contentlength>0)
         {
            char contentlengthstr[13];
            snprintf(contentlengthstr, 13, "%u\0", contentlength);
            add_response_header(HEADER_CONTENT_LENGTH, "%s", contentlengthstr);
            send_header(response_code);
            printf(content);
         }
    }
    else
    {
        // there are multiple arguments, so we will chunk the data.
        add_response_header(HEADER_TRANSFER_ENCODING,"%s",HEADER_TRANFSER_TYPE_CHUNKED); 
        send_header(response_code);
        if (content) printf("%x\r\n%s\r\n",contentlength, content);
        while (arg_content)
        {
           contentlength=strlen(arg_content); 
            if (contentlength>0) printf("%x\r\n%s\r\n", contentlength, arg_content);
            arg_content = va_arg(content_args, const char *);
        }
        printf("0\r\n");
    }
    printf("\r\n");
    reset_response_headers();
}

/// A successful request has been received and content will be returned
void _ok(const char *content, ...) 
{
   SENDCONTENT(200);
}

/// The requested resource cannot be found
void _notfound(const char *content, ...)
{
   SENDCONTENT(404); 
}
/// When the client sends no authentication header or the header
/// has been rejected, fewer times than the max limit)
void _notauthorized(const char *realm, const char *content, ...)
{
    // ensure there is a safe realm to apply
   char myRealm[1024]="PICOServer"; 
   int rl = 10;
   if (realm)
   {  
      rl=strlen(realm);
      strncpy(myRealm,realm,1023);
   }
   myRealm[rl]=0; 
   add_response_header(HEADER_CONNECTION, "%s", HEADER_VALUE_CONNECTION_KEEP_ALIVE);
   add_response_header(HEADER_WWW_AUTHENTICATE,"Basic realm=\"%s\"",myRealm);
   SENDCONTENT(401);
}

/// When authentication fails and the server refuses to accept any more attemps 
/// from the client.
void _forbidden(const char *content, ...)
{
    add_response_header(HEADER_CONNECTION,"%s", HEADER_VALUE_CONNECTION_CLOSE);
    SENDCONTENT(403);
}

void _send_content(int response_code, const char *content, ...)
{
  va_list args;
  va_start(args, content);
  _internal_send_content(response_code, content, args);
  va_end(args);
}
