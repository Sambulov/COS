#include "bldl.h"
#include "CooperativeMultitasking.h"

#ifdef USPD

static void _bldl_uspd_set_default(hdl_plc_port_t *port) {
  port->desc = port->default_config->desc | PLC_IO_PORT_OP_RESET_CONFIG;
  port->output = port->default_config->default_output;
  port->option = port->default_config->default_option;
}

static void _bldl_uspd_analog_update(hdl_plc_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    hdl_plc_port_descriptor_t hw_unsupported = port->desc &
      (PLC_IO_PORT_HW_PULL_DOWN_STRONG | PLC_IO_PORT_HW_PULL_UP_WEAK |
      PLC_IO_PORT_HW_VOLTAGE_DIVIDER);
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG);
    sw_unsupported = (sw_unsupported != PLC_IO_PORT_SW_VOLTAGE) && (sw_unsupported != PLC_IO_PORT_SW_CURRENT);
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *pullup = (hdl_gpio_pin_t *)port->module.dependencies[2];
    hdl_gpio_pin_t *pulldown = (hdl_gpio_pin_t *)port->module.dependencies[3];
    hdl_gpio_pin_t *shunt = (hdl_gpio_pin_t *)port->module.dependencies[4];
    hdl_gpio_write(pullup, (port->desc & PLC_IO_PORT_HW_PULL_UP_STRONG)? !pullup->inactive_default: pullup->inactive_default);
    hdl_gpio_write(pulldown, (port->desc & PLC_IO_PORT_HW_PULL_DOWN_WEAK)? !pulldown->inactive_default: pullup->inactive_default);
    hdl_gpio_write(shunt, (port->desc & PLC_IO_PORT_HW_CURRENT_SHUNT)? !shunt->inactive_default: pullup->inactive_default);
    uint32_t value = hdl_adc_get_data((hdl_adc_t *)port->module.dependencies[0], (hdl_adc_source_t *)port->module.reg);
    /* TODO: apply convertation */
    port->input = value;
  }
}

static void _bldl_uspd_discrete_update(bldl_uspd_discrete_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    hdl_plc_port_descriptor_t hw_unsupported = 
      PLC_IO_PORT_HW_ANALOG | PLC_IO_PORT_HW_PULL_DOWN_WEAK |
      PLC_IO_PORT_HW_PULL_DOWN_STRONG | PLC_IO_PORT_HW_PULL_UP_WEAK |
      PLC_IO_PORT_HW_PULL_UP_STRONG | PLC_IO_PORT_HW_VOLTAGE_DIVIDER |
      PLC_IO_PORT_HW_CURRENT_SHUNT;
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_DISCRETE;
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *input = (hdl_gpio_pin_t *)port->module.dependencies[0];
    hdl_gpio_pin_t *output = (hdl_gpio_pin_t *)port->module.dependencies[1];
    port->input = (uint32_t)hdl_gpio_read(input);
    hdl_gpio_write(output, port->output);
  }
}

static void _bldl_uspd_led_update(bldl_uspd_led_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    hdl_plc_port_descriptor_t hw_unsupported = 
      PLC_IO_PORT_HW_ANALOG | PLC_IO_PORT_HW_PULL_DOWN_WEAK |
      PLC_IO_PORT_HW_PULL_DOWN_STRONG | PLC_IO_PORT_HW_PULL_UP_WEAK |
      PLC_IO_PORT_HW_PULL_UP_STRONG | PLC_IO_PORT_HW_VOLTAGE_DIVIDER |
      PLC_IO_PORT_HW_CURRENT_SHUNT;
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_DISCRETE;
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *led = (hdl_gpio_pin_t *)*(port->module.dependencies);
    port->input = hdl_gpio_read(led);
    hdl_gpio_write(led, port->output);
  }
}

static uint8_t _uspd_expander_handler(coroutine_desc_t this, uint8_t cancel, void *arg) {
  bldl_uspd_port_expander_t *exp = (bldl_uspd_port_expander_t *)arg;
  hdl_plc_port_t **ports = (hdl_plc_port_t **)exp->dependencies;
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
    ports++;
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
  _bldl_uspd_discrete_update((hdl_plc_port_t *)desc);
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
  static coroutine_desc_static_t expander_task;
  if(enable) {
    coroutine_add_static(&expander_task, &_uspd_expander_handler, desc);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&expander_task);
  return HDL_MODULE_DEINIT_OK;
}

#endif