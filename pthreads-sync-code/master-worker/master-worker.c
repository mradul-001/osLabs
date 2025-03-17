#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int items_produced, curr_buf_size, items_consumed;
int total_items, max_buf_size, num_workers, num_masters;

pthread_mutex_t prodLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prodCondi = PTHREAD_COND_INITIALIZER;
pthread_cond_t consCondi = PTHREAD_COND_INITIALIZER;

int *buffer;

void print_produced(int num, int master)
{
    printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker)
{
    printf("Consumed %d by worker %d\n", num, worker);
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *generate_requests_loop(void *data)
{
    int thread_id = *((int *)data);
    while (1)
    {
        pthread_mutex_lock(&lock);

        if (items_produced >= total_items)
        {
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        }

        while (curr_buf_size >= max_buf_size)
            pthread_cond_wait(&cond, &lock);

        if (items_produced >= total_items)
        {
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        }

        buffer[curr_buf_size] = items_produced;
        curr_buf_size = curr_buf_size + 1;
        print_produced(items_produced, thread_id);
        items_produced++;

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *handle_requests_loop(void *data)
{
    int thread_id = *((int *)data);
    while (1)
    {
        pthread_mutex_lock(&lock);

        if (items_consumed >= total_items)
        {
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        }

        while (curr_buf_size <= 0)
            pthread_cond_wait(&cond, &lock);

        if (items_consumed >= total_items)
        {
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        }

        print_consumed(buffer[--curr_buf_size], thread_id);
        items_consumed++;

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

// write function to be run by worker threads
// ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{

    int *master_thread_id;
    pthread_t *master_thread;
    items_produced = 0;
    items_consumed = 0;
    curr_buf_size = 0;

    int i;

    if (argc < 5)
    {
        printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
        exit(1);
    }
    else
    {
        num_masters = atoi(argv[4]);
        num_workers = atoi(argv[3]);
        total_items = atoi(argv[1]);
        max_buf_size = atoi(argv[2]);
    }

    buffer = (int *)malloc(sizeof(int) * max_buf_size);

    // ------------------------- create master producer threads -------------------------
    master_thread_id = (int *)malloc(sizeof(int) * num_masters);
    master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

    for (i = 0; i < num_masters; i++)
        master_thread_id[i] = i;

    for (i = 0; i < num_masters; i++)
        pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

    // ------------------------- create worker consumer threads -------------------------
    int *worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
    pthread_t *worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);

    for (int i = 0; i < num_workers; i++)
        worker_thread_id[i] = i;

    for (int i = 0; i < num_workers; i++)
        pthread_create(&worker_thread[i], NULL, handle_requests_loop, (void *)&worker_thread_id[i]);

    // ------------------------- wait for all threads to complete -------------------------
    for (i = 0; i < num_masters; i++)
    {
        pthread_join(master_thread[i], NULL);
        printf("master %d joined\n", i);
    }
    for (i = 0; i < num_workers; i++)
    {
        pthread_join(worker_thread[i], NULL);
        printf("worker %d joined\n", i);
    }

    // ------------------------- Deallocating Buffers -------------------------
    free(buffer);
    free(master_thread_id);
    free(master_thread);

    return 0;
}
