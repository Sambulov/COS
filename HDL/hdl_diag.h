#ifndef HDL_DIAG_H_
#define HDL_DIAG_H_

#include <stdint.h>

/*
  Platform diagnostics hook. HDL passes messages that must not pull in the C
  library (lwIP assertions and diagnostics). A project can replace the weak
  implementation with its own strong one, for example to redirect into a log.
*/
void hdl_diag(const char *message, int32_t line, const char *file);

#endif /* HDL_DIAG_H_ */
