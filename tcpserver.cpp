/* 
 * tcpserver.c - A simple TCP echo server 
 * usage: tcpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/var.h"

pp::Instance* g_instance = NULL;

void nacl_log( const char* level, const char* format, ...)
{
    if( !g_instance )
        return;
    
    va_list args;
    char buffer[1024*32];
    va_start(args, format);
    vsnprintf(buffer,1024*32,format,args);
    va_end(args);

    g_instance->PostMessage(pp::Var(buffer));
}


#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(const char *msg) {
    //perror(msg);
    nacl_log("error",msg);
    exit(1);
}

int start_tcp_server( pp::Instance* instance, int port )
{
    g_instance = instance;
    
    //int argc, char **argv) {
    int parentfd; /* parent socket */
    int childfd; /* child socket */
    int portno = port; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    char buf[BUFSIZE]; /* message buffer */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */

    nacl_log("info","tcpserver listening on port %d",portno);
  
    /* 
     * socket: create the parent socket 
     */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) 
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
     * us rerun the server immediately after we kill it; 
     * otherwise we have to wait about 20 secs. 
     * Eliminates "ERROR on binding: Address already in use" error. 
     */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
               (const void *)&optval , sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short)portno);

    /* 
     * bind: associate the parent socket with a port 
     */
    if (bind(parentfd, (struct sockaddr *) &serveraddr, 
             sizeof(serveraddr)) < 0) 
        error("ERROR on binding");

    /* 
     * listen: make this socket ready to accept connection requests 
     */
    if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */ 
        error("ERROR on listen");

    /* 
     * main loop: wait for a connection request, echo input line, 
     * then close connection.
     */
    clientlen = sizeof(clientaddr);
    
    while (1) {

        /* 
         * accept: wait for a connection request 
         */
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        nacl_log("debug","%s:%s:%d childfd: %d",__func__,__FILE__,__LINE__,childfd);

        if (childfd < 0)
        {
            nacl_log("error","ERROR on accept");
            usleep(1e5);
            continue;
        }

        bzero(buf, BUFSIZE);
        n = read(childfd, buf, BUFSIZE);
        if (n < 0) 
            error("ERROR reading from socket");
        nacl_log("info","server received %d bytes: %s", n, buf);

        n = write(childfd, buf, strlen(buf));
        if (n < 0) 
            error("ERROR writing to socket");
        
        close(childfd);
        
    }
}
