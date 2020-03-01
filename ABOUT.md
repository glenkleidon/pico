# Pico HTTP Server in C 

This is a very simple HTTP server for Unix, entirely in C.

This is a clone of the PICO project. It's very easy to use

## How to use

1. include header `httpd.h`
2. write your route method, handling requests.
3. call `serve_forever("12913")` to start serving on port 12913

See `main.c`, an interesting example.

To log stuff, use `fprintf(stderr, "message");`

View `httpd.h` for more information

based on <http://blog.abhijeetr.com/2010/04/very-simple-http-server-writen-in-c.html>
