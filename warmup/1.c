#include <pthread.h>
#include <stdio.h>

int counter = 0;
pthread_mutex_t lock;

void *incrementCounter(void *)
{
    pthread_mutex_lock(&lock);
    int t = 1000;
    while (t-- > 0)
    {
        counter++;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main()
{
    pthread_t threads[10];

    for (int i = 0; i < 10; i++)
    {
        pthread_create(&threads[i], NULL, &incrementCounter, NULL);
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Value of counter: %d\n", counter);

    return 0;
}