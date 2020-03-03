#ifndef _HTTPD_RESPONSE___
#define _HTTPD_AUTH___

#include <stdlib.h>

#define MAXARG 1000
#define OK(...) _ok(__VA_ARGS__, NULL)
#define NOTAUTHORIZED(...) _notauthorized(__VA_ARGS__, NULL);
#define UNAUTHORIZED(...) _notauthorized(__VA_ARGS__, NULL);


// Set Content Header and output
#define SENDCONTENT(response_code) \
   int _HC_argCount=0; int _HC_isChunked=0; int _HC_contentlength=0; \
   const char *_HC_args = content; \ 
   va_list _HC_ap;  va_start(_HC_ap, content); \
   _HC_contentlength=strlen(content); \
   while(content && _HC_argCount++<2) content = va_arg(_HC_ap, const char*); \
   va_end(_HC_ap); \
   if (_HC_argCount<2) \ 
   { \
      char _HC_contentlengthstr[13]; \
      snprintf(_HC_contentlengthstr,13,"%u",_HC_contentlength); \
      add_response_header(HEADER_CONTENT_LENGTH,"%s",_HC_contentlengthstr); \
   } \
   else  \
   { \
      _HC_isChunked=1; \
      add_response_header(HEADER_TRANSFER_ENCODING,"%s",HEADER_TRANFSER_TYPE_CHUNKED); \
   };\
   send_header(response_code);\
   content = _HC_args; \
   va_start(_HC_ap, content);\
   while (content) \
   { \
     if (_HC_isChunked) \
     { \
         printf("%x\r\n%s\r\n",strlen(content),content);\
     } \
     else \
     { \
         printf(content);   \
     } \
     content = va_arg(_HC_ap, const char*); \
   }\
   va_end(_HC_ap); \
   if (_HC_isChunked) printf("0\r\n"); \
   printf("\r\n"); \
   reset_response_headers(); 


typedef struct { char *name, *format, *value; } header_r;
#define MAX_RESPONSE_HEADERS 49
static header_r reshdr[1+MAX_RESPONSE_HEADERS] = { {NULL, NULL,NULL} };

void _ok(const char *content, ...);
void _notfound(const char *content, ...);
void _notauthorized(const char *content, ...);
char* http_description(int response_code);
char* response_header(const char* header);
void add_response_header(const char *header, const char *format, const char *value);
void reset_response_headers();
void release_response_header(header_r *responseheader);
char* all_response_headers();
header_r *find_response_header(const char* header);

#endif /* HTTPD_RESPONSE__ */
