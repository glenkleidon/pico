#ifndef _HTTPD_AUTH___
#define _HTTPD_AUTH___

#define AUTH_LOGGED_IN 99
#define AUTH_NOT_LOGGED_IN 0
#define AUTH_CLIENT_REQUEST 1
#define AUTH_HAS_CREDENTIALS 10

typedef struct 
{
    int state;
    char *username;
    char *password;
    char *bearer;
    int error_code;
} user_credentials;

char *auth_realm;
char *auth_host;


user_credentials* credentials(const char * token);
char *realm();

#endif /* HTTPD_AUTH__ */


