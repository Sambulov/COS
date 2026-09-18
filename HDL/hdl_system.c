#include "hdl_base.h"
#include "hdl_diag.h"

__WEAK void _read(void) { }
__WEAK void _write(void) { }
__WEAK void _close(void) { }
__WEAK void _lseek(void) { }
__WEAK void _isatty(void) { }
__WEAK void _fstat(void) { }
__WEAK void _getpid(void) { }
__WEAK void _kill(void) { }

__WEAK void hdl_diag(const char *message, int32_t line, const char *file) {
  (void)message;
  (void)line;
  (void)file;
}

/* The C library heap starts at the linker symbol `end`, which is the same address
   as `_free_ram_start` - the region given to hdl_malloc. Two independent allocators
   over one memory region corrupt each other, so the library heap is denied. */
void *_sbrk(ptrdiff_t incr) {
  (void)incr;
  return (void *)-1;
}
