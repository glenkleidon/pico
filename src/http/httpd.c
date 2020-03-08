#include "httpd.h"
#include "httpconst.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#define CONNMAX 1000

static int listenfd, clients[CONNMAX];
static void error(char *);
static void startServer(const char *);
static void respond(int);

typedef struct { char *name, *value; } header_t;
#define MAX_REQUEST_HEADERS 49
static header_t reqhdr[MAX_REQUEST_HEADERS+1] = { {"\0", "\0"} };
static int clientfd;
static int keepalive;

static char *buf;
static char _pico_hostname[1024] = "\0";
static char *bufptr;
static int clientslot;
static int eob;

void serve_forever(const char *PORT)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    int slot=0;
    
    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",PORT,"\033[0m"
            );

    // Setting all elements to -1: signifies there is no client connected
    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    startServer(PORT);
    
    // Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);

    // ACCEPT connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients[slot]<0)
        {
            perror("accept() error");
        }
        else
        {
            if ( fork()==0 )
            {
                // I am now the client - close the listener: client doesnt need it
                keepalive=1;
                clientslot=slot;
                close(listenfd); 
                init_response_headers();
                respond(slot);
                exit(0);
            } else 
            {
                // I am still the server - close the accepted handle: server doesnt need it.
                close(clients[slot]);
            }
        }

        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}

//start server
void startServer(const char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family =  AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    // extract host name (if http name has been set)
    _pico_hostname[0] = '\0';
    int gai_result;
    if (gai_result=getaddrinfo(_pico_hostname,"http",&hints, &res)!=0)
    {
       fprintf(stderr, 
        "Note: cant find the FQDN (%s). Using local hostname instead.\r\n",gai_strerror(gai_result));
       // could not get it.  just use hostname
       strcpy(_pico_hostname, getenv("HOSTNAME"));
    }
    else
    {
       for(p=res; p!=NULL; p=p->ai_next) 
       {
         fprintf(stderr,"hostname: %s\r\n",p->ai_canonname);
       }      
       freeaddrinfo(res);
    }
    // get the Socket information for binding
    hints.ai_family=AF_INET;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        int option = 1;
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}


// get request header
char* request_header(const char* name)
{
    header_t *h = reqhdr;
    while(h->name) {
        if (strcmp(h->name, name) == 0) return h->value;
        h++;
    }
    return NULL;
}

void reset_headers()
{
    header_t *h = reqhdr;
    while(h->name) {
        h->name="\0";
        h++;
    }
}

void reset_request()
{
    eob=0;
    payload=NULL;
    bufptr=buf;
    reset_headers();
}

int get_bytes()
{
    // move the buffer point to the end of the last buffer
    bufptr=bufptr+eob; 
    
    // calculate the remaining buffer size
    int buffsize = buf+MAXBUFFER-bufptr;
    if (buffsize>65534) buffsize=65534;
    fprintf(stderr, "Slot: %u, buffsize: %u bytes\r\n",clientslot, buffsize);

    // read the bytes from the socket.
    int rcvd = recv(clients[clientslot], bufptr, buffsize, 0);

    // handle outcome of recv call.
    if (rcvd < 0) 
    { // receive error
        fprintf(stderr, "recv() error or session end\r\n");
        keepalive=0;
    }
    else if (rcvd == 0)
    {
        // receive socket closed
        fprintf(stderr, "Client disconnected upexpectedly.\r\n");
        keepalive=0;
    }
    else // message received
    {
        fprintf(stderr, "Received %u bytes buf=%u, bufptr=%u\r\n",rcvd, buf, bufptr);
        // terminate the incoming buffer;
        bufptr[rcvd] = '\0';
    }
    // update the new end of buffer 
    eob = eob+rcvd;

    // return the number of bytes recieved on this occaion.
    return rcvd;
}

//client connection
void respond(int n)
{
    int rcvd, fd, bytes_read;
    char *ptr;

    buf = malloc(MAXBUFFER);

    // header pointers.
    header_t *h = reqhdr;
    char *header_name, *header_value;
    char *t, *t2;
    char *eohptr=NULL;
    reset_request();

    while (keepalive)
    {
                
        rcvd=get_bytes();
        
        if (rcvd>0)
        {
            //wait for the headers to be completely loaded.
            if (!eohptr)
            {
                eohptr=strstr(buf, "\r\n\r\n");
                // if no dobule crlf is found, go back for more bytes.
                if (!eohptr) 
                {
                    // TODO: if the buffer is full and still no header, then return a 413 Entity too large.
                    continue;
                }
                // double crlf is found, reset bufptr back to the beginning. 
                // note: EOB will still be the last by byte received.
                bufptr=buf;
            }  
            int protcol_rcvd=0; 
            int headers_rcvd=0;

            // extract the protocol, URL and 
            if (!protcol_rcvd)
            {
                method = strtok(bufptr, " \t\r\n");
                uri = strtok(NULL, " \t");
                prot = strtok(NULL, " \t\r\n");

                if (method==NULL || uri == NULL || prot==NULL)
                {
                    // send bad request 400 
                    keepalive=0;
                    break;
                } 

                protcol_rcvd=1;
                fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);


                if (querystring = strchr(uri, '?'))
                {
                    *querystring++ = '\0'; //split URI
                }
                else
                {
                    querystring = uri - 1; //use an empty string
                }
            }

            if (!headers_rcvd && protcol_rcvd) 
            {
                header_t *h = reqhdr;
                while (h < reqhdr + MAX_REQUEST_HEADERS)
                {
                    // is there a new header.
                    header_name = strtok(NULL, "\r\n: \t");
                    if (!header_name)
                        break;
                    
                    header_value = strtok(NULL, "\r\n");
                    while (*header_value && *header_value == ' ')
                        header_value++;
                    h->name = header_name;
                    h->value = header_value;
                    h++;
                    fprintf(stderr, "[H] %s: %s\n", header_name, header_value);
                    t = header_value + 1 + strlen(header_value);
                    if (t[1] == '\r' && t[2] == '\n')
                    {
                        headers_rcvd=1;
                        break;
                    }
                }
                if (headers_rcvd)
                {
                    t++;                                        // now the *t shall be the beginning of user payload
                    t2 = request_header(HEADER_CONTENT_LENGTH); // and the related header if there is
                    payload = t;
                    payload_size = t2 ? atol(t2) : (eob - (t - buf));
                    if (t2)
                    {
                        fprintf(stderr, "Expecting %s bytes\r\n", t2);
                        fprintf(stderr, "%u Bytes Received\r\n", payload_size);
                    }
                }
            }
            if (payload)
            { 
                // bind clientfd to stdout, making it easier to write
                clientfd = clients[n];
                dup2(clientfd, STDOUT_FILENO);
                close(clientfd);

                // call router
                route();
                reset_request();
            }
        }
        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    //Closing SOCKET
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clientfd);
    clients[n]=-1;
}

char* pico_hostname()
{
    return &_pico_hostname[0];
}