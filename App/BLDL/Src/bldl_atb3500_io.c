#include "bldl.h"
#include "CooperativeMultitasking.h"

#ifdef ATB_3500

static void _bldl_atb3500_set_default(hdl_plc_port_t *port) {
  port->desc = port->default_config->desc | PLC_IO_PORT_OP_RESET_CONFIG;
  port->output = port->default_config->default_output;
  port->option = port->default_config->default_option;
}

static void _bldl_atb3500_analog_update(hdl_plc_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    /* Support only analog mode */
    hdl_plc_port_descriptor_t hw_unsupported = !(port->desc & PLC_IO_PORT_HW_ANALOG) || 
      ((port->desc & PLC_IO_PORT_HW_CONFIG) & ~(PLC_IO_PORT_HW_ANALOG));
    /* TODO: Support only RAW ADC */
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG);
    sw_unsupported = (sw_unsupported != PLC_IO_PORT_SW_VOLTAGE) && (sw_unsupported != PLC_IO_PORT_SW_RAW_ADC);
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= ~PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    uint32_t value = hdl_adc_get_data((hdl_adc_t *)port->module.dependencies[0], (hdl_adc_source_t *)port->module.reg);
    /* TODO: apply convertation */
    port->input = value;
  }
}

static void _bldl_atb3500_di_update(hdl_plc_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    /* Support only INPUT mode */
    hdl_plc_port_descriptor_t hw_unsupported = !(port->desc & PLC_IO_PORT_HW_CONFIG);
    hdl_plc_port_descriptor_t sw_unsupported = (((port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_DISCRETE) &&
        ((port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_VOLTAGE));
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= ~PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *input = (hdl_gpio_pin_t *)port->module.dependencies[0];
    port->input = (uint32_t)hdl_gpio_read(input);
  }
}

static void _bldl_atb3500_do_update(hdl_plc_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    /* Support only DIGITAL OUTOUT mode */
    /* Support only analog mode */
    hdl_plc_port_descriptor_t hw_unsupported = !(port->desc & PLC_IO_PORT_HW_DIRECTION) || 
      ((port->desc & PLC_IO_PORT_HW_CONFIG) & ~(PLC_IO_PORT_HW_DIRECTION));
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_DISCRETE;
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= ~PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *output = (hdl_gpio_pin_t *)port->module.dependencies[0];
    hdl_gpio_write(output, port->output);
    port->input = (uint32_t)hdl_gpio_read(output);

    //hdl_gpio_pin_t *output = (hdl_gpio_pin_t *)port->module.dependencies[1];
    //port->input = (uint32_t)hdl_gpio_read(input);
    //hdl_gpio_write(output, port->output);
  }
}

static void _bldl_atb3500_led_update(bldl_atb3500_led_port_t *port) {
  if(port->desc & PLC_IO_PORT_OP_RESET_CONFIG) {
    /* Support only digital output mode */
    hdl_plc_port_descriptor_t hw_unsupported = !(port->desc & PLC_IO_PORT_HW_DIRECTION) || 
      ((port->desc & PLC_IO_PORT_HW_CONFIG) & ~(PLC_IO_PORT_HW_DIRECTION));
    hdl_plc_port_descriptor_t sw_unsupported = (port->desc & PLC_IO_PORT_SW_CONFIG) != PLC_IO_PORT_SW_DISCRETE;
    if(hw_unsupported || sw_unsupported) {
      port->desc |= PLC_IO_PORT_STATUS_HARD_ERROR;
    }
    port->desc &= ~PLC_IO_PORT_OP_RESET_CONFIG;
  }
  if(!(port->desc & PLC_IO_PORT_STATUS_HARD_ERROR)) {
    hdl_gpio_pin_t *led = (hdl_gpio_pin_t *)*(port->module.dependencies);
    hdl_gpio_write(led, port->output);
    port->input = (uint32_t)hdl_gpio_read(led);

  }
}

static uint8_t _atb3500_expander_handler(coroutine_desc_t this, uint8_t cancel, void *arg) {
  bldl_atb3500_port_expander_t *exp = (bldl_atb3500_port_expander_t *)arg;
  hdl_plc_port_t **ports = (hdl_plc_port_t **)exp->dependencies;
  while ((ports != NULL) && (*ports != NULL)) {
    if((*ports)->module.init == &bldl_atb3500_ain_port) {
      _bldl_atb3500_analog_update(*ports);
    }
    else if((*ports)->module.init == &bldl_atb3500_di_port) {
      _bldl_atb3500_di_update(*ports);
    }
    else if((*ports)->module.init == &bldl_atb3500_do_port) {
      _bldl_atb3500_do_update(*ports);
    }
    else if((*ports)->module.init == &bldl_atb3500_led_port) {
      _bldl_atb3500_led_update(*ports);
    }

    
    ports++;
  }
  return cancel;
}

hdl_module_state_t bldl_atb3500_led_port(void *desc, uint8_t enable) {
  _bldl_atb3500_set_default((hdl_plc_port_t *)desc);
  _bldl_atb3500_led_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_atb3500_do_port(void *desc, uint8_t enable) {
  _bldl_atb3500_set_default((hdl_plc_port_t *)desc);
  _bldl_atb3500_do_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_atb3500_di_port(void *desc, uint8_t enable) {
  _bldl_atb3500_set_default((hdl_plc_port_t *)desc);
  _bldl_atb3500_di_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_atb3500_ain_port(void *desc, uint8_t enable) {
  _bldl_atb3500_set_default((hdl_plc_port_t *)desc);
  _bldl_atb3500_analog_update((hdl_plc_port_t *)desc);
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_atb3500_port_expander(void *desc, uint8_t enable) {
  static coroutine_desc_static_t expander_task;
  if(enable) {
    coroutine_add_static(&expander_task, &_atb3500_expander_handler, desc);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&expander_task);
  return HDL_MODULE_DEINIT_OK;
}

#endif