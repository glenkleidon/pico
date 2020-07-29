# Pico HTTP Server in C 

# SORRY THIS REPO IS TEMPORARILY ON HOLD 
## THERE IS AN ISSUE IN THE MAIN BRANCH AND IT WILL NOT COMPILE 

If you are just looking for a simple proof of concept, go to 
 [laobubu's original gitter](https://gist.github.com/laobubu/d6d0e9beb934b60b2e552c2d03e1409e)


This is a very simple HTTP server for Unix, written entirely in C.

This is a clone of the PICO project. It's very easy to use - simply add your routes into main.c

## How to use

1. include header `httpd.h`
2. write your route method, handling requests.
3. call `serve_forever("12913")` to start serving on port 12913

See `main.c`, an interesting example.

To log stuff, use `fprintf(stderr, "message");`

View `httpd.h` for more information

based on <http://blog.abhijeetr.com/2010/04/very-simple-http-server-writen-in-c.html>
