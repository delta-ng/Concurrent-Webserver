#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
char default_root[] = "."; 
//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int d;
    int MAXBUF=8;
	int *new_conn;
    char *root_dir = default_root;
    int port = 10000;
    
    while ((d = getopt(argc, argv, "d:p:b:")) != -1)
	switch (d) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 'b':
		MAXBUF = atoi(optarg);
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);
    int t[MAXBUF];
    int c[MAXBUF];
    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
	pthread_t worker_thread[MAXBUF];
    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	new_conn = malloc(1);
	*new_conn = conn_fd;
	// start = time(NULL);
	for(int i=0;i<MAXBUF;i++) {
	if(t[i]) {
		t[i]=pthread_create(&worker_thread, NULL, request_handle, (void*) new_conn);
		c[i]=*new_conn;
		break;
	}
		if(i==MAXBUF-1) {
			for(int i=0;i<MAXBUF;i++) {
				pthread_join(worker_thread[i], NULL);
				close_or_die(c[i]);
			}
		}
    }
	}
    return 0;
}


    


 
