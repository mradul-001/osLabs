
#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Added for labquiz4


int
sys_is_proc_valid(void)
{
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return is_proc_valid(pid);
}

int
sys_get_proc_state(void)
{
  int pid;
  char *buf;
  int size;

  if (argint(0, &pid) < 0)
    return -1;
  if (argptr(1, &buf, sizeof(char) * 16) < 0)  // Assuming max state name size
    return -1;
  if (argint(2, &size) < 0)
    return -1;

  return get_proc_state(pid, buf, size);
}

int sys_fill_proc_name(void)
{
  int pid;
  char *name;
  
  if (argint(0, &pid) < 0 || argptr(1, &name, 16) < 0) 
    return -1;
  
  if (name == 0)  // Ensure the pointer is valid
    return -1;

  return fill_proc_name(pid, name);
}


int
sys_get_proc_name(void)
{
  int pid;
  char *buf;
  int size;

  if (argint(0, &pid) < 0)
    return -1;
  if (argptr(1, &buf, sizeof(char) * 16) < 0)  // Assuming max name size 16
    return -1;
  if (argint(2, &size) < 0)
    return -1;

  return get_proc_name(pid, buf, size);
}

int
sys_get_num_syscall(void)
{
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return get_num_syscall(pid);
}

int
sys_get_num_timer_interrupts(void)
{
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return get_num_timer_interrupts(pid);
}