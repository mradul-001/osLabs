#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int itemsProduced, curr_buf_size, itemsConsumed = 0;
int total_items, max_buf_size, num_workers, num_masters;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
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

void *generate_requests_loop(void *data)
{
    int thread_id = *(int *)data;
    while (1)
    {
        while (curr_buf_size >= max_buf_size)
            pthread_cond_wait(&prodCondi, &lock);

        if (itemsProduced >= total_items)
        {
            pthread_cond_broadcast(&prodCondi);
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        else
        {
            int random = rand() % 100;
            buffer[curr_buf_size++] = random;
            itemsProduced++;
            print_produced(random, thread_id);
            pthread_cond_broadcast(&consCondi);
            pthread_mutex_unlock(&lock);
        }
    }
}

void *consume_requests_loop(void *data)
{
    int thread_id = *(int *)data;
    while (1) {
        while (curr_buf_size == 0) {
            if (itemsProduced == total_items) {
                pthread_cond_broadcast(&consCondi);
                pthread_mutex_unlock(&lock);
                return NULL;
            }
            else {
                pthread_cond_wait(&consCondi, &lock);
            }
        }
        int temp = buffer[--curr_buf_size];
        print_consumed(temp, thread_id);
        pthread_cond_broadcast(&prodCondi);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int *master_thread_id;
    pthread_t *master_thread;
    itemsProduced = 0;
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

    // create master producer threads
    master_thread_id = (int *)malloc(sizeof(int) * num_masters);
    master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

    for (i = 0; i < num_masters; i++)
        master_thread_id[i] = i;

    for (i = 0; i < num_masters; i++)
        pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

    // create worker consumer threads
    // ---------------------------------------------------------------
    int *worker_thread_id = (int *)malloc(num_workers * sizeof(int));
    pthread_t *worker_thread = (pthread_t *)malloc(num_workers * sizeof(pthread_t));

    for (int i = 0; i < num_workers; i++)
    {
        worker_thread_id[i] = i;
        pthread_create(&worker_thread[i], NULL, &consume_requests_loop, (void *)&worker_thread_id[i]);
    }
    // ---------------------------------------------------------------

    // wait for all threads to complete
    for (i = 0; i < num_masters; i++)
    {
        pthread_join(master_thread[i], NULL);
        printf("master %d joined\n", i);
    }

    // waiting for all worker threads to complete
    // ---------------------------------------------------------------
    for (int i = 0; i < num_workers; i++)
    {
        pthread_join(worker_thread[i], NULL);
        printf("Worker %d joined.\n", worker_thread_id[i]);
    }
    // ---------------------------------------------------------------

    free(buffer);
    free(master_thread_id);
    free(master_thread);

    return 0;
}
