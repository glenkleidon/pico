#ifndef _HTTPD_RESPONSE___
#define _HTTPD_RESPONSE___

#include <stdlib.h>
#include <stdarg.h>

#define MAXARG 1000
#define OK(...) _ok(__VA_ARGS__, NULL)
#define NOTAUTHORIZED(...) _notauthorized(__VA_ARGS__, NULL,NULL);
#define UNAUTHORIZED(...) _notauthorized(__VA_ARGS__, NULL,NULL);
#define FORBIDDEN(...) _forbidden(__VA_ARGS__, NULL);

/// Send content Including a response code
#define SEND_CONTENT(...) _send_content(__VA_ARGS__, NULL);


// Set Content Header and output
#define SENDCONTENT(response_code) \
  va_list args;\
  va_start(args, content);\
  _internal_send_content(response_code, content, args);\
  va_end(args);
#define HAS_VALUE 43690 // eq to binar 10101010101010 - unlikely random value

typedef struct { char *name, *format, *value; int state; } header_r;
#define MAX_RESPONSE_HEADERS 49
static header_r reshdr[1+MAX_RESPONSE_HEADERS] = { {NULL, NULL,NULL} };

void _ok(const char *content, ...);
void _notfound(const char *content, ...);
void _notauthorized(const char *realm, const char *content, ...);
void _forbidden(const char *content, ...);
void _send_content(int response_code, const char *content, ...);
void _internal_send_content(int response_code, const char *content, va_list content_args);
void init_response_headers();

char* http_description(int response_code);
char* response_header(const char* header);
void add_response_header(const char *header, const char *format, const char *value);
void reset_response_headers();
void release_response_header(header_r *responseheader);
char* all_response_headers();
header_r *find_response_header(const char* header);



#endif /* HTTPD_RESPONSE__ */
