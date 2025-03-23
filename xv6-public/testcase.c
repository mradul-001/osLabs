#include "user.h"

int arr[2200];

int main()
{
    for (int i = 0; i < 100; i++)
    {
        arr[i] = i * 10;
    }
    printf(1, "Number of virtual pages: %d\n", numvp());
    // printf(1, "Number of physical pages: %d\n", numpp());
    printf(1, "%d", arr[0]);
    exit();
}