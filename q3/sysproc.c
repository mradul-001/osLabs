#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

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

/////////////// New Additions ////////////

struct psm *psmMain;

int sys_registerPub(void)
{
  // initialise the publisher
  struct proc *p = myproc();
  if (p->role == -1)
    p->role = 1;
  return 0;
}

int sys_registerSub(void)
{
  // initialise the subscriber
  struct proc *p = myproc();
  if (p->role == -1)
    p->role = 0;
  return 0;
}

int sys_publish(void)
{
  // retrieve the arguments and call psmPublish
  if (myproc()->role != 1)
    return -1;
  int *msg;
  int length;
  if (argstr(0, (char **)&msg) == -1)
    return -1;
  if (argint(1, &length) == -1)
    return -1;
  psmPublish(psmMain, msg, length);
  return 0;
}

int sys_retrieve(void)
{
  // retrieve the arguments and call psmRetrieve
  if (myproc()->role != 0)
    return -1;
  int *buff;
  int length;
  if (argstr(0, (char **)&buff) == -1)
    return -1;
  if (argint(1, &length) == -1)
    return -1;
  if (psmRetrieve(psmMain, buff, length) == -1)
    return -1;
  return 0;
}

void sys_initPubSub(void)
{
  char *mem = kalloc();
  psmMain = (struct psm *)mem;
  initpsm(psmMain);
}

//////////////////////////////////////////