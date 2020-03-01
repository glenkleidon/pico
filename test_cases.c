#include "httpresponse.h"
#include "testmethods.h"
#include "base64.h"
#include "authenticate.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>


void test_http_codes(char *set_name)
{
   new_set(set_name);

    new_test("404 Not found");
    check_str("Not Found",http_description(404),DEFAULT_MESSAGE);

    new_test("426 Upgrade Required");
    check_str("Upgrade Required",http_description(426),DEFAULT_MESSAGE);

    new_test("200 OK");;
    check_str("OK",http_description(200),DEFAULT_MESSAGE);
    
    new_test("511 Network Authentication Required");
    check_str("Network Authentication Required",http_description(511),DEFAULT_MESSAGE);
    
    new_test("100 Continue");
    check_str("Continue",http_description(100),DEFAULT_MESSAGE);

}

void test_response_headers(char *set_name)
{
    new_set(set_name);
    new_test("unknown Headers");
    header_r *h = find_response_header("not a real header");
    check_str("\0",h->name,DEFAULT_MESSAGE);
   
    new_test("Add a header");
    add_response_header("Content-Encoding","%s","text/xml");
    h = find_response_header("Content-Encoding");
    check_str("text/xml",h->value,DEFAULT_MESSAGE);

    new_test("Add a second header, both still present");
    add_response_header("Content-Length","%s","205");
    h = find_response_header("Content-Encoding");
    check_str("text/xml",h->value,DEFAULT_MESSAGE);
    h = find_response_header("Content-Length");
    check_str("205",h->value,DEFAULT_MESSAGE);

    new_test("after Adding a second header, unknown header is not present");
    h = find_response_header("Content-Lengfth");
    check_str(NULL,h->value,DEFAULT_MESSAGE);

}

void test_base64_decoding(char *set_name)
{
    new_set(set_name);
    new_test("Test string decodes to a known value");
    char *test_data = "VGhpcyBpcyBhIHRlc3QK";
    size_t sz=1024;
    char * decoded_text = malloc(sz);
    decoded_text = base64_decode(test_data, strlen(test_data), &sz);
    check_str("This is a test",decoded_text, DEFAULT_MESSAGE);

    new_test("Test string encodes into known value");
    test_data="This is plain text";
    sz=1024;
    char *encoded_text = malloc(sz);
    encoded_text = base64_encode(test_data, strlen(test_data), &sz);
    check_str("VGhpcyBpcyBwbGFpbiB0ZXh0",encoded_text, DEFAULT_MESSAGE);

    new_test("Encoded data returns to original data");
    test_data=base64_decode(encoded_text,strlen(encoded_text),&sz);
    check_str("This is plain text", test_data,DEFAULT_MESSAGE);
    free(decoded_text);
    free(encoded_text);
}

void test_auth_decoding(char *set_name)
{
    new_set(set_name);
    new_test("Basic Auth is decoded");
    char * test_data = "Authorization: Basic QWxhZGRpbjpPcGVuU2VzYW1l";

}

int main(int c, char** v)
{
    test_http_codes("HTTP Response Codes");
   
    test_response_headers("Assigning response Headers");

    test_base64_decoding("Base 64 encoding and decoding"); 

    test_auth_decoding("Base64 encoded Bearer converts to username/password");

    end_run();

    return 0;
}
