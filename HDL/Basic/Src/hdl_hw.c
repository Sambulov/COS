#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* private */
  __linked_list_object__
  uint32_t dependents;
  /* public */
  event_handler_t *service;
  hdl_hardware_t **dependencies;
} hdl_hardware_private_t;

//_Static_assert(sizeof(hdl_hardware_private_t) == sizeof(hdl_hardware_t), "In hdl_hw.h data structure size of hdl_hardware_t doesn't match, check HDL_HW_PRIVATE_SIZE");

static linked_list_t init_queue = NULL;
static linked_list_t dev_dep_graph = NULL;
static linked_list_t deinit_queue = NULL;

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
  hdl_init_state_t res = HDL_HW_INIT_OK;
  if(enable) {
    if(hw->service != NULL) {
      //res = hw->service(1,desc, enable);
    }
    switch (res)
    {
    case HDL_HW_INIT_OK:
      linked_list_insert_last(&dev_dep_graph, linked_list_item(hw));
      break;
    case HDL_HW_INIT_ONGOING:
      linked_list_insert_last(&init_queue, linked_list_item(hw));
      break;
    default:
      res = HDL_HW_INIT_FAILED;
      break;
    }

  }
  else {

  }

  if(linked_list_contains(init_queue, linked_list_item(hw))) {
    hw->dependents++;
    return hdl_hw_state(desc);
  }
  
  linked_list_insert_last(&init_queue, linked_list_item(hw));
  return res;
}
