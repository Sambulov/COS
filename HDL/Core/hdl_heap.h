#ifndef HDL_HEAP_H_
#define HDL_HEAP_H_

#define static_malloc(size)             (&((size_t [(size + sizeof(size_t *) - 1) >> 2]){ 0 }))

typedef struct {
  void *address;
  uint32_t size;
} hdl_mem_block_t;

void *hdl_malloc(size_t size);

static inline void *hdl_calloc(size_t amount, size_t block) {
  size_t size = amount * block;
  uint8_t* ptr = (uint8_t *)hdl_malloc(size);
  if (ptr != NULL) while (size--) ptr[size] = 0;
  return ptr;
}

void hdl_free(void *ptr);

#endif /* HDL_HEAP_H_ */
