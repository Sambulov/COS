#include "app.h"
#include "CodeLib.h"


void main() {
  //hdl_enable(&xxx.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1) {
    cooperative_scheduler(false);
  }
}
