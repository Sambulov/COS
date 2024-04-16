#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* private */
  __linked_list_object__
  uint32_t dependents;
  /* public */
  hdl_hardware_initializer_t init;
  hdl_hardware_t **dependencies;
  void *periph;
} hdl_hardware_private_t;

_Static_assert(sizeof(hdl_hardware_private_t) == sizeof(hdl_hardware_t), "In hdl_hw.h data structure size of hdl_hardware_t doesn't match, check HDL_HW_PRIVATE_SIZE");

static linked_list_t dev_init_queue = NULL;
static linked_list_t dev_list = NULL;
static linked_list_t dev_deinit_queue = NULL;

static uint8_t _hdl_hw_work(CoroutineDesc_t this, uint8_t cancel, void *arg) {
  hdl_init_state_t res;
  hdl_hardware_private_t *dev = linked_list_get_object(hdl_hardware_private_t, dev_deinit_queue);
  if(dev != NULL) {
    res = HDL_HW_DEINIT_OK;
    if(dev->init != NULL)
      res = dev->init(dev, HDL_FALSE);
    if(res == HDL_HW_DEINIT_OK)
      linked_list_unlink(linked_list_item(dev));
  }
  dev = linked_list_get_object(hdl_hardware_private_t, dev_init_queue);
  if(dev != NULL) {
    res = HDL_HW_INIT_OK;
    if(dev->init != NULL)
      res = dev->init(dev, HDL_TRUE);
    switch (res) {
      case HDL_HW_INIT_OK:
        linked_list_insert_last(&dev_list, linked_list_item(dev));
        break;
      case HDL_HW_INIT_ONGOING:
        break;
      default:
        hdl_hw_kill((hdl_hardware_t *)dev);
        break;
    }
  }
  return cancel; /* todo */
  /* ??? stop worker if no dev ??? */
  /* ??? run driver worker ??? */
}

hdl_init_state_t hdl_hw_state(hdl_hardware_t *desc) {
  hdl_init_state_t res = HDL_HW_INIT_UNKNOWN;
  if(desc != NULL) {
    hdl_hardware_private_t *hw = (hdl_hardware_private_t *)desc;
    res = linked_list_contains(dev_list, linked_list_item(hw))? HDL_HW_INIT_OK:
          linked_list_contains(dev_init_queue, linked_list_item(hw))? HDL_HW_INIT_ONGOING:
          linked_list_contains(dev_deinit_queue, linked_list_item(hw))? HDL_HW_INIT_TERMINATING:
          HDL_HW_INIT_UNKNOWN;
  }
  return res;
}

static void _hdl_hw_enable_parents(hdl_hardware_t *desc) {
  if(desc->dependencies != NULL) {
    hdl_hardware_t **dependency = desc->dependencies;
    while(*dependency != NULL) {
      hdl_hw_enable(*dependency);
      dependency++;
    }
  }
}

void hdl_hw_enable(hdl_hardware_t *desc) {
  static coroutine_desc_static_t hw_worker;
  hdl_hardware_private_t *hw = (hdl_hardware_private_t *)desc;
  hdl_init_state_t res = hdl_hw_state(desc);
  if(res < HDL_HW_INIT_OK) {
    _hdl_hw_enable_parents(desc);
    hw->dependents = 1;
    linked_list_insert_last(&dev_init_queue, linked_list_item(hw));
    coroutine_add_static(&hw_worker, &_hdl_hw_work, NULL);
  }
  else
    hw->dependents++; /* check overlap */
}

static uint8_t _dev_parent_match(LinkedListItem_t *item, void *arg) {
  hdl_hardware_private_t *parent = (hdl_hardware_private_t *)arg;
  hdl_hardware_private_t **dependency = (hdl_hardware_private_t **)linked_list_get_object(hdl_hardware_private_t, item)->dependencies;
  if(dependency != NULL) {
    while (*dependency != NULL) {
      if(*dependency == parent)
        return HDL_TRUE;
      dependency++;
    }
  }
  return HDL_FALSE;
}

/* Search dependent devices in initialized list and in init queue */
static hdl_hardware_private_t *_hdl_hw_deep_search_dependent(hdl_hardware_private_t *desc) {
  hdl_hardware_private_t *dependent = desc;
  do {
    desc = linked_list_get_object(hdl_hardware_private_t, 
      linked_list_find_next_no_overlap(linked_list_item(dependent), &_dev_parent_match, (void*)dependent));
    if((desc == NULL) && linked_list_contains(dev_list, linked_list_item(dependent)))
      desc = linked_list_get_object(hdl_hardware_private_t,
        linked_list_find_first(dev_init_queue, &_dev_parent_match, (void*)dependent));
    if(desc != NULL)
      dependent = desc;
  } while (desc != NULL);
  return dependent;
}

static void _hdl_hw_free(hdl_hardware_private_t *desc) {
  /* ??? call dev deinit first time here ??? */
  linked_list_insert_last(&dev_deinit_queue, linked_list_item(desc));
  hdl_hardware_private_t **parent = (hdl_hardware_private_t **)desc->dependencies;
  if(parent != NULL) {
    while(*parent != NULL) {
      (*parent)->dependents--;
      if((*parent)->dependents == 0)
        _hdl_hw_free(*parent);
      parent++;
    }
  }
}

void hdl_hw_kill(hdl_hardware_t *desc) {
  while(hdl_hw_state(desc) >= HDL_HW_INIT_OK) {
    hdl_hardware_private_t * dependent = _hdl_hw_deep_search_dependent((hdl_hardware_private_t *)desc);
    _hdl_hw_free(dependent);
  }
}
