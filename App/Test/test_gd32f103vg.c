#include "app.h"
#include "CodeLib.h"

#if defined ( GD32F103VG )

hdl_core_t core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .flash_latency = WS_WSCNT_2
};

hdl_gpio_port_t gpio_a = {
  .init = &hdl_gpio_port,
  .reg = (void *)GPIOA,
  .dependencies = NULL
};

hdl_gpio_port_t gpio_b = {
  .init = &hdl_gpio_port,
  .reg = (void *)GPIOB,
  .dependencies = NULL
};

hdl_gpio_port_t gpio_c = {
  .init = &hdl_gpio_port,
  .reg = (void *)GPIOC,
  .dependencies = NULL
};

hdl_gpio_port_t gpio_d = {
  .init = &hdl_gpio_port,
  .reg = (void *)GPIOD,
  .dependencies = NULL
};

hdl_gpio_mode_t gpio_mode_output_pp = {
  .ospeed = GPIO_OSPEED_2MHZ,
  .type = GPIO_MODE_OUT_PP
};

hdl_gpio_pin_t gpio_a15 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_a),
  .module.reg = (void *)GPIO_PIN_15,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_c10 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_c),
  .module.reg = (void *)GPIO_PIN_10,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_c11 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_c),
  .module.reg = (void *)GPIO_PIN_11,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_c12 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_c),
  .module.reg = (void *)GPIO_PIN_12,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_1,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_2,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_3,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_5,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d6 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_6,
  .mode = &gpio_mode_output_pp
};

hdl_gpio_pin_t gpio_d7 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&gpio_d),
  .module.reg = (void *)GPIO_PIN_7,
  .mode = &gpio_mode_output_pp
};

hdl_module_t my_module = {
  .init = NULL,
  .dependencies = hdl_module_dependencies(&gpio_a15.module, &gpio_c10.module, &gpio_c11.module, &gpio_c12.module,
                                          &gpio_d0.module, &gpio_d1.module, &gpio_d2.module, &gpio_d3.module, 
                                          &gpio_d4.module, &gpio_d5.module, &gpio_d6.module, &gpio_d7.module),
  .reg = NULL
};

void test() {
  hdl_enable(&my_module);
  
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t i = 0;
  while (1) {
    cooperative_scheduler(false);
    i++;
    if(i == 1000) {
      hdl_gpio_toggle(&gpio_c10);
    }
    if(i == 2000) {
      hdl_gpio_toggle(&gpio_c11);
    }
    if(i == 3000) {
      hdl_gpio_toggle(&gpio_c12);
    }
    if(i == 4000) {
      hdl_gpio_toggle(&gpio_d0);
    }
    if(i == 5000) {
      hdl_gpio_toggle(&gpio_d1);
    }
    if(i == 6000) {
      hdl_gpio_toggle(&gpio_d2);
    }
    if(i == 7000) {
      hdl_gpio_toggle(&gpio_d3);
    }
    if(i == 8000) {
      hdl_gpio_toggle(&gpio_d4);
    }
    if(i == 9000) {
      hdl_gpio_toggle(&gpio_d5);
    }
    if(i == 10000) {
      hdl_gpio_toggle(&gpio_d6);
    }
    if(i == 11000) {
      hdl_gpio_toggle(&gpio_d7);
    }
    if(i == 12000) {
      i = 0;
      hdl_gpio_toggle(&gpio_a15);
    }


  }
}

#endif