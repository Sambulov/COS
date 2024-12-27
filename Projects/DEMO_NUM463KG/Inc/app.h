#ifndef APP_H_
#define APP_H_

#include "hdl.h"
#include "mig.h"
#include "som_link.h"

extern hdl_timer_t led_y_timer;
extern hdl_timer_t led_g_timer;
extern hdl_button_t btn0;
extern hdl_button_t btn1;

#define APP_R_MEM_MAP_SIZE    1
#define APP_W_MEM_MAP_SIZE    1

void main(void);

#endif /* APP_H_ */
