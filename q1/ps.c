#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int e_flag = 0;
    if (argc > 1 && strcmp(argv[1], "-e") == 0)
    {
        e_flag = 1;
    }
    if (!e_flag)
    {
        int pid = getpid();
        char buf1[16];
        char buf2[16];

        fill_proc_name(pid, "ps");

        int status = get_proc_name(pid, buf1, 16);
        if (status < 0)
            return -1;

        int status2 = get_proc_state(pid, buf2, 16);
        if (status2 < 0)
            return -1;

        int ns = get_num_syscall(pid), nt = get_num_timer_interrupts(pid);

        printf(1, "PID\tNAME\tSTATE\tSYS\tINT\n");
        printf(1, "%d\t%s\t%s\t%d\t%d\n", pid, buf1, buf2, ns, nt);
    }
    else
    {
        printf(1, "PID\tSTATE\tSYS\tINT\n");
        for (int i = 1; i < 65; i++)
        {
            if (is_proc_valid(i) == 1)
            {
                int pid = i;
                char buf1[16];
                char buf2[16];

                int status = get_proc_name(pid, buf1, 16);
                if (status < 0)
                    return -1;

                int status2 = get_proc_state(pid, buf2, 16);
                if (status2 < 0)
                    return -1;

                int ns = get_num_syscall(pid), nt = get_num_timer_interrupts(pid);
                printf(1, "%d\t%s\t%d\t%d\n", pid, buf2, ns, nt);
            }
        }
    }
    exit();
}
