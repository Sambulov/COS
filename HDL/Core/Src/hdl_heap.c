#include "hdl_base.h"

#define OCCUPIED_BLOCK_FLAG   0x80000000

extern const hdl_mem_block_t hdl_heap_regions[];

hdl_mem_block_t *first_block = NULL;

void _hdl_heap_init() {
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

uint8_t _hdl_try_alloc(hdl_mem_block_t *free_block, uint32_t size) {
  size = (size + 3) & ~0x3UL;
  if(free_block->size > size) {
    if(free_block->size > (size + 2 * sizeof(hdl_mem_block_t))) {
      uint32_t alloc_size = size + sizeof(hdl_mem_block_t);
      hdl_mem_block_t *split = (hdl_mem_block_t *)(((uint32_t)free_block) + alloc_size);
      void *addr = free_block->address;
      split->address = addr;
      split->size = free_block->size - alloc_size;
      free_block->address = split;
      free_block->size = alloc_size;
    }
    else 
    {
      __NOP();
      /* code */
    }
    free_block->size |= OCCUPIED_BLOCK_FLAG;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

void *hdl_malloc(size_t size) {
  if(!size) return NULL;
  if(first_block == NULL) _hdl_heap_init();
  hdl_mem_block_t *block = first_block;
  while (block != NULL) {
    if(!(block->size & OCCUPIED_BLOCK_FLAG) && _hdl_try_alloc(block, size))
      return (void *)((uint32_t)block + sizeof(hdl_mem_block_t));
    block = block->address;
  }
  return NULL;
}

static void _hdl_join_free_blocks() {
  hdl_mem_block_t *block = first_block;
  hdl_mem_block_t *next = NULL;
  while (block != NULL) {
    if(!(block->size & OCCUPIED_BLOCK_FLAG)) {
      next = block->address;
      if((next != NULL) && !(next->size & OCCUPIED_BLOCK_FLAG)) {
        if((void *)((uint32_t)block + block->size) == next) {
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
  ptr = (void *)((uint32_t)ptr - sizeof(hdl_mem_block_t));
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
