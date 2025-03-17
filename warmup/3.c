#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


void *printFunc(void *index)
{
    sleep(rand() % 10);
    printf("I am thread %d\n", *(int *)index);
    return NULL;
}


int id = 1;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condi = PTHREAD_COND_INITIALIZER;

void *printFunc2(void *index)
{
    pthread_mutex_lock(&lock);
    while (id != *(int *)index)
    {
        pthread_cond_wait(&condi, &lock);
    }
    id++;
    sleep(2);
    printf("I am thread %d\n", *(int *)index);
    pthread_cond_broadcast(&condi);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main()
{

    int N = 10;
    pthread_t threads[N];
    int args[N];

    printf("Without the use of conditional variables.\n");
    for (int i = 0; i < N; i++)
    {
        args[i] = i + 1;
        pthread_create(&threads[i], NULL, &printFunc, (void *)&args[i]);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("With the use of conditional variables.\n");

    for (int i = 0; i < N; i++)
    {
        args[i] = i + 1;
        pthread_create(&threads[i], NULL, &printFunc2, (void *)&args[i]);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}