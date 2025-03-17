#include <stdio.h>
#include <pthread.h>

int id = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condi = PTHREAD_COND_INITIALIZER;

void *func(void *arg)
{
    pthread_mutex_lock(&lock);
    while (id != *(int *)arg)
    {
        pthread_cond_wait(&condi, &lock);
    }
    id++;
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", i + 1);
    }
    printf("\n");
    pthread_cond_broadcast(&condi);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main()
{

    pthread_t threads[10];
    int ids[10];

    for (int i = 0; i < 10; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], NULL, &func, &ids[i]);
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}