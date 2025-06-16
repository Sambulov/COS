#ifndef HDL_HEAP_H_
#define HDL_HEAP_H_

#define static_malloc(size)             (&((size_t [(size + sizeof(size_t *) - 1) >> 2]){ 0 }))

typedef struct {
  void *address;
  uint32_t size;
} hdl_mem_block_t;

void *hdl_malloc(uint32_t size);
void hdl_free(void *ptr);

#endif /* HDL_HEAP_H_ */
