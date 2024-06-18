#include "hdl.h"
#include "CodeLib.h"

#define TRANSACTION_OVN ((uint32_t)(0xAF583DEA))
typedef struct {
  /* private */
  __linked_list_object__
  PRIVATE(hw, HDL_I2C_HW_MESSAGE_PRV_SIZE);
  /* public */
  uint16_t address;
  uint8_t *buffer;
  uint16_t buffer_size;
  hdl_i2c_message_options_t options;
} hdl_i2c_message_private_t;

typedef struct {
  /* private */
  uint32_t ovn;
  __linked_list_object__
  linked_list_t message_list;
  hdl_i2c_transaction_state_t state;
} hdl_i2c_transaction_private_t;

typedef struct {
  /* public */
  hdl_module_t module;
  const hdl_i2c_client_hw_t *hw_conf;
  /* private */
  linked_list_t transaction_queue;
  linked_list_item_t *current_message;
  __linked_list_object__;
  PRIVATE(hw, HDL_I2C_HAL_PRV_SIZE);
} hdl_i2c_client_private_t;

_Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
_Static_assert((offsetof(hdl_i2c_message_private_t, address) == offsetof(hdl_i2c_message_t, address)) ||
               (offsetof(hdl_i2c_message_private_t, options) == offsetof(hdl_i2c_message_t, options)) ||
               (offsetof(hdl_i2c_message_private_t, buffer) == offsetof(hdl_i2c_message_t, buffer)) ||
               (offsetof(hdl_i2c_message_private_t, buffer_size) == offsetof(hdl_i2c_message_t, buffer_size)), "In hdl_i2c.h hdl_i2c_message_t properties order doesn't match");

_Static_assert(sizeof(hdl_i2c_transaction_private_t) == sizeof(hdl_i2c_transaction_t), "In hdl_i2c.h data structure size of hdl_i2c_transaction_t doesn't match, check HDL_I2C_TRANSACTION_PRV_SIZE");

_Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_PRV_SIZE");
_Static_assert((offsetof(hdl_i2c_client_private_t, hw_conf) == offsetof(hdl_i2c_client_t, hw_conf)) , "In hdl_i2c.h hdl_i2c_client_t properties order doesn't match");

uint8_t hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message) {
  hdl_i2c_message_private_t *mess = (hdl_i2c_message_private_t *)message;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((tr == NULL) || (mess == NULL))
    return HDL_FALSE;
  if(tr->ovn != TRANSACTION_OVN) {
    tr->ovn = TRANSACTION_OVN;
    tr->message_list = NULL;
  } else switch (tr->state) {
    case HDL_I2C_TRANSACTION_ENQUEUED:
    case HDL_I2C_TRANSACTION_EXECUTING:
    case HDL_I2C_TRANSACTION_TERMINATING:
      return HDL_FALSE;
    case HDL_I2C_TRANSACTION_EXECUTED:
    case HDL_I2C_TRANSACTION_TERMINATED:
    case HDL_I2C_TRANSACTION_FAILED:
      linked_list_clear(&tr->message_list);
    case HDL_I2C_TRANSACTION_NEW:
    default: break;
  }
  // TODO: if message is part of the other transaction
  tr->state = HDL_I2C_TRANSACTION_NEW;
  linked_list_insert_last(&tr->message_list, linked_list_item(mess));
  return HDL_TRUE;
}

void hdl_i2c_client_enqueue_transaction(hdl_i2c_client_t *desc, hdl_i2c_transaction_t *transaction) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)desc;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((hdl_state(&desc->module) != HDL_MODULE_INIT_OK) || (tr == NULL) || (tr->ovn != TRANSACTION_OVN) || (tr->state != HDL_I2C_TRANSACTION_NEW))
    return;
  linked_list_insert_last(&i2c->transaction_queue, linked_list_item(tr));
  tr->state = HDL_I2C_TRANSACTION_ENQUEUED;
}

