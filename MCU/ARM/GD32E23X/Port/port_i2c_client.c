// void hdl_hal_i2c_deinit(hdl_i2c_client_t *desc) {
//   if((desc == NULL || desc->hw_conf == NULL))
//     return;
//   /* reset config */
//   hdl_gpio_deinit(desc->hw_conf->scl);
//   hdl_gpio_deinit(desc->hw_conf->sda);
//   i2c_deinit(desc->hw_conf->periph);
// }

// int32_t hdl_i2c(hdl_i2c_client_t *desc, uint8_t enable) {
//   if((desc == NULL) || (desc->hw_conf == NULL))
//     return;
//   /* reset config */
//   hdl_hal_i2c_deinit(desc);

//   if((desc->hw_conf->mode & 0x03) == 0x00) { /* if disabled mode */
//     return;
//   }
//   /* init gpio */
//   hdl_gpio_init(desc->hw_conf->scl);
//   hdl_gpio_init(desc->hw_conf->sda);
//   hdl_gpio_write(desc->hw_conf->scl, HDL_GPIO_HIGH);
//   hdl_gpio_write(desc->hw_conf->sda, HDL_GPIO_HIGH);

//   switch (desc->hw_conf->periph) {
//     case I2C0:
//       rcu_periph_clock_enable(RCU_I2C0);
//       break;
//     case I2C1:
//       rcu_periph_clock_enable(RCU_I2C1);
//       break;
//     default:
//       return HDL_FALSE;
//   }
//   /* configure I2C clock */
//   i2c_clock_config(desc->hw_conf->periph, desc->hw_conf->speed, desc->hw_conf->dtcy);

//   i2c_slave_response_to_gcall_config(desc->hw_conf->periph, I2C_GCEN_DISABLE);
//   i2c_mode_addr_config(desc->hw_conf->periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0); /* general call address */
//   i2c_dualaddr_disable(desc->hw_conf->periph);

//   /* if server mode enabled */
//   if(desc->hw_conf->mode & 2) {
//     /* configure I2C address */
//     i2c_mode_addr_config(desc->hw_conf->periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, desc->hw_conf->addr << 1);
//     if(desc->hw_conf->general_call)
//       i2c_slave_response_to_gcall_config(desc->hw_conf->periph, I2C_GCEN_ENABLE);
//   }
//   i2c_enable(desc->hw_conf->periph);
//   if(!desc->hw_conf->mode & 1) { /* if i2c client reset bus */
//     i2c_stop_on_bus(desc->hw_conf->periph);
//   }
// }

// #define is_timeout(start_tick, timeout) ((hdl_millis() - start_tick) >= timeout)

// void hdl_i2c_client_xfer_message(hdl_i2c_client_t *desc, hdl_i2c_message_t *msg) {

// }



///===================================================================================================================

// #include "hdl_portable.h"
// #include "CodeLib.h"

// typedef enum {
//   HDL_I2C_MESSAGE_START_ON_BUS   = 0x01, /* Generate start condition */
//   HDL_I2C_MESSAGE_ADDR_HIT       = 0x02, /* Send addr */
//   HDL_I2C_MESSAGE_MRSW_SET       = 0x04, /* Receiver mode if option set, else trasmitter mode */
//   HDL_I2C_MESSAGE_NACK_ON_BUS    = 0x08, /* NACK sent */
//   HDL_I2C_MESSAGE_STOP_ON_BUS    = 0x10, /* Stop condition generated */
// } hdl_i2c_message_state_t;

// typedef struct {
//   /* private */
//   PRIVATE(hdl, HDL_I2C_MESSAGE_PRV_SIZE);
//   uint16_t transfered;
//   hdl_i2c_message_state_t state;
//   uint32_t ovn;
//   /* public */
//   uint16_t address;
//   uint8_t *buffer;
//   uint16_t buffer_size;
//   hdl_i2c_message_options_t options;
// } hdl_i2c_message_private_t;

// typedef struct {
//   hdl_module_t module;
//   const hdl_i2c_client_phy_t *hw_conf;
//   //PRIVATE(hdl, HDL_I2C_PRV_SIZE);
//   __linked_list_object__;
//   hdl_i2c_message_private_t *current_msg;
//   hdl_delegate_t ev_isr;
//   hdl_delegate_t er_isr;
// } hdl_i2c_client_private_t;

// // _Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
// // _Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_PRV_SIZE");

// #define HDL_MESSAGE_OVN   ((uint32_t)(0xFD357DAB))

// void hdl_i2c_hw_client_xfer_message(hdl_i2c_client_phy_t *i2c, hdl_i2c_message_t *msg) {
//   hdl_i2c_message_private_t *message = (hdl_i2c_message_private_t *)msg;
//   hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
//   if((message != NULL) && (_i2c->current_msg == NULL)) {
//     message->state = 0;
//     message->transfered = 0;
//     message->ovn = HDL_MESSAGE_OVN;
//     _i2c->current_msg = message;
//     //TODO: prepare hw
//   }
// }

