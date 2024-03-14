#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* private */
  linked_list_item_t __ll;
  /* public */
  uint32_t address;
  uint8_t *data;
  uint32_t length;
  hdl_i2c_message_options_t options;
} hdl_i2c_message_private_t;

#define HDL_I2C_TRANSACTION_OVN   ((uint32_t)(0x35768470))
#define HDL_I2C_OVN               ((uint32_t)(0x9025F3AC))

typedef struct {
  /* private */
  linked_list_item_t __ll;
  linked_list_t message_list;
  uint32_t ovn;
  hdl_i2c_transaction_state_t state;
} hdl_i2c_transaction_private_t;

//typedef hdl_i2c_transaction_private_t *hdl_i2c_transaction_queue_t;

typedef struct {
  /* private */
  linked_list_t master_transaction_queue;
  uint32_t state;
  uint32_t ovn;
  const hdl_i2c_hw_t *hw;
  uint8_t __private[HDL_I2C_HAL_PRV_SIZE];
  /* public */
  const hdl_i2c_hw_t *hw_conf;
  hdl_callback_t *slave_event_cb;
} hdl_i2c_private_t;

_Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
_Static_assert(offsetof(hdl_i2c_message_private_t, address) == offsetof(hdl_i2c_message_t, address), "In hdl_i2c.h hdl_i2c_message_t properties order doesn't match");

_Static_assert(sizeof(hdl_i2c_transaction_private_t) == sizeof(hdl_i2c_transaction_t), "In hdl_i2c.h data structure size of hdl_i2c_transaction_t doesn't match, check HDL_I2C_TRANSACTION_PRV_SIZE");

_Static_assert(sizeof(hdl_i2c_private_t) == sizeof(hdl_i2c_t), "In hdl_i2c.h data structure size of hdl_i2c_t doesn't match, check HDL_I2C_PRV_SIZE");
_Static_assert(offsetof(hdl_i2c_private_t, slave_event_cb) == offsetof(hdl_i2c_t, slave_event_cb), "In hdl_i2c.h hdl_i2c_t properties order doesn't match");

void hdl_i2c_client_transaction_init_reset(hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if(tr != NULL) {
    if(tr->ovn == HDL_I2C_TRANSACTION_OVN) {
      if(tr->state == HDL_I2C_TRANSACTION_EXECUTING)
        tr->state = HDL_I2C_TRANSACTION_TERMINATING;
      if(tr->state == HDL_I2C_TRANSACTION_TERMINATING)
        return;
    }
    linked_list_unlink(&tr->__ll);
    tr->ovn = HDL_I2C_TRANSACTION_OVN;
    tr->state = HDL_I2C_TRANSACTION_NEW;
    linked_list_clear((linked_list_t *)&tr->message_list);
  }
}

void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message) {
  hdl_i2c_message_private_t *mess = (hdl_i2c_message_private_t *)message;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((tr != NULL) && (tr->ovn == HDL_I2C_TRANSACTION_OVN) && (tr->state == HDL_I2C_TRANSACTION_NEW) && (mess != NULL)) {
    linked_list_insert_last(&tr->message_list, &mess->__ll);
  }
}

void hdl_i2c_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction) {
  hdl_i2c_private_t *i2c = (hdl_i2c_private_t *)desc;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((i2c != NULL) && (i2c->ovn == HDL_I2C_OVN) && 
     (tr != NULL) && (tr->ovn == HDL_I2C_TRANSACTION_OVN) && (tr->state == HDL_I2C_TRANSACTION_NEW)) {
    linked_list_insert_last(&i2c->master_transaction_queue, &tr->__ll);
  }
}

hdl_i2c_transaction_state_t hdl_i2c_transaction_status(hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((tr != NULL) && (tr->ovn == HDL_I2C_TRANSACTION_OVN)) {
    return tr->state;
  }
  return HDL_I2C_TRANSACTION_UNKNOWN;
}

void hdl_i2c_init(hdl_i2c_t *desc) {
  hdl_i2c_private_t *i2c = (hdl_i2c_private_t *)desc;
  if((i2c != NULL) && (i2c->ovn != HDL_I2C_OVN)) {
    linked_list_clear(&i2c->master_transaction_queue);
    i2c->ovn = HDL_I2C_OVN;
    hdl_hal_i2c_init(desc);
  }
}

void hdl_i2c_work(hdl_i2c_t *desc) {
  hdl_i2c_private_t *i2c = (hdl_i2c_private_t *)desc;
  if((i2c != NULL) && (i2c->ovn == HDL_I2C_OVN)) {
    //TODO:
    //hdl_hal_i2c_transfer_message(i2c->hw, (hdl_i2c_message_t *)i2c->master_transaction_queue->message_list);
    //linked_list_unlink(i2c->master_transaction_queue->message_list);
  }
}


// void hdl_i2c_transfer(const hdl_i2c_t *desc, hdl_i2c_message_t messages[], uint32_t amount) {
//   if(desc == NULL) {
//     // bad arg
//   }
//   //if(desc->)
//   i2c_periph_t *i2c_regs = (i2c_periph_t *)hw_conf->periph;
//   /* wait until I2C bus is idle */
//   if(_hdl_reg_wait_condition(&i2c_regs->STAT1, I2C_STAT1_I2CBSY, 0, 10)) {
    
//   }
//   for(int32_t i = 0; i < amount; i++) {
//     _hdl_i2c_transfer_message(i2c_regs, &messages[i]);
//   }
//   i2c_stop_on_bus(hw_conf->periph);
// }
