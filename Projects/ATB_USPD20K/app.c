#include "app.h"
#include "CodeLib.h"

//extern hdl_core_t mod_sys_core;

void main() {
  hdl_enable(&mod_app); 
  //hdl_enable(&mod_sys_core.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  while (1) {
    cooperative_scheduler(false);
  }
}
