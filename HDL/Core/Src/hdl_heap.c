#include "hdl_base.h"

#define OCCUPIED_BLOCK_FLAG   0x80000000

extern const hdl_mem_block_t hdl_heap_regions[];

static hdl_mem_block_t *first_block = NULL;

static void _hdl_heap_init() {
  uint32_t i = 0;
  hdl_mem_block_t *current = NULL;
  hdl_mem_block_t *prev = NULL;
  while (hdl_heap_regions[i].address != NULL) {
    if(hdl_heap_regions[i].size > sizeof(hdl_mem_block_t)) {
      current = (hdl_mem_block_t *)hdl_heap_regions[i].address;
      current->address = NULL;
      current->size = hdl_heap_regions[i].size;
      if(prev != NULL)
        prev->address = current;
      if(first_block == NULL)
        first_block = current;
      prev = current;
    }
    i++;
  }
}

static void _hdl_split_blok(hdl_mem_block_t *block, uint32_t size) {
  if(block->size <= (size + 2 * sizeof(hdl_mem_block_t)))
    return;
  uint32_t alloc_size = size + sizeof(hdl_mem_block_t);
  hdl_mem_block_t *split = (hdl_mem_block_t *)(((size_t)block) + alloc_size);
  split->address = block->address;
  split->size = block->size - alloc_size;
  block->address = split;
  block->size = alloc_size;
}

static uint8_t _hdl_try_alloc(hdl_mem_block_t *block, uint32_t size) {
  if(!block || (block->size & OCCUPIED_BLOCK_FLAG) || ((block->size - sizeof(hdl_mem_block_t)) < size))
    return HDL_FALSE;
  _hdl_split_blok(block, size);
  block->size |= OCCUPIED_BLOCK_FLAG;
  return HDL_TRUE;
}

static hdl_mem_block_t *_hdl_find_next_free(hdl_mem_block_t *cur_block) {
  if(!first_block) 
    _hdl_heap_init();
  if(!cur_block)
    cur_block = first_block;
  else
    cur_block = cur_block->address;
  while ((cur_block != NULL) && (cur_block->size & OCCUPIED_BLOCK_FLAG))
    cur_block = cur_block->address;
  return cur_block;
}

void *hdl_malloc(size_t size) {
  size = (size + 3) & ~0x3UL;
  if(!size) 
    return NULL;
  hdl_mem_block_t *block = NULL;
  while ((block = _hdl_find_next_free(block)) && !_hdl_try_alloc(block, size));
  if(!block)
    return NULL;
  return (void *)((size_t)block + sizeof(hdl_mem_block_t));
}

static void _hdl_join_free_blocks() {
  hdl_mem_block_t *block = first_block;
  hdl_mem_block_t *next = NULL;
  while (block != NULL) {
    if(!(block->size & OCCUPIED_BLOCK_FLAG)) {
      next = block->address;
      if((next != NULL) && !(next->size & OCCUPIED_BLOCK_FLAG)) {
        if((void *)((size_t)block + block->size) == next) {
          block->size += next->size;
          block->address = next->address;
          continue;
        }
      }
    }
    block = block->address;
  }
}

void hdl_free(void *ptr) {
  ptr = (void *)((size_t)ptr - sizeof(hdl_mem_block_t));
  hdl_mem_block_t *block = first_block;
  while (block != NULL) {
    if((block->size & OCCUPIED_BLOCK_FLAG) && (ptr == block)) {
      block->size &= ~OCCUPIED_BLOCK_FLAG;
      _hdl_join_free_blocks();
      return;
    }
    block = block->address;
  }
}

void hdl_heap_stat(uint32_t *free, uint32_t *total) {
  if(!first_block) 
    _hdl_heap_init();
  hdl_mem_block_t *block = first_block;
  uint32_t _total = 0, _free = 0;
  while (block) {
    if(!(block->size & OCCUPIED_BLOCK_FLAG))
      _free += block->size;
    _total += (block->size & ~OCCUPIED_BLOCK_FLAG);
    block = block->address;
  };
  if(free)
    *free = _free;
  if(total)
    *total = _total;
}
