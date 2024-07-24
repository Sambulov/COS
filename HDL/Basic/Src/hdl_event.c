#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  hdl_event_handler_t handler;
  void *context;
  __linked_list_object__;
} hdl_delegate_private_t;

typedef struct {
  linked_list_t delegates;
} hdl_event_private_t;

_Static_assert(sizeof(hdl_delegate_private_t) == sizeof(hdl_delegate_t), "In hdl_event.h data structure size of hdl_delegate_t doesn't match, check HDL_DELEGATE_PRIVATE_SIZE");
_Static_assert(sizeof(hdl_event_private_t) == sizeof(hdl_event_t), "In hdl_event.h data structure size of hdl_event_t doesn't match, check HDL_EVENT_PRIVATE_SIZE");

void hdl_event_subscribe(hdl_event_t *event, hdl_delegate_t *delegate) {
  hdl_event_private_t *evt = (hdl_event_private_t *)event;
  if((event != NULL) && (delegate != NULL))
    linked_list_insert(&(evt->delegates), linked_list_item((hdl_delegate_private_t*)delegate), NULL);
}

void hdl_event_unsubscribe(hdl_delegate_t *delegate) {
  if(delegate != NULL)
    linked_list_unlink(linked_list_item((hdl_delegate_private_t*)delegate));
}

static void _call_delegate(linked_list_item_t *item, void *arg) {
  hdl_delegate_private_t *delegate = linked_list_get_object(hdl_delegate_private_t, item);
  void **args =  (void**)arg;
  delegate->handler(*(uint32_t*)args[0], (args[1]),delegate->context);
}

uint32_t hdl_event_raise(hdl_event_t *event, void *sender, uint32_t event_trigger) {
  hdl_event_private_t *evt = (hdl_event_private_t *)event;
  if(event != NULL)
    return linked_list_do_foreach(evt->delegates, &_call_delegate, (void *)((void*[]){&event_trigger, sender}));
  return 0;
}
