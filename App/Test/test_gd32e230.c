#include "app.h"
#include "CodeLib.h"

#if defined ( GD32E23X )

extern hdl_adc_t mod_adc;
extern hdl_gpio_pin_t mod_gpio_adc_channel_3v3;
extern hdl_gpio_pin_t mod_gpio_adc_channel_1v5;
extern hdl_timer_t mod_timer_ms;

uint16_t adc_raw[2];
void test() {
  static uint32_t time_stamp_ms = 0;

  hdl_enable(&mod_gpio_adc_channel_3v3.module);
  hdl_enable(&mod_gpio_adc_channel_1v5.module);
  hdl_enable(&mod_adc.module);
  hdl_enable(&mod_timer_ms.module);
  
  // while (hdl_state(&mod_adc.module) != HDL_MODULE_INIT_OK ||
  //         hdl_state(&mod_gpio_adc_channel_3v3.module) != HDL_MODULE_INIT_OK ||
  //           hdl_state(&mod_gpio_adc_channel_1v5.module) != HDL_MODULE_INIT_OK)
  // {
  //   cooperative_scheduler(false);
  // }
  
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

;

  while (1) {
    cooperative_scheduler(false);

    if (TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_timer_ms)))
    {
        hdl_adc_start(&mod_adc, adc_raw);
    }
  }

}

#endif
