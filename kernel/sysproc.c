#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  struct proc *p = myproc();

  if (argaddr(0, &p->trace_mask) < 0)
    return -1;
  
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct proc *cur_p = myproc();
  uint64 u_sysinfo_p;
  struct proc *p;
  struct
  {
    uint64 freemem;   // amount of free memory (bytes)
    uint64 nproc;     // number of process
  }sysinfo;
  
  sysinfo.freemem = 0;
  sysinfo.nproc = 0;

  extern struct 
  {
    struct spinlock lock;
    struct run *freelist;
    uint64 free_page_n;
  } kmem;
  
  sysinfo.freemem = kmem.free_page_n * 4096;

  extern struct proc proc[NPROC];
  for(p = &proc[0]; p < &proc[NPROC]; p++) 
  {
    acquire(&p->lock);
    if(p->state != UNUSED)
      sysinfo.nproc++;
    release(&p->lock);
  }
  
  if (argaddr(0, &u_sysinfo_p) < 0)
    return -1;
  
  if (copyout(cur_p->pagetable, u_sysinfo_p, (char *)&sysinfo, sizeof(sysinfo)) < 0)
    return -1;
  
  return 0;
}