#include <pthread.h>
#include <stdio.h>

void *msgFunc(void *index)
{
    printf("I am thread %d\n", *(int *)index);
    return NULL;
}

int main()
{

    int N = 10;

    pthread_t threads[N];
    int args[N];

    for (int i = 0; i < N; i++)
    {
        args[i] = i + 1;
        pthread_create(&threads[i], NULL, &msgFunc, (void *)&args[i]);
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("I am the main thread.\n");

    return 0;
}