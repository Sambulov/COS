#include "bldl.h"
#include "CooperativeMultitasking.h"

static void _bldl_uspd_analog_update(hdl_plc_port_t *port) {

}

static void _bldl_uspd_discrete_update(hdl_plc_port_t *port) {

}

static void _bldl_uspd_led_update(hdl_plc_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    hdl_plc_port_descriptor_t hw_unsupported = 
      PLC_IO_PORT_HW_ANALOG | PLC_IO_PORT_HW_PULL_DOWN_WEAK |
      PLC_IO_PORT_HW_PULL_DOWN_STRONG | PLC_IO_PORT_HW_PULL_UP_WEAK |
      PLC_IO_PORT_HW_PULL_UP_STRONG | PLC_IO_PORT_HW_VOLTAGE_DIVIDER |
      PLC_IO_PORT_HW_CURRENT_SHUNT;
    if(port->desc & hw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }

    // TODO: config

    port->desc &= PLC_IO_PORT_OP_RESET_CONFIG;
  }

  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *led = (hdl_gpio_pin_t *)*(port->module.dependencies);
    port->input.ul_value = (uint32_t)hdl_gpio_read(led);
    if(port->input.ul_value != port->output.ul_value) {
      hdl_gpio_write(led, port->output.ul_value);
    }
  }
}

static void _bldl_uspd_set_default(hdl_plc_port_t *port) {
  port->desc = port->default_config->desc;
  port->output = port->default_config->default_output;
  port->option = port->default_config->default_option;
}

static uint8_t _uspd_expander_handler(coroutine_desc_t this, uint8_t cancel, void *arg) {
  hdl_plc_port_t **ports = (hdl_plc_port_t**)arg;
  while ((ports != NULL) && (*ports != NULL)) {
    if((*ports)->module.init == &bldl_uspd_ain_port) {
      _bldl_uspd_analog_update(*ports);
    }
    else if((*ports)->module.init == &bldl_uspd_discrete_port) {
      _bldl_uspd_discrete_update(*ports);
    }
    else if((*ports)->module.init == &bldl_uspd_led_port) {
      _bldl_uspd_led_update(*ports);
    }
  }
  return cancel;
}

hdl_module_state_t bldl_uspd_led_port(void *desc, uint8_t enable) {
  _bldl_uspd_set_default((hdl_plc_port_t *)desc);
  _bldl_uspd_led_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_uspd_discrete_port(void *desc, uint8_t enable) {
  _bldl_uspd_set_default((hdl_plc_port_t *)desc);
  _bldl_uspd_led_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_uspd_ain_port(void *desc, uint8_t enable) {
  _bldl_uspd_set_default((hdl_plc_port_t *)desc);
  _bldl_uspd_analog_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_uspd_port_expander(void *desc, uint8_t enable) {
  coroutine_desc_static_t expander_task;
  if(enable) {
    coroutine_add_static(&expander_task, &_uspd_expander_handler, desc);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&expander_task);
  return HDL_MODULE_DEINIT_OK;
}
