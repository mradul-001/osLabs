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


// ---------------- Author: Mradul Sonkar ----------------

int sys_hello(void) {
  cprintf("Hello\n");
  return 0;
}

int sys_helloYou(void) {
  char *name;
  if (argstr(0, &name) == -1) return -1;
  cprintf("Hello, %s\n", name);
  return 0;
}

int sys_getNumProc(void) {
  return getNumProc();
}

int sys_getMaxPid(void) {
  return getMaxPid();
}

int sys_getProcInfo(void) {
  // extract the pid of process from the arguments
  int pid;
  argint(0, &pid);
  struct processInfo *addr;
  argstr(1, (char **)&addr);
  getProcInfo(pid, addr);
  return 0;
}


int sys_setPrio(void) {
  int priority;
  if (argint(0, &priority) == -1) return -1;
  if (setPrio(priority) == -1) return -1;
  return 0;
}

int sys_getPrio(void) {
  return getPrio();
}