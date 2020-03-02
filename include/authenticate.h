#ifndef _HTTPD_AUTH___
#define _HTTPD_AUTH___

typedef struct 
{
    int step;
    char *username;
    char *password;
    char *bearer;
    int error_code;
} user_credentials;

user_credentials* credentials(const char * token);

user_credentials* require_credentials(char * realm);

#endif /* HTTPD_AUTH__ */


