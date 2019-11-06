#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "thread_pool.h"
#define MAXBUF (8192)
char default_root[] = "."; 
// #define SIZE 3
// void *routine(void *arg) {
//   int arg_v = *(int *) arg;
//   int sleep_time = rand() % 2 + 4;
//   printf("Working on %d. Sleeping for %d seconds.\n", arg_v, sleep_time);
//   sleep(sleep_time);
//   printf("Work done (%d).\n", arg_v);
//   return NULL;
// }
// int items[SIZE];
// int front = -1, rear =-1;
// int isFull()
// {
//     if( (front == rear + 1) || (front == 0 && rear == SIZE-1)) return 1;
//     return 0;
// }
// int isEmpty()
// {
//     if(front == -1) return 1;
//     return 0;
// }
// int enQueue(int element)
// {
//     if(isFull()) {printf("\n Queue is full!! \n"); return 0;}
//     else
//     {
//         if(front == -1) front = 0;
//         rear = (rear + 1) % SIZE;
//         items[rear] = element;
//         printf("\n Inserted -> %d", element);
//         return 1;
//     }
// }
// int deQueue()
// {
//     int element;
//     if(isEmpty()) {
//         printf("\n Queue is empty !! \n");
//         return(-1);
//     } else {
//         element = items[front];
//         if (front == rear){
//             front = -1;
//             rear = -1;
//         } /* Q has only one element, so we reset the queue after dequeing it. ? */
//         else {
//             front = (front + 1) % SIZE;
            
//         }
//         printf("\n Deleted element -> %d \n", element);
//         return(element);
//     }
// }
// void display()
// {
//     int i;
//     if(isEmpty()) printf(" \n Empty Queue\n");
//     else
//     {
//         printf("\n Front -> %d ",front);
//         printf("\n Items -> ");
//         for( i = front; i!=rear; i=(i+1)%SIZE) {
//             printf("%d ",items[i]);
//         }
//         printf("%d ",items[i]);
//         printf("\n Rear -> %d \n",rear);
//     }
// }
struct info {
    int is_static;
    int size;
    int fd;
    char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];
};
int main(int argc, char *argv[]) {
    int d;
    int BUF=8;
    int MAXTH=4;
	int *new_conn;
    char *root_dir = default_root;
    int port = 8888;
    
    while ((d = getopt(argc, argv, "d:p:t:b:")) != -1) {
	switch (d) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		MAXTH = atoi(optarg);
		break;
	case 'b':
		BUF = atoi(optarg);
		break;
	default:
		// printf("%c",d);
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffer]\n");
	    exit(1);
	}
	}

	srand(time(NULL));
	// printf("%d %d\n",MAXTH,MAXBUF);
    thread_pool *pool = pool_init(MAXTH,BUF);
    printf("Testing threadpool of %d threads.\n", get_max_threads(pool));
    // run out of this directory
    chdir_or_die(root_dir);
    // int *t=malloc((MAXBUF+1)*sizeof(int));
    // int *c=calloc(MAXBUF+1,sizeof(int));
    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
	// pthread_t *worker_thread = malloc((MAXBUF+1)*sizeof(pthread_t));
    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	new_conn = malloc(1);
	*new_conn = conn_fd;
    if(conn_fd!=-1) {
        printf("Start Work\n");
        void *temp=getsize((void *)new_conn);
        pool_add_task(pool,request_handle, (void *)new_conn,temp);
        // printf("Schedule: %d\n Head: %d\n Tail: %d\n",get_schedule(pool),get_head(pool),get_tail(pool));
        // printf("%d %d\n",get_count(pool),getsize((void *)new_conn));
        print_heap(pool);
        }
    }
  // printf("All scheduled!\n");

/*
  srand(time(NULL));
  for (int i = 1; i <= 10; i++) {
    sleep(2);
    if (rand() % 2 == 0) {
      int *arg = malloc(sizeof(int));
      *arg = 1000 + i;
      pool_add_task(pool, work_routine, (void *)arg);
    }
  }
  */

    pool_wait(pool);
    pool_destroy(pool);


    printf("Done.");
	// if(conn_fd!=-1) {
	// 	// start = time(NULL);
	// 	if(enQueue(con_fd)==0) {
	// 		// printf("%d\n",i);
	// 		if(*(t+i)!=0) {
	// 			*(t+i)=pthread_create(worker_thread+i, NULL, request_handle, (void*) new_conn);
	// 			*(c+i)=conn_fd;
	// 			printf("%d\n",i);
	// 			// printf("%d\n",*(c+i));
	// 			break;
	// 		}
	// 		pthread_join(*(worker_thread+*end), NULL);
	// 		i=*(end);
	// 		*(t+i)=pthread_create(worker_thread+i, NULL, request_handle, (void*) new_conn);
	// 		*(c+i)=conn_fd;
	// 		// *(start)=i;
	// 			// *(end)=(j%MAXBUF)+1;
	// 		}

		// if(i==MAXBUF) {
				// for(int j=1;j<MAXBUF+1;j++) {
				// 	pthread_join(*(worker_thread+j), NULL);
				// 	*(end)=(j%MAXBUF)+1;
				// 	break;
					// if(*(c+j) != 0) {
					// 	// printf("%d\n",*(c+j));
					// 	close_or_die(*(c+j));
					// }
					
    return 0;
}


    


 
