#ifndef BUTTON_H_
#define BUTTON_H_

#include "hdl.h"

typedef struct {
  uint32_t debounce_delay;
  uint32_t long_clic_time;
  void (*on_button_down)(void *sender);
  void (*on_button_up)(void *sender);
  void (*on_button_click)(void *sender);
  void (*on_button_long_click)(void *sender);
} hdl_button_t;

#endif // BUTTON_H_
