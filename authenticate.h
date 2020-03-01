#ifndef _HTTPD_AUTH___
#define _HTTPD_AUTH___

typedef struct authenticate_user
{
    /* data */
    int step;
    char *username;
    char *password;
    char *bearer;
    int error_code;
};

authenticate_user credentials();

authenticate_user require_credentials(char * realm);

#endif /* HTTPD_AUTH__ */