// hdl_i2c_message_state_t hdl_i2c_client_message_state(hdl_i2c_message_t *msg) {
//   hdl_i2c_message_private_t *mess = (hdl_i2c_message_private_t *)msg;
//   if((mess != NULL) && (mess->ovn == HDL_MESSAGE_OVN))
//     return ((hdl_i2c_message_private_t *)msg)->state;
//   return 0;
// }

// uint16_t hdl_i2c_client_message_get_transfered(hdl_i2c_message_t *msg) {
//   hdl_i2c_message_private_t *mess = (hdl_i2c_message_private_t *)msg;
//   if((mess != NULL) && (mess->ovn == HDL_MESSAGE_OVN))
//     return ((hdl_i2c_message_private_t *)msg)->transfered;
//   return 0;
// }



// static uint8_t _i2c_client_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   linked_list_t i2cs = (linked_list_t)arg;
//   linked_list_do_foreach(i2cs, &_i2c_client_handler, NULL);
//   return cancel;
// }

// hdl_module_state_t hdl_i2c_hw(void *i2c, uint8_t enable) {
//   static coroutine_desc_static_t i2c_client_task_buff;
//   static linked_list_t i2cs = NULL;
//   hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
//   rcu_periph_enum rcu;
//   switch ((uint32_t)_i2c->module.reg) {
//     case I2C0:
//       rcu = RCU_I2C0;
//       break;
//     case I2C1:
//       rcu = RCU_I2C1;
//       break;  
//     default:
//       break;
//   }
//   i2c_deinit((uint32_t)_i2c->module.reg);
//   if(enable) {
//     linked_list_insert_last(&i2cs, linked_list_item(_i2c));
//     rcu_periph_clock_enable(rcu);
//     i2c_clock_config((uint32_t)_i2c->module.reg, _i2c->hw_conf->speed, _i2c->hw_conf->dtcy);
//     i2c_mode_addr_config((uint32_t)_i2c->module.reg, I2C_I2CMODE_ENABLE, 
//       (_i2c->hw_conf->addr_10_bits? I2C_ADDFORMAT_10BITS: I2C_ADDFORMAT_7BITS), _i2c->hw_conf->addr0);
//     I2C_SADDR1((uint32_t)_i2c->module.reg) = (_i2c->hw_conf->addr1 & 0xFE);
//     I2C_SADDR1((uint32_t)_i2c->module.reg) |= (_i2c->hw_conf->dual_address? I2C_SADDR1_DUADEN : 0);
//     I2C_CTL0((uint32_t)_i2c->module.reg) |= 
//       (_i2c->hw_conf->general_call_enable? I2C_GCEN_ENABLE : I2C_GCEN_DISABLE) |
//       (_i2c->hw_conf->stretch_enable? I2C_SCLSTRETCH_ENABLE : I2C_SCLSTRETCH_DISABLE) |
//       I2C_I2CMODE_ENABLE;
    
//     _i2c->ev_isr.context = i2c;
//     _i2c->ev_isr.handler = &event_i2c_ev_isr;
//     _i2c->er_isr.context = i2c;
//     _i2c->er_isr.handler = &event_i2c_er_isr;
    
//     hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)_i2c->module.dependencies[3];
//     hdl_interrupt_request(ic, _i2c->hw_conf->err_interrupt, &_i2c->er_isr);
//     hdl_interrupt_request(ic, _i2c->hw_conf->ev_interrupt, &_i2c->ev_isr);

//     coroutine_add_static(&i2c_client_task_buff, &_i2c_client_worker, (void *)i2cs);
//     I2C_CTL0((uint32_t)_i2c->module.reg) |= I2C_CTL0_I2CEN;
//     return HDL_MODULE_INIT_OK;
//   }
//   linked_list_unlink(linked_list_item(_i2c));
//   rcu_periph_clock_disable(rcu);
//   return HDL_MODULE_DEINIT_OK;
// }

// hdl_i2c_hw_client_xfer_state_t hdl_i2c_hw_client_xfer_state(hdl_i2c_client_phy_t *i2c) {
//   hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
//   if((_i2c->current_msg != NULL) && (_i2c->current_msg->transfered != _i2c->current_msg->buffer_size)) {
//     _i2c->current_msg->transfered++;
//     return HDL_I2C_HW_CLIENT_XFER_STATE_ONGOING;
//   }
//   _i2c->current_msg = NULL;
//   return HDL_I2C_HW_CLIENT_XFER_STATE_IDLE;
// }

