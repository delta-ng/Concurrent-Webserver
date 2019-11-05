// #include<stdio.h>
// #include<stdlib.h>
//
// client.c: A very, very primitive HTTP client.
// 
// To run, try: 
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//8
// For testing your server, you will want to modify this client.  
// For example:
// You may want to make this multi-threaded so that you can 
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs; 
// you may want to get more URIs from the command line 
// or read the list from a file. 
//
// When we test your server, we will be using modifications to this client.
//

#include "io_helper.h"
#include<pthread.h>
#define MAXBUF (8192)

//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
	printf("Header: %s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
	
	// If you want to look for certain HTTP tags... 
	// int length = 0;
	//if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
	//    printf("Length = %d\n", length);
	//}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
	printf("%s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
    }
}

struct threadd{
    int clientfd;
    char *filename;
};
void *dos(struct threadd *new){
    // client_send(new->clientfd, new->filename);
    // client_print(new->clientfd);
    return NULL;
}


int main(int argc, char *argv[]) {
    char *host;
    int port, no_of_requests;
    int clientfd;
    
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <host> <port> <no_of_requests> <filenames>\n", argv[0]);
	exit(1);
    }
    
    host = argv[1];
    port = atoi(argv[2]);
    no_of_requests = atoi(argv[3]);
    char *filenames[no_of_requests];
    
    for(int i = 0; i<no_of_requests; i++){
        filenames[i]=argv[4+i];
    }
    // for(int i = 0; i<no_of_requests; i++){
    //     printf("%s\t%s\n",argv[4+i],filenames[i]);
    // }
    // return 0;
    /* Open a single connection to the specified host and port */
    // st = (pthread_t*)malloc(no_of_requests*sizeof(pthread_t))
    for (int i = 0; i < no_of_requests; i++)
    {
        /* code */
        // struct threadd *new = malloc(sizeof(struct threadd));
        clientfd = open_client_fd_or_die(host, port);

        // new->clientfd = clientfd;
        // new->filename = malloc(sizeof(filenames[i]));
        // new->filename = filenames[i];
        client_send(clientfd, filenames[i]);
        client_print(clientfd);
        // pthread_create(st+i , NULL, dos , new);
        // sleep(10);
        close_or_die(clientfd);
    }

    // for (int i = 0; i<no_of_requests;i++){
    //     pthread_join(*(st+i),NULL);
    // }
    exit(0);
}

