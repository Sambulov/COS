#include "hdl_portable.h"
#include "CodeLib.h"

#define I2C_ERROR_CLEAR_MASK (uint32_t)(I2C_STAT0_SMBALT | I2C_STAT0_SMBTO | I2C_STAT0_PECERR | \
                                        I2C_STAT0_OUERR | I2C_STAT0_AERR | I2C_STAT0_LOSTARB |  \
                                        I2C_STAT0_BERR)   
                                         

typedef struct {
  __IO uint32_t CTL0;            /*!< I2C Control register 0,                Address offset: 0x00 */
  __IO uint32_t CTL1;            /*!< I2C Control register 1,                Address offset: 0x04 */
  __IO uint32_t SADDR0;          /*!< I2C Slave address register 0,          Address offset: 0x08 */
  __IO uint32_t SADDR1;          /*!< I2C Slave address register 1,          Address offset: 0x0C */
  __IO uint32_t DATA;            /*!< I2C Transfer buffer register,          Address offset: 0x10 */
  __IO uint32_t STAT0;           /*!< I2C Transfer status register 0,        Address offset: 0x14 */
  __IO uint32_t STAT1;           /*!< I2C Transfer status register 1,        Address offset: 0x18 */
  __IO uint32_t CKCFG;           /*!< I2C Clock configure register,          Address offset: 0x1C */
  __IO uint32_t RT;              /*!< I2C Rise time register,                Address offset: 0x20*/
  __IO uint32_t _dummy24_7C[23]; 
  __IO uint32_t SAMCS;           /*!< I2C SAM control and status register,   Address offset: 0x80 */
  __IO uint32_t _dummy84_8C[3];
  __IO uint32_t FMPCFG;          /*!< I2C Fast mode plus configure register, Address offset: 0x90 */
} i2c_periph_t;

typedef struct {
  hdl_module_t module;
  hdl_i2c_server_config_t *config;
  /* private */
  hdl_delegate_t ev_isr;
  hdl_delegate_t er_isr;
  coroutine_desc_static_t i2c_server_task_buff;
  hdl_transceiver_t *transceiver;
} hdl_i2c_server_private_t;



//_Static_assert(sizeof(hdl_i2c_server_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_CLIENT_PRV_SIZE");

static void _i2c_clear_error(i2c_periph_t *i2c_periph) {
  i2c_periph->STAT0 &= ~(I2C_ERROR_CLEAR_MASK);
}

static void event_i2c_ev_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_server_private_t *i2c = (hdl_i2c_server_private_t *)context;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  uint32_t tmp = i2c_periph->STAT0;
  uint32_t tmp1 = 0;
  uint8_t data = 0;
  /* START condition */
  if(tmp & I2C_STAT0_SBSEND) { 
    i2c_periph->CTL0 |= (I2C_CTL0_ACKEN);
  }
  if(tmp & I2C_STAT0_ADDSEND) {
    /* Clear bit ADDSEND*/
    tmp1 = i2c_periph->STAT1;
  }
  /* Read from slave */
  if(tmp & I2C_STAT0_TBE) {
    if((i2c->transceiver != NULL) && (i2c->transceiver->tx_empty != NULL)) {
        i2c->transceiver->tx_empty(i2c->transceiver->proto_context, &data, 1);
        i2c_periph->DATA = data;
    }
  }
  /* Write to slave */
  if(tmp & I2C_STAT0_RBNE) {
    data = i2c_periph->DATA;
    if((i2c->transceiver != NULL) && (i2c->transceiver->rx_data != NULL) && (i2c->transceiver->rx_available != NULL)) {
        i2c->transceiver->rx_data(i2c->transceiver->proto_context, &data, 1);
        i2c_periph->DATA = data;
        uint16_t bytes_available = i2c->transceiver->rx_available(i2c->transceiver->proto_context);
        if(bytes_available <= 1)
          i2c_periph->CTL0 &= ~(I2C_CTL0_ACKEN);
    }
  }
  /* STOP condition */
  if(tmp & I2C_STAT0_STPDET) { 
    if((i2c->transceiver != NULL) && (i2c->transceiver->end_of_transmission != NULL))
      i2c->transceiver->end_of_transmission(i2c->transceiver->proto_context);
    i2c_periph->CTL0 |= (I2C_CTL0_ACKEN);
  }
}

static void event_i2c_er_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_server_private_t *i2c = (hdl_i2c_server_private_t *)context;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  _i2c_clear_error(i2c_periph);
}

static uint8_t _i2c_server_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  hdl_i2c_server_private_t *i2c = (hdl_i2c_server_private_t *) arg;

  //todo
}

hdl_module_state_t hdl_i2c_server(void *i2c, uint8_t enable) {
  hdl_i2c_server_private_t *_i2c = (hdl_i2c_server_private_t *)i2c;
  rcu_periph_enum rcu;
  switch ((uint32_t)_i2c->module.reg) {
    case I2C0:
      rcu = RCU_I2C0;
      break;
    case I2C1:
      rcu = RCU_I2C1;
      break;  
    default:
      break;
  }
  i2c_deinit((uint32_t)_i2c->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    i2c_clock_config((uint32_t)_i2c->module.reg, _i2c->config->speed, _i2c->config->dtcy);
    i2c_mode_addr_config((uint32_t)_i2c->module.reg, I2C_I2CMODE_ENABLE, 
      (_i2c->config->addr_10_bits? I2C_ADDFORMAT_10BITS: I2C_ADDFORMAT_7BITS), _i2c->config->addr0);
    i2c_ackpos_config((uint32_t)_i2c->module.reg, I2C_ACKPOS_CURRENT);
    I2C_SADDR1((uint32_t)_i2c->module.reg) = (_i2c->config->addr1 & 0xFE);
    I2C_SADDR1((uint32_t)_i2c->module.reg) |= (_i2c->config->dual_address? I2C_SADDR1_DUADEN : 0);
    I2C_CTL0((uint32_t)_i2c->module.reg) |= 
      (_i2c->config->general_call_enable? I2C_GCEN_ENABLE : I2C_GCEN_DISABLE) |
      (_i2c->config->stretch_enable? I2C_SCLSTRETCH_ENABLE : I2C_SCLSTRETCH_DISABLE) |
      I2C_I2CMODE_ENABLE;   
    _i2c->ev_isr.context = i2c;
    _i2c->ev_isr.handler = &event_i2c_ev_isr;
    _i2c->er_isr.context = i2c;
    _i2c->er_isr.handler = &event_i2c_er_isr; 
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)_i2c->module.dependencies[3];
    hdl_interrupt_request(ic, _i2c->config->err_interrupt, &_i2c->er_isr);
    hdl_interrupt_request(ic, _i2c->config->ev_interrupt, &_i2c->ev_isr);
    I2C_CTL1((uint32_t)_i2c->module.reg) |= (I2C_CTL1_BUFIE | I2C_CTL1_EVIE | I2C_CTL1_ERRIE);
    I2C_CTL0((uint32_t)_i2c->module.reg) |= I2C_CTL0_I2CEN;
    i2c_ack_config(((uint32_t)_i2c->module.reg), I2C_ACK_ENABLE);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&_i2c->i2c_server_task_buff);
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

void hdl_i2c_server_set_transceiver(hdl_i2c_server_t *i2c, hdl_transceiver_t *transceiver) {
  if(i2c != NULL) {
    ((hdl_i2c_server_private_t*)i2c)->transceiver = transceiver;
  }
}