hdl_i2c_transaction_state_t hdl_i2c_client_transaction_state(hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((tr == NULL) || (tr->ovn != TRANSACTION_OVN))
    return HDL_I2C_TRANSACTION_UNKNOWN;
  return tr->state;
}

void hdl_i2c_client_cancel_transaction(hdl_i2c_client_t *desc, hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if((tr == NULL) || (tr->ovn != TRANSACTION_OVN))
    return;
  else switch (tr->state) {
    case HDL_I2C_TRANSACTION_ENQUEUED:
      linked_list_unlink(linked_list_item(tr));
      tr->state = HDL_I2C_TRANSACTION_TERMINATED;
      return;
    case HDL_I2C_TRANSACTION_EXECUTING:
      tr->state = HDL_I2C_TRANSACTION_TERMINATING;
    default : break;
  }
}

static void _hdl_i2c_work(linked_list_item_t *i2c_item, void *arg) {
  hdl_i2c_client_private_t *i2c = linked_list_get_object(hdl_i2c_client_private_t, i2c_item);
  hdl_i2c_transaction_private_t *tr = linked_list_get_object(hdl_i2c_transaction_private_t, i2c->transaction_queue);
  hdl_i2c_hw_client_xfer_state_t bus_state = hdl_i2c_hw_client_xfer_state((hdl_i2c_client_t *)i2c);
  if(tr != NULL) {
    if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_IDLE) {
      if(i2c->current_message == NULL) {
        if(tr->state == HDL_I2C_TRANSACTION_EXECUTING) {
          tr->state = HDL_I2C_TRANSACTION_EXECUTED;
          linked_list_unlink(linked_list_item(tr));
        }
        else {
          tr->state = HDL_I2C_TRANSACTION_EXECUTING;
          i2c->current_message = tr->message_list;
        }
      }
      if(i2c->current_message != NULL) {
        hdl_i2c_message_t *mess = (hdl_i2c_message_t *)linked_list_get_object(hdl_i2c_message_private_t, i2c->current_message);
        hdl_i2c_hw_client_xfer_message((hdl_i2c_client_t *)i2c, mess);
        i2c->current_message = linked_list_find_next_no_overlap(i2c->current_message, NULL, NULL);
      }
    }
    else if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_ONGOING) {
      if(tr->state == HDL_I2C_TRANSACTION_TERMINATING) {
        hdl_i2c_hw_client_bus_reset((hdl_i2c_client_t *)i2c);
        tr->state = HDL_I2C_TRANSACTION_TERMINATED;
        i2c->current_message = NULL;
        linked_list_unlink(linked_list_item(tr));
      }
    }
    else if(bus_state == HDL_I2C_HW_CLIENT_XFER_STATE_ERROR) {
      i2c->current_message = NULL;
      tr->state = HDL_I2C_TRANSACTION_FAILED;
      linked_list_unlink(linked_list_item(tr));
      hdl_i2c_hw_client_bus_reset((hdl_i2c_client_t *)i2c);
    }
  }
}

static uint8_t _i2c_client_sheduler(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t i2cs = (linked_list_t)arg;
  linked_list_do_foreach(i2cs, &_hdl_i2c_work, NULL);
  return cancel;
}

hdl_module_state_t hdl_i2c(void *desc, uint8_t enable) {
  static coroutine_desc_static_t i2c_task_buf;
  static linked_list_t i2cs = NULL;
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t*)desc;
  hdl_module_state_t i2c_state = hdl_i2c_hw(desc, enable);
  if(i2c_state == HDL_MODULE_INIT_OK) {
    linked_list_insert_last(&i2cs, linked_list_item(i2c));
    coroutine_add_static(&i2c_task_buf, &_i2c_client_sheduler, (void *)i2cs);
  }
  else if(i2c_state == HDL_MODULE_DEINIT_OK) {
    linked_list_unlink(linked_list_item(i2c));
  }
  return i2c_state;
}
