#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include "request.h"
#include "io_helper.h"
#include "thread_pool.h"

struct q_node{
    void *(*routine)(void *);
    void *arg;
};
// void* get_arg(q_node *ar) {
//     return ar->arg;
// }

struct thread_pool{
    pthread_t *workers; //worker threads
    struct q_node* queue; //queue that holds tasks waiting to be executed
    int q_head, q_tail;
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
int get_head(struct thread_pool *pool){
    return (pool->q_head)% pool->TASK_Q_MAX;
}
int get_tail(struct thread_pool *pool){
    return (pool->q_tail)% pool->TASK_Q_MAX;
} 
struct task_thread_args{
    struct thread_pool *pool;
    struct q_node qn;
};


void *worker_func(void *pool_arg){
    struct thread_pool *pool = (struct thread_pool *)pool_arg;
    while(1){
        struct q_node task_picked;
        pthread_mutex_lock(&pool->mutex);

        while (pool->q_head == pool->q_tail){
            //empty queue
            pthread_cond_wait(&pool->task_available, &pool->mutex);
        }

        //assert(pool->q_head != pool->q_tail);
        task_picked = pool->queue[pool->q_head % pool->TASK_Q_MAX];
        pool->q_head++;
        pool->scheduled++; //task scheduled

        pthread_mutex_unlock(&pool->mutex);

        task_picked.routine(task_picked.arg);

        pthread_mutex_lock(&pool->mutex);
        
        
        
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

    if (pool->q_head == pool->q_tail){
        pthread_cond_broadcast(&pool->task_available);
    }

    struct q_node task;
    task.routine = routine;
    task.arg = arg;

    pool->queue[pool->q_tail % pool->TASK_Q_MAX] = task;
    pool->q_tail++;

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

    pool->q_head = pool->q_tail = 0;
    pool->scheduled = 0;
    pool->queue = malloc(sizeof(pthread_t)*max_threads);
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
    free(pool->queue);

    free(pool);
}

// int main() {
//     return 0;
// }
