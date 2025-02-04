#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_base_t const *mod;
  const void *owner;
  uint32_t dependents;
  __linked_list_object__
} hdl_module_var_t;

hdl_module_base_t hdl_null_module = {
  .iface = NULL,
  .dependencies = NULL,
  .config = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

HDL_ASSERRT_STRUCTURE_CAST(hdl_module_var_t, *((hdl_module_base_t *)0)->mod_var, HDL_MODULE_VAR_SIZE, hdl_module.h);

static linked_list_t _mod_load = NULL;
static linked_list_t _mod_active = NULL;
static linked_list_t _mod_unload = NULL;

static uint8_t _hdl_module_work(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)arg;
  hdl_module_state_t res;
  hdl_module_var_t *mod_var = linked_list_get_object(hdl_module_var_t, _mod_unload);
  if(mod_var != NULL) {
    const hdl_module_base_iface_t *iface = mod_var->mod->iface;
    res = HDL_MODULE_UNLOADED;
    if((iface != NULL) && (iface->init != NULL))
      res = iface->init(mod_var->mod, HDL_FALSE);
    if(res == HDL_MODULE_UNLOADED)
      linked_list_unlink(linked_list_item(mod_var));
  }
  mod_var = linked_list_get_object(hdl_module_var_t, _mod_load);
  if(mod_var != NULL) {
    const hdl_module_base_iface_t *iface = mod_var->mod->iface;
    res = HDL_MODULE_ACTIVE;
    if((iface != NULL) && (iface->init != NULL))
      res = iface->init(mod_var->mod, HDL_TRUE);
    switch (res) {
      case HDL_MODULE_ACTIVE:
        linked_list_insert_last(&_mod_active, linked_list_item(mod_var));
        break;
      case HDL_MODULE_LOADING:
        break;
      default:
        hdl_kill(mod_var->mod);
        break;
    }
  }
  return cancel;
}

hdl_module_state_t hdl_state(const void *desc) {
  if(desc != NULL) {
    hdl_module_var_t *module_var = (hdl_module_var_t *)((hdl_module_base_t *)desc)->mod_var;
    return linked_list_contains(_mod_active, linked_list_item(module_var))? HDL_MODULE_ACTIVE:
           linked_list_contains(_mod_load, linked_list_item(module_var))? HDL_MODULE_LOADING:
           linked_list_contains(_mod_unload, linked_list_item(module_var))? HDL_MODULE_UNLOADING:
           HDL_MODULE_FAULT;
  }
  return HDL_MODULE_FAULT;
}

static void _hdl_hw_enable_parents(hdl_module_base_t *desc) {
  if(desc->dependencies != NULL) {
    hdl_module_base_t * const *dependency = (hdl_module_base_t **)desc->dependencies;
    while(*dependency != NULL) {
      hdl_enable(*dependency);
      dependency++;
    }
  }
}

void hdl_enable(const void *desc) {
  static coroutine_t hdl_module_worker;
  hdl_module_var_t *module_var = (hdl_module_var_t *)((hdl_module_base_t *)desc)->mod_var;
  hdl_module_state_t res = hdl_state(desc);
  if(res < HDL_MODULE_ACTIVE) {
    hdl_module_base_t *mod = (hdl_module_base_t *)desc;
    ((hdl_module_var_t *)mod->mod_var)->mod = mod;
    _hdl_hw_enable_parents(mod);
    module_var->dependents = 1;
    linked_list_insert_last(&_mod_load, linked_list_item(module_var));
    coroutine_add(&hdl_module_worker, &_hdl_module_work, NULL);
  }
  else
    module_var->dependents++; /* check overlap */
}

static uint8_t _dev_parent_match(LinkedListItem_t *item, void *arg) {
  const hdl_module_base_t *parent = ((hdl_module_var_t *)arg)->mod;
  hdl_module_base_t **dependency = (hdl_module_base_t **)linked_list_get_object(hdl_module_var_t, item)->mod->dependencies;
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
static hdl_module_var_t *_hdl_hw_deep_search_dependent(hdl_module_var_t *mod) {
  hdl_module_var_t *dependent = mod;
  do {
    mod = linked_list_get_object(hdl_module_var_t, 
      linked_list_find_next_no_overlap(linked_list_item(dependent), &_dev_parent_match, (void*)dependent));
    if((mod == NULL) && linked_list_contains(_mod_active, linked_list_item(dependent)))
      mod = linked_list_get_object(hdl_module_var_t,
        linked_list_find_first(_mod_load, &_dev_parent_match, (void*)dependent));
    if(mod != NULL)
      dependent = mod;
  } while (mod != NULL);
  return dependent;
}

static void _hdl_hw_free(hdl_module_var_t *mod_var) {
  linked_list_insert_last(&_mod_unload, linked_list_item(mod_var));
  hdl_module_base_t **parent = (hdl_module_base_t **)mod_var->mod->dependencies;
  if(parent != NULL) {
    hdl_module_var_t *parent_var = (hdl_module_var_t *)(*parent)->mod_var;
    while(*parent != NULL) {
      parent_var->dependents--;
      if(parent_var->dependents == 0)
        _hdl_hw_free(parent_var);
      parent++;
    }
  }
}

void hdl_kill(const void *desc) {
  while(hdl_state(desc) >= HDL_MODULE_ACTIVE) {
    hdl_module_var_t *dependent = _hdl_hw_deep_search_dependent((hdl_module_var_t *)((hdl_module_base_t *)desc)->mod_var);
    _hdl_hw_free(dependent);
  }
}

uint8_t hdl_init_complete() {
  return (_mod_load == NULL)? HDL_TRUE: HDL_FALSE;
}

uint8_t hdl_take(const void *desc, const void *owner) {
  hdl_module_var_t *module_var = (hdl_module_var_t *)((hdl_module_base_t *)desc)->mod_var;
  if(module_var->owner == NULL) {
    module_var->owner = owner;
  }
  return (module_var->owner == owner);
}

uint8_t hdl_give(const void *desc, const void *owner) {
  hdl_module_var_t *module_var = (hdl_module_var_t *)((hdl_module_base_t *)desc)->mod_var;
  if(module_var->owner == owner) {
    module_var->owner = NULL;
  }
  return (module_var->owner == NULL);
}
