#ifndef HDL_CALLBACK_H_
#define HDL_CALLBACK_H_

#define HDL_DELEGATE_PRIVATE_SIZE    16
#define HDL_EVENT_PRIVATE_SIZE        4

typedef void (*hdl_event_handler_t)(uint32_t event_trigger, void *sender, void *context);

typedef struct {
  hdl_event_handler_t handler;
  void *context;
  PRIVATE(hdl, HDL_DELEGATE_PRIVATE_SIZE);
} hdl_delegate_t;

typedef struct {
  PRIVATE(hdl, HDL_EVENT_PRIVATE_SIZE);
}hdl_event_t;

void hdl_event_subscribe(hdl_event_t *event, hdl_delegate_t *delegate);
void hdl_event_unsubscribe(hdl_delegate_t *delegate);
uint32_t hdl_event_raise(hdl_event_t *event, void *sender, uint32_t event_trigger);

#endif // HDL_CALLBACK_H_
