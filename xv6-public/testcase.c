#include "user.h"

int main() {

    // hello();
    // helloYou("Mradul");
    // printf(getNumProc(), "Number of processes: %d\n");
    // printf(getMaxPid(), "Maximum pid: %d\n");


    struct processInfo info;
    int pid;
    printf(1, "Total Number of Active Processes: %d\n", getNumProc());
    printf(1, "Maximum PID: %d\n\n", getMaxPid());
    
    printf(1, "PID\t\tPPID\t\tSIZE\t\tNumber of Context Switch\n");
    for(int i=1; i<=getMaxPid(); i++)
    {
        pid = i;
        if(getProcInfo(pid, &info) == 0)
	  printf(1, "%d\t\t%d\t\t%d\t\t%d\n", pid, info.ppid, info.psize, info.numberContextSwitches);
    }



    return 0;
}