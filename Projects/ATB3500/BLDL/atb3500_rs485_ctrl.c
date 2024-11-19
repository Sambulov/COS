#include "app.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_t module;
  atb3500_rs485_ctrl_config_t *config;
  struct {
    uint32_t timer_period;
    hdl_delegate_t clear_rts_delegate;
    hdl_delegate_t set_rts_delegate;
    atb3500_rs485_ctrl_proto_tx_t tx_data;
  } private;
} atb3500_rs485_ctrl_private_t;

HDL_ASSERRT_STRUCTURE_CAST(atb3500_rs485_ctrl_private_t, atb3500_rs485_ctrl_t, ATB3500_RS485_CTRL_PRV_SIZE, atb3500_rs485_ctrl.h);

static void _clear_rts(uint32_t event_trigger, void *sender, void *context) {
    atb3500_rs485_ctrl_private_t *rs_ctl = (atb3500_rs485_ctrl_private_t*)context;    
    if(event_trigger & rs_ctl->config->counter_trigger) {
        hdl_tick_counter_t *counter = (hdl_tick_counter_t *)rs_ctl->module.dependencies[0];
        hdl_gpio_pin_t *do_pin = (hdl_gpio_pin_t *)rs_ctl->module.dependencies[3];
        hdl_gpio_set_inactive(do_pin);
        hdl_tick_counter_stop(counter);
    }
}

static void _set_rts(uint32_t event_trigger, void *sender, void *context) {
    atb3500_rs485_ctrl_private_t *rs_ctl = (atb3500_rs485_ctrl_private_t*)context;
	if(event_trigger & rs_ctl->config->pin_trigger) {
        hdl_tick_counter_t *counter = (hdl_tick_counter_t *)rs_ctl->module.dependencies[0];
        hdl_gpio_pin_t *do_pin = (hdl_gpio_pin_t *)rs_ctl->module.dependencies[3];
        hdl_gpio_set_active(do_pin);
        hdl_tick_counter_set(counter, 0, rs_ctl->private.timer_period);
	}
}

hdl_module_state_t atb3500_rs485_ctrl(void *desc, uint8_t enable) {
    if(enable) {
        atb3500_rs485_ctrl_private_t *rs_ctl = (atb3500_rs485_ctrl_private_t*)desc;
        hdl_tick_counter_t *counter = (hdl_tick_counter_t *)rs_ctl->module.dependencies[0];
        hdl_tick_counter_stop(counter);
        hdl_interrupt_controller_t *int_ctrlr = (hdl_interrupt_controller_t *)rs_ctl->module.dependencies[1];
        {
            hdl_clock_t *counter_clk = (hdl_clock_t *)counter->module.dependencies[0];
            hdl_clock_freq_t clk_freq;
            hdl_get_clock(counter_clk, &clk_freq);
            rs_ctl->private.timer_period = (clk_freq.num * rs_ctl->config->trunsfer_unit_size) / (clk_freq.denom * rs_ctl->config->boud_rate_default);
            rs_ctl->private.tx_data.xfer_unit_size = rs_ctl->config->trunsfer_unit_size;
            rs_ctl->private.tx_data.boud = rs_ctl->config->boud_rate_default;
            rs_ctl->private.tx_data.ovn = ATB3500_RS485_CTRL_OVN;
        }
        rs_ctl->private.clear_rts_delegate.handler = &_clear_rts;
        rs_ctl->private.clear_rts_delegate.context = desc;
        hdl_event_subscribe(&rs_ctl->config->counter_reload_int->event, &rs_ctl->private.clear_rts_delegate);

        rs_ctl->private.set_rts_delegate.handler = &_set_rts;
        rs_ctl->private.set_rts_delegate.context = desc;
        hdl_event_subscribe(&rs_ctl->config->tx_pin_int->event, &rs_ctl->private.set_rts_delegate);

        if(hdl_interrupt_request(int_ctrlr, rs_ctl->config->counter_reload_int) &&
           hdl_interrupt_request(int_ctrlr, rs_ctl->config->tx_pin_int)) 
            return HDL_MODULE_ACTIVE;
    }
    return HDL_MODULE_UNLOADED;
}

atb3500_rs485_ctrl_proto_tx_t *atb3500_rs485_ctrl_update(atb3500_rs485_ctrl_t *desc, atb3500_rs485_ctrl_proto_rx_t *rx_data) {
    atb3500_rs485_ctrl_private_t *rs_ctl = (atb3500_rs485_ctrl_private_t*)desc;
    if((rx_data->magic == ATB3500_RS485_CTRL_MAGIC) && (rx_data->boud > 0)) {
        hdl_tick_counter_t *counter = (hdl_tick_counter_t *)rs_ctl->module.dependencies[0];
        hdl_clock_t *counter_clk = (hdl_clock_t *)counter->module.dependencies[0];
        hdl_clock_freq_t clk_freq;
        hdl_get_clock(counter_clk, &clk_freq);
        rs_ctl->private.tx_data.xfer_unit_size = rx_data->xfer_unit_size;
        rs_ctl->private.tx_data.boud = rx_data->boud;
        rs_ctl->private.timer_period = (clk_freq.num * rx_data->xfer_unit_size) / (clk_freq.denom * rx_data->boud);
    }
    return &rs_ctl->private.tx_data;
}
