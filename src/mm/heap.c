#include <stdint.h>
#include <sys/types.h>
#include <lib/common.h>
#include <jamix/printk.h>

#pragma GCC diagnostic ignored "-Wmultichar" 

// I still dont really got the kernel memory map fully figured out
//
// [        2MB        ][                 8MB                 ]
//
// ^                    ^
// Kernel Image         Kernel Heap                

struct heap_metadata
{
  uint64_t size;
  bool used;
};

struct heap_block
{
  uint64_t magic;
  struct heap_metadata metadata;
};

#define KERNEL_HEAP_MAX     0x800000
#define KERNEL_HEAP_MAGIC   'heap'
#define KERNEL_HEAP_ALIGNMENT   8

static bool heap_initialized = false;
static void * heap_start = NULL;
static uint64_t heap_end, heap_used = 0;
static uint64_t prev_alloc = 0;

void heap_init(void * start)
{
  if(!heap_initialized)
  {
    heap_start = start;
    prev_alloc = (uint64_t)heap_start;
    heap_end = (uint64_t)(heap_start + KERNEL_HEAP_MAX);
    // clear all memory in the kernel heap range
    memset((void *)heap_start, 0, (heap_end - (uint64_t)heap_start));
    heap_initialized = true;
    return;
  }
  printk("WARNING: tried to re-init heap\n");
}

uint64_t heap_get_used(void)
{
  return heap_used; 
}

static inline size_t align(size_t size)
{
  return ALIGN(KERNEL_HEAP_ALIGNMENT, size);
}

static void * heap_get_free(size_t size)
{
  struct heap_block * block = NULL;
  uint64_t * mem = (uint64_t *)heap_start;

  size = align(size);
  // if this is our first alloc, return the start of the kernel heap
  if (heap_used == 0)
  {
    return (void *)prev_alloc;
  }

  while ((uint64_t)mem < heap_end)
  {
    block = (struct heap_block *)mem;
    if (block->magic != KERNEL_HEAP_MAGIC || !block->metadata.size)
    {
      return mem;
    }

    if (!block->metadata.used && block->metadata.size >= size)
    {
      return mem;
    }

    mem += (block->metadata.size + sizeof(struct heap_block)) / sizeof(uint64_t);
  }
  return NULL;
}

static void * heap_alloc(size_t size)
{
  if(!size)
    return NULL;
  struct heap_block * b;
  b = heap_get_free(size);
  if(!b)
    return NULL;
  size_t usage = prev_alloc + size + sizeof(struct heap_block);
  if(usage > heap_end)
  {
    return NULL;
  }

  // update variables
  
  prev_alloc += size + sizeof(struct heap_block);

  heap_used += size + sizeof(struct heap_block);
  // update header
  b->magic = KERNEL_HEAP_MAGIC;
  b->metadata.size = size;
  b->metadata.used = true;
  return (void *)((uint64_t)b + sizeof(struct heap_block));
}

static inline struct heap_block * get_block_from_ptr(void * ptr)
{
  return (struct heap_block *)(ptr - sizeof(struct heap_block));
}

static void heap_free(void * ptr)
{
  /* do we have a valid pointer? */
  if(!ptr)
  {
    printk("WARNING: Freeing a null pointer\n");
    return;
  }
  struct heap_block * b = get_block_from_ptr(ptr);
  if(b->magic != KERNEL_HEAP_MAGIC)
  {
    printk("WARNING: Block header is corrupted\n");
    return;
  }
  if(!b->metadata.used)
    return;
  // update variables
  heap_used -= b->metadata.size + sizeof(struct heap_block);
  b->metadata.used = false;
}

static void heap_merge_blocks(void)
{
  uint64_t * mem = (uint64_t *)heap_start;
  struct heap_block * prev = NULL;

  while ((uint64_t)mem < heap_end)
  {
    struct heap_block * block = (struct heap_block *)mem;

    /* is a valid block? */
    if (block->magic != KERNEL_HEAP_MAGIC)
      break;
    
    /* found two free blocks, now, we merge! */
    if (prev && !prev->metadata.used && !block->metadata.used)
      prev->metadata.size += block->metadata.size + sizeof(struct heap_block);
    else 
      prev = block; /* save block and look for other good ones */

    mem += (block->metadata.size + sizeof(struct heap_block)) / sizeof(uint64_t);
  }
}

void * malloc(size_t n)
{
	return heap_alloc(n);
}

void free(void * ptr)
{
	heap_free(ptr);
  /* merge free blocks to prevent fragmentation */
  heap_merge_blocks();
}

void * calloc(size_t num, size_t size)
{
  size_t sz = num * size;
  void * ptr = malloc(sz);
  if(!ptr)
    return NULL;
  struct heap_block * b = get_block_from_ptr(ptr);
  memset((void *)((uint64_t)b + sizeof(struct heap_block)), 0, sz);
  return ptr;
}
