#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main()
{

    char *NAME = "/osLab";
    char *freeString = "freeeee";
    unsigned int SIZE = 4100;
    unsigned fd;
    void *ptr;

    fd = shm_open(NAME, O_RDWR, 0666);
    if (fd == -1)
    {
        printf("%s", "Consumer: Error accessing the shared memory.\n");
    }

    ptr = mmap(NULL, SIZE, PROT_READ | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("%s", "Consumer: Error mapping the shared memory address.\n");
    }
    void *copy = ptr;
    void *cptr = ptr + 512 * (strlen(freeString) + 1);


    // run forever
    while (1)
    {
        printf("%s", "ran \n");
        copy = ptr;
        char value = *((char *) cptr);
        printf("%s", "ran \n");
        printf("%d\n", value);


        // if producer is writing, wait
        if (value == 48)
        {
            while (value == 48)
            {
            }
        }


        // if producer has completed everything, you exit too
        if (value == 50)
        {
            for (int i = 0; i < 512; i++)
            {
                sprintf(copy, "%s", freeString);
                copy += strlen(freeString) + 1;
            }
            printf("Program finished.\n");
            exit(EXIT_SUCCESS);
        }


        // if producer has instructed for freeing, free the strings and tell the producer
        if (value == 49)
        {
            for (int i = 0; i < 512; i++)
            {
                printf("%p\n", ptr);
                printf("%p\n", copy);
                sprintf(copy, "%s", freeString);
                copy += strlen(freeString) + 1;
                sleep(1);
            }
            printf("%s", "I have completed cleaning the memory, let the producer work.\n");
            sleep(2);
            sprintf(ptr, "%s", "0");
        }
    }

    return 0;
}