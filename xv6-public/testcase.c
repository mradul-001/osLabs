#include "user.h"

int main() {

    // hello();

    // helloYou("Mradul");

    // printf(getNumProc(), "Number of processes: %d\n");

    // printf(getMaxPid(), "Maximum pid: %d\n");

    struct processInfo p;

    getProcInfo(2, &p);

    return 0;
}