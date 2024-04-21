#include "app.h"
#include "CodeLib.h"

#if defined ( GD32E23X )

void test() {
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1) {
    cooperative_scheduler(false);
  }
}

#endif
