#ifndef HDL_CALLBACK_H_
#define HDL_CALLBACK_H_

typedef struct {
  void (*handler)(void *sender, void *context);
  void *context;
} hdl_callback_t;

#endif // HDL_CALLBACK_H_
