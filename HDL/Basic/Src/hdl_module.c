#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* public */
  hdl_module_initializer_t init;
  hdl_module_t **dependencies;
  void *reg;
  /* private */
  __linked_list_object__
  uint32_t dependents;
} hdl_module_private_t;

hdl_module_state_t hdl_null_module_init(void *desc, const uint8_t enable) {
  if(enable)
    return HDL_MODULE_ACTIVE;
  return HDL_MODULE_UNLOADED;
}

hdl_module_t hdl_null_module = {
  .init = &hdl_null_module_init,
};

_Static_assert(sizeof(hdl_module_private_t) == sizeof(hdl_module_t), "In hdl_hw.h data structure size of hdl_module_t doesn't match, check HDL_MODULE_DESC_PRIVATE_SIZE");

static linked_list_t _mod_load = NULL;
static linked_list_t _mod_active = NULL;
static linked_list_t _mod_unload = NULL;

static uint8_t _hdl_module_work(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_module_state_t res;
  hdl_module_private_t *dev = linked_list_get_object(hdl_module_private_t, _mod_unload);
  if(dev != NULL) {
    res = HDL_MODULE_UNLOADED;
    if(dev->init != NULL)
      res = dev->init(dev, HDL_FALSE);
    if(res == HDL_MODULE_UNLOADED)
      linked_list_unlink(linked_list_item(dev));
  }
  dev = linked_list_get_object(hdl_module_private_t, _mod_load);
  if(dev != NULL) {
    res = HDL_MODULE_ACTIVE;
    if(dev->init != NULL)
      res = dev->init(dev, HDL_TRUE);
    switch (res) {
      case HDL_MODULE_ACTIVE:
        linked_list_insert_last(&_mod_active, linked_list_item(dev));
        break;
      case HDL_MODULE_LOADING:
        break;
      default:
        hdl_kill((hdl_module_t *)dev);
        break;
    }
  }
  return cancel;
}

hdl_module_state_t hdl_state(const hdl_module_t *desc) {
  if(desc != NULL) {
    hdl_module_private_t *module = (hdl_module_private_t *)desc;
    return linked_list_contains(_mod_active, linked_list_item(module))? HDL_MODULE_ACTIVE:
           linked_list_contains(_mod_load, linked_list_item(module))? HDL_MODULE_LOADING:
           linked_list_contains(_mod_unload, linked_list_item(module))? HDL_MODULE_UNLOADING:
           HDL_MODULE_FAULT;
  }
  return HDL_MODULE_FAULT;
}

static void _hdl_hw_enable_parents(hdl_module_t *desc) {
  if(desc->dependencies != NULL) {
    hdl_module_t **dependency = desc->dependencies;
    while(*dependency != NULL) {
      hdl_enable(*dependency);
      dependency++;
    }
  }
}

void hdl_enable(hdl_module_t *desc) {
  static coroutine_t hdl_module_worker;
  hdl_module_private_t *module = (hdl_module_private_t *)desc;
  hdl_module_state_t res = hdl_state(desc);
  if(res < HDL_MODULE_ACTIVE) {
    _hdl_hw_enable_parents(desc);
    module->dependents = 1;
    linked_list_insert_last(&_mod_load, linked_list_item(module));
    coroutine_add(&hdl_module_worker, &_hdl_module_work, NULL);
  }
  else
    module->dependents++; /* check overlap */
}

static uint8_t _dev_parent_match(LinkedListItem_t *item, void *arg) {
  hdl_module_private_t *parent = (hdl_module_private_t *)arg;
  hdl_module_private_t **dependency = (hdl_module_private_t **)linked_list_get_object(hdl_module_private_t, item)->dependencies;
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
static hdl_module_private_t *_hdl_hw_deep_search_dependent(hdl_module_private_t *desc) {
  hdl_module_private_t *dependent = desc;
  do {
    desc = linked_list_get_object(hdl_module_private_t, 
      linked_list_find_next_no_overlap(linked_list_item(dependent), &_dev_parent_match, (void*)dependent));
    if((desc == NULL) && linked_list_contains(_mod_active, linked_list_item(dependent)))
      desc = linked_list_get_object(hdl_module_private_t,
        linked_list_find_first(_mod_load, &_dev_parent_match, (void*)dependent));
    if(desc != NULL)
      dependent = desc;
  } while (desc != NULL);
  return dependent;
}

static void _hdl_hw_free(hdl_module_private_t *desc) {
  linked_list_insert_last(&_mod_unload, linked_list_item(desc));
  hdl_module_private_t **parent = (hdl_module_private_t **)desc->dependencies;
  if(parent != NULL) {
    while(*parent != NULL) {
      (*parent)->dependents--;
      if((*parent)->dependents == 0)
        _hdl_hw_free(*parent);
      parent++;
    }
  }
}

void hdl_kill(hdl_module_t *desc) {
  while(hdl_state(desc) >= HDL_MODULE_ACTIVE) {
    hdl_module_private_t * dependent = _hdl_hw_deep_search_dependent((hdl_module_private_t *)desc);
    _hdl_hw_free(dependent);
  }
}

uint8_t hdl_init_complete() {
  return (_mod_load == NULL)? HDL_TRUE: HDL_FALSE;
}