// void hdl_i2c_hw_client_bus_reset(hdl_i2c_client_phy_t *i2c) {
//   hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
//   _i2c->current_msg = NULL;
//   i2c_stop_on_bus((uint32_t)_i2c->module.reg);
// }


// #include "hdl.h"
// #include "CodeLib.h"

// // _Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
// // _Static_assert((offsetof(hdl_i2c_message_private_t, address) == offsetof(hdl_i2c_message_t, address)) ||
// //                (offsetof(hdl_i2c_message_private_t, options) == offsetof(hdl_i2c_message_t, options)) ||
// //                (offsetof(hdl_i2c_message_private_t, buffer) == offsetof(hdl_i2c_message_t, buffer)) ||
// //                (offsetof(hdl_i2c_message_private_t, buffer_size) == offsetof(hdl_i2c_message_t, buffer_size)), "In hdl_i2c.h hdl_i2c_message_t properties order doesn't match");

// // _Static_assert(sizeof(hdl_i2c_transaction_private_t) == sizeof(hdl_i2c_transaction_t), "In hdl_i2c.h data structure size of hdl_i2c_transaction_t doesn't match, check HDL_I2C_TRANSACTION_PRV_SIZE");

// // _Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_PRV_SIZE");
// // _Static_assert((offsetof(hdl_i2c_client_private_t, hw_conf) == offsetof(hdl_i2c_client_t, hw_conf)) , "In hdl_i2c.h hdl_i2c_client_t properties order doesn't match");



// static void _hdl_i2c_work(linked_list_item_t *i2c_item, void *arg) {
//   hdl_i2c_client_private_t *i2c = linked_list_get_object(hdl_i2c_client_private_t, i2c_item);
//   hdl_i2c_transaction_private_t *tr = linked_list_get_object(hdl_i2c_transaction_private_t, i2c->transaction_queue);

//   hdl_i2c_hw_client_xfer_state_t bus_state = hdl_i2c_hw_client_xfer_state(i2c->hw_conf);
//   if(tr != NULL) {
//     if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_IDLE) {
//       if(i2c->current_message == NULL) {
//         if(tr->state == HDL_I2C_TRANSACTION_EXECUTING) {
//           tr->state = HDL_I2C_TRANSACTION_EXECUTED;
//           linked_list_unlink(linked_list_item(tr));
//         }
//         else {
//           tr->state = HDL_I2C_TRANSACTION_EXECUTING;
//           i2c->current_message = tr->message_list;
//         }
//       }
//       if(i2c->current_message != NULL) {
//         hdl_i2c_message_t *mess = (hdl_i2c_message_t *)linked_list_get_object(hdl_i2c_message_private_t, i2c->current_message);
//         hdl_i2c_hw_client_xfer_message((hdl_i2c_client_t *)i2c, mess);
//         i2c->current_message = linked_list_find_next_no_overlap(i2c->current_message, NULL, NULL);
//       }
//     }
//     else if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_ONGOING) {
//       if(tr->state == HDL_I2C_TRANSACTION_TERMINATING) {
//         hdl_i2c_hw_client_bus_reset((hdl_i2c_client_t *)i2c);
//         tr->state = HDL_I2C_TRANSACTION_TERMINATED;
//         i2c->current_message = NULL;
//         linked_list_unlink(linked_list_item(tr));
//       }
//     }
//     else if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_ERROR) {
//       i2c->current_message = NULL;
//       tr->state = HDL_I2C_TRANSACTION_FAILED;
//       linked_list_unlink(linked_list_item(tr));
//       hdl_i2c_hw_client_bus_reset((hdl_i2c_client_t *)i2c);
//     }
//   }
// }

// static uint8_t _i2c_client_sheduler(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   linked_list_t i2cs = (linked_list_t)arg;
//   linked_list_do_foreach(i2cs, &_hdl_i2c_work, NULL);
//   return cancel;
// }

// hdl_module_state_t hdl_i2c(void *desc, uint8_t enable) {
//   static coroutine_desc_static_t i2c_task_buf;
//   static linked_list_t i2cs = NULL;
//   hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t*)desc;
//   hdl_module_state_t i2c_state = hdl_i2c_hw(desc, enable);
//   if(i2c_state == HDL_MODULE_INIT_OK) {
//     linked_list_insert_last(&i2cs, linked_list_item(i2c));
//     coroutine_add_static(&i2c_task_buf, &_i2c_client_sheduler, (void *)i2cs);
//   }
//   else if(i2c_state == HDL_MODULE_DEINIT_OK) {
//     linked_list_unlink(linked_list_item(i2c));
//   }
//   return i2c_state;
// }



#include "hdl_portable.h"
#include "CodeLib.h"

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
  hdl_i2c_client_config_t *config;
  /* private */
  hdl_delegate_t ev_isr;
  hdl_delegate_t er_isr;
  coroutine_desc_static_t i2c_client_task_buff;
} hdl_i2c_client_private_t;



_Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_CLIENT_PRV_SIZE");

static void event_i2c_ev_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

static void event_i2c_er_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

static uint8_t _i2c_client_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *) arg;
  //todo
}

// typedef enum {
//   HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
//   HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
//   HDL_I2C_MESSAGE_MRSW           = 0x04, /* Receiver mode if option set, else trasmitter mode */
//   HDL_I2C_MESSAGE_ADDR_10        = 0x02, /* Send addr */
//   HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
//   HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */
// } hdl_i2c_message_options_t;

static uint8_t _hdl_reg_wait_condition(__IO uint32_t *reg, uint32_t flags, uint8_t is_set, uint32_t timeout) {
  if(is_set) {
    while ((*reg & flags) != flags) ;
  }
  else {
    flags = ~flags;
    while ((*reg | flags) != flags) ;
  }
  return 0;
}

void _hdl_hal_i2c_transfer_message(hdl_i2c_client_t *i2c, hdl_i2c_message_t *message) {
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  uint32_t ss = i2c_periph->CTL0 & I2C_SCLSTRETCH_DISABLE;
  i2c_periph->CTL0 &= ~I2C_SCLSTRETCH_DISABLE;
  uint32_t tmp = 0;
  if(message->options & HDL_I2C_MESSAGE_START) {
    /* send a start condition to I2C bus */
    i2c_periph->CTL0 |= I2C_CTL0_START;
    /* wait until SBSEND bit is set */
    _hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_SBSEND, 1, 10);
  }
  if(message->options & HDL_I2C_MESSAGE_ADDR) {
    if(!(i2c_periph->STAT0 & I2C_STAT0_SBSEND)) {
      // TODO: BAD STATE 
      return;
    }
    uint8_t addr = message->address;
    addr = (addr << 1) | ((message->options & HDL_I2C_MESSAGE_MRSW)? I2C_RECEIVER: 0);
    /* send slave address */
    tmp = i2c_periph->STAT0;
    i2c_periph->DATA = addr;
    /* wait until ADDSEND bit is set */
    _hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_ADDSEND, 1, 10);

  }
  if((message->buffer_size > 0) && !(i2c_periph->STAT1 & I2C_STAT1_TR))
    i2c_periph->CTL0 |= I2C_CTL0_ACKEN;

  if(((message->options & HDL_I2C_MESSAGE_MRSW) && (i2c_periph->STAT1 & I2C_STAT1_TR)) ||
      (!(message->options & HDL_I2C_MESSAGE_MRSW) && !(i2c_periph->STAT1 & I2C_STAT1_TR))) {
    /* if message read operation and desc state transmitter or
          message write operation and desc state receiver */
    // TODO: reset transceiver, return bad state
    i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
    i2c_periph->CTL0 |= I2C_CTL0_STOP;
    return;
  }
  if((message->buffer_size > 0) && (i2c_periph->STAT0 & I2C_STAT0_ADDSEND)) {
    /* clear the ADDSEND bit */
    tmp = i2c_periph->STAT0;
    tmp = i2c_periph->STAT1;
    /* transfer data */
    for (uint32_t i = 0; i < message->buffer_size; i++) {
      if(!(i2c_periph->STAT1 & I2C_STAT1_TR)) {
        if(i == (message->buffer_size - 1)) {
          i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
        }
        /* wait until byte received */
        _hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);    
        message->buffer[i] = i2c_periph->DATA;
      }
      else {
        _hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_TBE, 1, 10);
        i2c_periph->DATA = message->buffer[i];
      }
    }
  }
  if((i2c_periph->STAT1 & I2C_STAT1_TR) && (message->buffer_size > 0))
    _hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_BTC, 1, 10);
  if(message->options & HDL_I2C_MESSAGE_STOP) {
    /* send a start condition to I2C bus */
    i2c_periph->CTL0 |= I2C_CTL0_STOP;
    /* wait until SBSEND bit is set */
    //_hdl_reg_wait_condition(&i2c_periph->STAT0, I2C_STAT0_STPDET, 1, 10);
  }
  I2C_CTL0((uint32_t)i2c_periph) |= ss;
}


hdl_module_state_t hdl_i2c_client(void *i2c, uint8_t enable) {
  hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
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
    i2c_ack_config(((uint32_t)_i2c->module.reg), I2C_ACK_DISABLE);
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
    I2C_CTL0((uint32_t)_i2c->module.reg) |= I2C_CTL0_I2CEN;
    coroutine_add_static(&_i2c->i2c_client_task_buff, &_i2c_client_worker, (void*)_i2c);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&_i2c->i2c_client_task_buff);
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}
