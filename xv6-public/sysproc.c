#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
/////////////////// New addition ///////////////////////
// #include "barrier.h"
/////////////////// End of new addition ///////////////////////


// define any variables needed here
struct barrier
{
	int counter;
	void * chan;
	struct spinlock lock;
} temp;

int barrier_init(int n)
{
	// to be done
	initlock(&temp.lock, "tempLock");
	acquire(&temp.lock);
	temp.counter = n;
	temp.chan = (void*) &temp;
	release(&temp.lock);
	return n;
}

int barrier_check(void)
{
	// to be done
	acquire(&temp.lock);
	if (--temp.counter == 0)
	{
		wakeup(temp.chan);
	}
	else
	{
		sleep(temp.chan, &temp.lock);
	}
	release(&temp.lock);
	return 0;
}

/*----------xv6 sync lock end----------*/


int sys_fork(void)
{
	return fork();
}

int sys_exit(void)
{
	exit();
	return 0; // not reached
}

int sys_wait(void)
{
	return wait();
}

int sys_kill(void)
{
	int pid;

	if (argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

int sys_getpid(void)
{
	return myproc()->pid;
}

int sys_sbrk(void)
{
	int addr;
	int n;

	if (argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if (growproc(n) < 0)
		return -1;
	return addr;
}

int sys_sleep(void)
{
	int n;
	uint ticks0;

	if (argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while (ticks - ticks0 < n)
	{
		if (myproc()->killed)
		{
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}

/////////////////// New addition ///////////////////////
int sys_thread_create(void)
{
	return -1;
}

int sys_thread_exit(void)
{
	return -1;
}

int sys_thread_join(void)
{
	return -1;
}

int sys_barrier_init(void)
{
	int arg;
	if (argint(0, &arg) < 0)
	{
		return -1;
	}
	if (barrier_init(arg) != arg)
	{
		return -1;
	}
	return -1;
}

int sys_barrier_check(void)
{
	return barrier_check();
}

int sys_waitpid(void)
{
	int pid;
	if (argint(0, &pid) < 0)
	{
		return -2; // some error occurred
	};
	return waitpid(pid);
}

////////////////// End of new addition /////////////////
/////////// Parts D and E of threads lab/////////
int sys_sleepChan(void)
{
	return -1;
}

int sys_getChannel(void)
{
	return -1;
}

int sys_sigChan(void)
{
	return -1;
}

int sys_sigOneChan(void)
{
	return -1;
}
/////////// End of Parts D and E of threads lab/////////
