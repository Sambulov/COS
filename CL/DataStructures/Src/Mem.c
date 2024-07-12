#include "CodeLib.h"

/* Copy memory to memory */
void mem_cpy(void* dst, const void* src, size_t cnt)
{
	uint8_t *d = (uint8_t*)dst;
	const uint8_t *s = (const uint8_t*)src;

	if (dst != src) 
	{
		while (cnt--) 
		{
			*d++ = *s++;
		}
	}
}

/* Move memory to memory */
void mem_move(void* dst, const void* src, size_t cnt)
{
	if (src != dst && cnt != 0)
	{
		ptrdiff_t dif = src - dst;
		if (dif > 0 || -dif >= cnt)
		{
			mem_cpy(dst, src, cnt);
		}
		else
		{
			uint8_t *d = (uint8_t *)(dst + cnt - 1);
			const uint8_t *s = (const uint8_t *)(src + cnt - 1);
			while (cnt--)
			{
				*d-- = *s--;
			}
		}
	}
}

/* Fill memory block */
void mem_set(void* dst, uint8_t val, size_t cnt)
{
	uint8_t *d = (uint8_t*)dst;
	while (cnt--)
	{
		*d++ = val;
	}
}


/* Compare memory block */
int8_t mem_cmp(const void* dst, const void* src, size_t cnt)	/* ZR:same, NZ:different */
{
	int8_t r = 0;
	if (dst != src && cnt != 0)
	{
		const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;
		do 
		{
			r = *d++ -*s++;
		} while (--cnt && r == 0);
	}
	return r;
}
