#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "thread_pool.h"

struct q_node{
    void *(*routine)(void *);
    void *arg;
    int size;
};
// #include<stdio.h>
// #include<stdlib.h>
/*
 Array Implementation of MinHeap data Structure
*/

struct Heap{
    struct q_node *arr;
    int count;
    int capacity;
};

typedef struct Heap Heap;

// int main(){
//     int i;
//     Heap *heap = CreateHeap(HEAP_SIZE, 0); //Min Heap
//     if( heap == NULL ){
//         printf("__Memory Issue____\n");
//         return -1;
//     }

//     for(i =9;i>0;i--)
//         insert(heap, i);

//     print(heap);

//     for(i=9;i>=0;i--){
//         printf(" Pop Minima : %d\n", PopMin(heap));
//         print(heap);
//     }
//     return 0;
// }

Heap *CreateHeap(int capacity){
    Heap *h = (Heap * ) malloc(sizeof(Heap)); //one is number of heap

    //check if memory allocation is fails
    if(h == NULL){
        printf("Memory Error!");
        return h;
    }
    h->count=0;
    h->capacity = capacity;
    h->arr = (struct q_node *) malloc(capacity*sizeof(struct q_node)); //size in bytes

    //check if allocation succeed
    if ( h->arr == NULL){
        printf("Memory Error!");
        return h;
    }
    return h;
}
void heapify_bottom_top(Heap *h,int index){
    struct q_node temp;
    int parent_node = (index-1)/2;

    if(h->arr[parent_node].size > h->arr[index].size){
        //swap and recursive call
        temp = h->arr[parent_node];
        h->arr[parent_node] = h->arr[index];
        h->arr[index] = temp;
        heapify_bottom_top(h,parent_node);
    }
}

void heapify_top_bottom(Heap *h, int parent_node){
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min;
    struct q_node temp;

    if(left >= h->count || left <0)
        left = -1;
    if(right >= h->count || right <0)
        right = -1;

    if(left != -1 && h->arr[left].size < h->arr[parent_node].size)
        min=left;
    else
        min=parent_node;
    if(right != -1 && h->arr[right].size < h->arr[min].size)
        min = right;

    if(min != parent_node){
        temp = h->arr[min];
        h->arr[min] = h->arr[parent_node];
        h->arr[parent_node] = temp;

        // recursive  call
        heapify_top_bottom(h, min);
    }
}
void insert(Heap *h, struct q_node key){
    if( h->count < h->capacity){
        h->arr[h->count] = key;
        heapify_bottom_top(h, h->count);
        h->count++;
    }
}



struct q_node PopMin(Heap *h){
    struct q_node pop;
    if(h->count==0){
        printf("\n__Heap is Empty__\n");
        return NULL;
    }
    // replace first node by last and delete last
    pop = h->arr[0];
    h->arr[0] = h->arr[h->count-1];
    h->count--;
    heapify_top_bottom(h, 0);
    return pop;
}
// void print(Heap *h){
//     int i;
//     printf("____________Print Heap_____________\n");
//     for(i=0;i< h->count;i++){
//         printf("-> %d ",h->arr[i]);
//     }
//     printf("->__/\\__\n");
// }
// void* get_arg(q_node *ar) {
//     return ar->arg;
// }

struct thread_pool{
    pthread_t *workers; //worker threads
    Heap* heap; //queue that holds tasks waiting to be executed
    int max_workers;
    int TASK_Q_MAX;
    int scheduled; //keeps track of number of tasks scheduled
    pthread_mutex_t mutex;
    pthread_cond_t task_available; //signaled on when we go from no task to task available
    pthread_cond_t no_task_left; //signaled on when no more tasks scheduled

};
// const int TASK_Q_MAX = 7;
int get_max_threads(struct thread_pool *pool){
    return pool->max_workers;
}
int get_schedule(struct thread_pool *pool){
    return pool->scheduled;
}
// int get_head(struct thread_pool *pool){
//     return (pool->q_head)% pool->TASK_Q_MAX;
// }
// int get_tail(struct thread_pool *pool){
//     return (pool->q_tail)% pool->TASK_Q_MAX;
// } 
struct task_thread_args{
    struct thread_pool *pool;
    struct q_node qn;
};


void *worker_func(void *pool_arg){
    struct thread_pool *pool = (struct thread_pool *)pool_arg;
    while(1){
        struct q_node task_picked;
        pthread_mutex_lock(&pool->mutex);

        while ((pool->heap)->count==0){
            //empty queue
            pthread_cond_wait(&pool->task_available, &pool->mutex);
        }

        //assert(pool->q_head != pool->q_tail);
        task_picked = PopMin(pool->heap);
        pool->scheduled++; //task scheduled

        pthread_mutex_unlock(&pool->mutex);

        task_picked.routine(task_picked.arg);

        pthread_mutex_lock(&pool->mutex);
        
        printf("Work Done \n");
        
        pool->scheduled--;

        if (pool->scheduled == 0){
            pthread_cond_signal(&pool->no_task_left);
        }

        pthread_mutex_unlock(&pool->mutex);
    }

    return NULL;
}

void pool_add_task(struct thread_pool *pool, void *(*routine)(void*), void *arg){
    pthread_mutex_lock(&pool->mutex);

    if ((pool->heap)->count==0){
        pthread_cond_broadcast(&pool->task_available);
    }

    struct q_node task;
    task.routine = routine;
    task.arg = arg;
    task.size = getsize(arg);

    insert(pool->heap,task);

    pthread_mutex_unlock(&pool->mutex);
}


void pool_wait(struct thread_pool *pool){
    pthread_mutex_lock(&pool->mutex);
    while (pool->scheduled > 0 ){
        pthread_cond_wait(&pool->no_task_left, &pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
}

struct thread_pool* pool_init(int max_threads,int buffer){
    struct thread_pool* pool = malloc(sizeof(struct thread_pool));

    // pool->q_head = pool->q_tail = 0;
    pool->scheduled = 0;
    pool->heap = CreateHeap(buffer);
    pool->TASK_Q_MAX = buffer;
    pool->max_workers = max_threads;
    pool->workers = malloc(sizeof(pthread_t)*max_threads);

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->task_available, NULL);
    pthread_cond_init(&pool->no_task_left, NULL);

    for (int i=0; i<max_threads; i++){
        pthread_create(&pool->workers[i], NULL, worker_func, pool);
    }

    return pool;
}

void pool_destroy(struct thread_pool *pool){
    pool_wait(pool);

    for (int i=0; i < pool->max_workers; i++){
        pthread_detach(pool->workers[i]);
    }

    free(pool->workers);
    free(pool->heap);
    free(pool);
}

// int main() {
//     return 0;
// }
