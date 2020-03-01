#include "httpresponse.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define pass_message printf("%s%s%s",ANSI_COLOR_GREEN,"PASS: ",ANSI_COLOR_RESET);
#define fail_message printf("%s%s%s",ANSI_COLOR_RED,"FAIL: ",ANSI_COLOR_RESET);


char *testname;
void new_test(char *new_test_name)
{
    testname = new_test_name;
}
void new_set(char * new_set_name)
{
   printf("\r\n%s",new_set_name);
}

void check_int(int expected, int actual, char *message)
{
   int result=1;
   char *msg = "OK";
   if (expected!=actual) 
   {
      result=0;
      msg=message; 
   } 
   if (!msg) 
   { 
        printf(" Expected %u, actual %u",result, expected, actual);
   }else 
   {
       printf("%s-%s",result,testname,msg);
   }
}
void check_str(char *expected, char *actual, char *message)
{
   int result=1;
   char *msg = "OK";
   if (expected==NULL && actual==NULL)
   {
       // this is ok - still a pass;
   } else if ( (expected==NULL && actual!=NULL) || (expected!=NULL && actual==NULL))
   {
      result=0;
      msg=message; 
   } else if (strncmp(expected,actual, strlen(expected)))
   {
      result=0;
      msg=message; 
   } 
   if (result) 
   {
      pass_message; 
   } else
   {
      fail_message;
   }
   if (!msg) 
   { 
      printf("%s\r\nExpected <%s>\r\nactual   <%s>\r\n", testname, expected, actual);
   }else 
   {
      printf("%s-%s\r\n",testname,msg);
   }
}

void test_http_codes(char *set_name)
{
   new_test(set_name);

    new_test("404 Not found");
    check_str("Not Found",http_description(404),NULL);

    new_test("426 Upgrade Required");
    check_str("Upgrade Required",http_description(426),NULL);

    new_test("200 OK");;
    check_str("OK",http_description(200),NULL);
    
    new_test("511 Network Authentication Required");
    check_str("Network Authentication Required",http_description(511),NULL);
    
    new_test("100 Continue");
    check_str("Continue",http_description(100),NULL);

}

void test_response_headers()
{
    new_test("unknown Headers");
    header_r *h = find_response_header("not a real header");
    check_str("\0",h->name,NULL);
   
    new_test("Add a header");
    add_response_header("Content-Encoding","%s","text/xml");
    h = find_response_header("Content-Encoding");
    check_str("text/xml",h->value,NULL);

    new_test("Add a second header, both still present");
    add_response_header("Content-Length","%s","205");
    h = find_response_header("Content-Encoding");
    check_str("text/xml",h->value,NULL);
    h = find_response_header("Content-Length");
    check_str("205",h->value,NULL);

    new_test("after Adding a second header, unknown header is not present");
    h = find_response_header("Content-Lengfth");
    check_str(NULL,h->value,NULL);

}

int main(int c, char** v)
{
    test_http_codes("HTTP Response Codes");
   
    test_response_headers("Assigning response Headers");

    
    return 0;
}