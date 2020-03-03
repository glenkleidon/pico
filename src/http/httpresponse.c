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

void reset_response_headers()
{
    release_response_header(reshdr);
}

void release_response_header(header_r *responseheader)
{
    if (responseheader && (strlen(responseheader->name)>0 || strlen(responseheader->value)>0))
    {
        free(responseheader->name); responseheader->name=NULL;
        free(responseheader->format); responseheader->format=NULL;
        free(responseheader->value); responseheader->value=NULL;
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
      h++;
  }
  printf("\r\n");
}



void _ok(const char *content, ...) 
{
   SENDCONTENT(200);
}

void _notfound(const char *content, ...)
{
   SENDCONTENT(404); 
}

void _notauthorized(const char *realm, const char *content, ...)
{
    // ensure there is a safe realm to apply
   char myRealm[1024]="PICOServer"; 
   int rl = 0;
   if (realm)
   {  rl=strlen(realm);
      strncpy(myRealm,realm,1023);
   }
   myRealm[rl]=0; 
   add_response_header(HEADER_WWW_AUTHENTICATE,"Basic realm=\"%s\"",myRealm);
   int _HC_argCount=0; int _HC_isChunked=0; int _HC_contentlength=0;
   const char *_HC_args = content, HC_contentlengthstr=NULL; va_list _HC_ap; 
   if (content) { 
    va_start(_HC_ap, content); 
   _HC_contentlength=strlen(content); 
   while(content && _HC_argCount++<2) content = va_arg(_HC_ap, const char*); 
   va_end(_HC_ap);
   } 
   if (_HC_argCount<2)  
   { 
       if (HC_contentlengthstr) {
        char _HC_contentlengthstr[13]; 
        snprintf(_HC_contentlengthstr,13,"%u",_HC_contentlength); 
        add_response_header(HEADER_CONTENT_LENGTH,"%s",_HC_contentlengthstr); 
       }
   } 
   else  
   { 
      _HC_isChunked=1; 
      add_response_header(HEADER_TRANSFER_ENCODING,"%s",HEADER_TRANFSER_TYPE_CHUNKED); 
   };
   send_header(401);
   content = _HC_args; 
   va_start(_HC_ap, content);
   while (content) 
   { 
     if (_HC_isChunked) 
     { 
         printf("%x\r\n%s\r\n",strlen(content),content);
     } 
     else 
     { 
         printf(content);   
     } 
     content = va_arg(_HC_ap, const char*); 
   }
   va_end(_HC_ap); 
   if (_HC_isChunked) printf("0\r\n"); 
   printf("\r\n"); 
   reset_response_headers(); 

  // SENDCONTENT(401);
}

void _forbidden(const char *content, ...)
{
    SENDCONTENT(403);
}
