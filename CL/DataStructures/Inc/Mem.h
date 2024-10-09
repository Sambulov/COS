/*!
   Mem.h

    Created on: 11.02.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */
#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Copy memory to memory */
void mem_cpy(void* dst, const void* src, size_t cnt);

/* Fill memory block */
void mem_set(void* dst, uint8_t val, size_t cnt);


/* Compare memory block */
int8_t mem_cmp(const void* dst, const void* src, size_t cnt);	/* ZR:same, NZ:different */

/* Move memory to memory */
void mem_move(void* dst, const void* src, size_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* MEM_H_INCLUDED */
