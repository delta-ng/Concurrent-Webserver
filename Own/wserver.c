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
    
    while ((d = getopt(argc, argv, "d:p:t:")) != -1)
	switch (d) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		MAXBUF = atoi(optarg);
		break;
	default:
		// printf("%c",d);
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);
    int *t=malloc((MAXBUF+1)*sizeof(int));
    int *c=calloc(MAXBUF+1,sizeof(int));
    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
	pthread_t *worker_thread = malloc((MAXBUF+1)*sizeof(pthread_t));
    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	new_conn = malloc(1);
	int i;
	*new_conn = conn_fd;
	if(conn_fd!=-1) {
		// start = time(NULL);
		for(i=1;i<MAXBUF+1;i++) {
		printf("%d\n",i);
		if(*(t+i)!=0) {
			*(t+i)=pthread_create(worker_thread+i, NULL, request_handle, (void*) new_conn);
			*(c+i)=conn_fd;
			printf("%d %d\n",i,conn_fd);
			// printf("%d\n",*(c+i));
			break;
		}
		if(i==MAXBUF) {
				for(int j=1;j<MAXBUF+1;j++) {
					pthread_join(*(worker_thread+j), NULL);
					// if(*(c+j) != 0) {
					// 	// printf("%d\n",*(c+j));
					// 	close_or_die(*(c+j));
					// }
					*(t+j)=1;
				}
			}
	    }
		}
	}
    return 0;
}


    


 
