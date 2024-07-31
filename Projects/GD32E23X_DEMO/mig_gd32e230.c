/*
  Module Init Graph (MIG)
*/

#include "hdl.h"

void * isr_vector[] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &reset_handler,
};

