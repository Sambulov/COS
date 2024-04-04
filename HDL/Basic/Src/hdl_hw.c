#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* private */
  __linked_list_object__
  uint32_t dependents;
  /* public */
  hdl_hardware_initializer_t init;
} hdl_hardware_private_t;

_Static_assert(sizeof(hdl_hardware_private_t) == sizeof(hdl_hardware_t), "In hdl_hw.h data structure size of hdl_hardware_t doesn't match, check HDL_HW_PRIVATE_SIZE");

static linked_list_t init_queue = NULL;
static linked_list_t dev_tree = NULL;

hdl_init_state_t hdl_hw_state(hdl_hardware_t *desc) {
  return HDL_HW_INIT_FAILED;
}

static void hdl_hw_work() {
  /* todo: find first uninitialized, run init */
}

hdl_init_state_t hdl_hw_enable(hdl_hardware_t *desc, uint8_t enable) {
  /* check if worker run */
  /* do nothing if we are in list */
  hdl_hardware_private_t *hw = (hdl_hardware_private_t *)desc;
  if(linked_list_contains(init_queue, linked_list_item(hw))) {
    hw->dependents++;
    return hdl_hw_state(desc);
  }
  hdl_init_state_t res = hw->init(desc, enable);
  linked_list_insert_last(&init_queue, linked_list_item(hw));
  return res;
}
