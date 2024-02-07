// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

#define REF_SIZE 33000 // (PHYSTOP - PGROUNDUP((uint64)end)) / PGSIZE + 1

struct page_reference
{
  struct spinlock lock;
  signed char cnt[REF_SIZE];
} pg_ref;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pg_ref.lock, "pg_ref");
  printf("end:%p stop:%p\n", end, PHYSTOP);
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    kfree(p);
    //printf("%p\n", p);
  }
}

int
page_ref(void *pa)
{
  int cnt = 0;
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("page_ref");

  acquire(&pg_ref.lock);
  cnt = pg_ref.cnt[((uint64)pa - PGROUNDUP((uint64)end)) / PGSIZE]++;
  release(&pg_ref.lock);

  return cnt; 
}

int
page_deref(void *pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("page_deref");

  acquire(&pg_ref.lock);
  int cnt = --pg_ref.cnt[((uint64)pa - PGROUNDUP((uint64)end)) / PGSIZE];
  if (cnt > 0)
  {
    release(&pg_ref.lock);
    return cnt;
  }
  else if (cnt < 0)
    pg_ref.cnt[((uint64)pa - PGROUNDUP((uint64)end)) / PGSIZE] = 0;
  release(&pg_ref.lock);
  return 0;
}

int get_page_ref_cnt(void *pa)
{
  int cnt = 0;
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("get_page_cnt");

  acquire(&pg_ref.lock);
  cnt = pg_ref.cnt[((uint64)pa - PGROUNDUP((uint64)end)) / PGSIZE];
  release(&pg_ref.lock);
  return cnt;
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  if (get_page_ref_cnt(pa) > 0)
    return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
