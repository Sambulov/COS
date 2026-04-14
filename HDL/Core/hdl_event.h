#ifndef HDL_EVENT_H_
#define HDL_EVENT_H_

typedef event_handler_t hdl_event_handler_t;
typedef delegate_t hdl_delegate_t;
typedef event_subscribe_t hdl_event_subscribe_t;
typedef event_t hdl_event_t;

typedef void (*hdl_event_subscribtion_t)(const void *desc, hdl_delegate_t *delegate);

#define hdl_event_subscribe event_subscribe
#define hdl_event_unsubscribe event_unsubscribe
#define hdl_event_raise event_raise
#define hdl_event_clear event_clear

#endif // HDL_EVENT_H_
