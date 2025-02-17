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
    char *osString = "OsIsFun";
    unsigned int SIZE = 4100;
    unsigned fd;
    void *ptr;

    fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("Problem in creating the shared memory");
    }

    ftruncate(fd, SIZE);

    ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("Error in mapping the shared memory in VT of the process");
    }
    void *copy = ptr;
    void *cptr = ptr + 512 * (strlen(freeString) + 1);

    for (int i = 0; i < 512; i++)
    {
        sprintf(copy, "%s", freeString);
        copy += strlen(freeString) + 1;
    }
    printf("%s", "Wrote the free strigns once, will now write os strings.\n");
    sleep(2);

    int count = 0;
    while (1)
    {
        copy = ptr;
        for (int i = 0; i < 512; i++)
        {
            if (++count == 1000)
            {
                sprintf(cptr, "%s", "2");
                printf("%s", "Program terminated.\n");
                return 0;
            }
            sprintf(copy, "%s", osString);
            copy += strlen(osString) + 1;
        }
        printf("%s", "I have written something, let the consumer clean it.\n");
        sleep(2);
        sprintf(cptr, "%s", "1"); // now consumer can work
        while (*(char *)cptr == '1')
        {
        }
    }

    return 0;
}