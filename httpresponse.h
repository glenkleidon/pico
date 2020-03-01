#ifndef _HTTPD_RESPONSE___
#define _HTTPD_AUTH___

#define MAXARG 1000
#define ok(...) _ok(__VA_ARGS__, NULL)
#define OUTPUT_CONTENT va_list ap; \
   va_start(ap, content);\
   while (content) \
   { \
     printf(content);  \
     content = va_arg(ap, const char*);\
   }\
   va_end(ap)

typedef struct { char *name, *format, *value; } header_r;
#define MAX_RESPONSE_HEADERS 49
static header_r reshdr[1+MAX_RESPONSE_HEADERS] = { {"\0", "\0","\0"} };

#define MAX_RESPONSE_CODE 62
typedef struct { int code; char *value; } http_response_code;
static http_response_code HTTP_RESPONSE_CODES[MAX_RESPONSE_CODE+1] = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{102, "Processing"},
	{103, "Early Hints"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{306, "Switch Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}
};

void _ok(const char *content, ...) ;
void _notfound(const char *content, ...);
void _notauthorized(const char *content, ...);
char* http_description(int response_code);
char* response_header(const char* header);
void add_response_header(const char *header, const char *format, const char *value);
void release_response_header(header_r *responseheader);
char* all_response_headers();
header_r *find_response_header(const char* header);

char *assign_string(char *source, const char *target);

#endif /* HTTPD_RESPONSE__ */
