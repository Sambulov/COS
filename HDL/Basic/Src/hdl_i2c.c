#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  /* private */
  __linked_list_object__
  /* public */
  uint32_t address;
  uint8_t *buffer;
  uint32_t buffer_size;
  uint32_t data_transfered;
  hdl_i2c_message_flags_t flags;
} hdl_i2c_message_private_t;

#define HDL_I2C_TRANSACTION_OVN   ((uint32_t)(0x35768470))
#define HDL_I2C_OVN               ((uint32_t)(0x9025F3AC))

typedef struct {
  /* private */
  __linked_list_object__
  linked_list_t message_list;
  uint32_t ovn;
  hdl_i2c_transaction_state_t state;
} hdl_i2c_transaction_private_t;

typedef struct {
  /* private */
  linked_list_t transaction_queue;
  hdl_i2c_message_private_t message_buf;
  uint32_t ovn;
  LinkedListItem_t *current_message;
  /* public */
  const hdl_i2c_hw_t *hw_conf;
  hdl_hal_i2c_t hal;
  event_handler_t *server_event_cb;
  void *context;
} hdl_i2c_private_t;

_Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
_Static_assert((offsetof(hdl_i2c_message_private_t, address) == offsetof(hdl_i2c_message_t, address)) ||
               (offsetof(hdl_i2c_message_private_t, buffer) == offsetof(hdl_i2c_message_t, buffer)) ||
               (offsetof(hdl_i2c_message_private_t, buffer_size) == offsetof(hdl_i2c_message_t, buffer_size)) ||
               (offsetof(hdl_i2c_message_private_t, data_transfered) == offsetof(hdl_i2c_message_t, data_transfered)) ||
               (offsetof(hdl_i2c_message_private_t, flags) == offsetof(hdl_i2c_message_t, flags)), "In hdl_i2c.h hdl_i2c_message_t properties order doesn't match");

_Static_assert(sizeof(hdl_i2c_transaction_private_t) == sizeof(hdl_i2c_transaction_t), "In hdl_i2c.h data structure size of hdl_i2c_transaction_t doesn't match, check HDL_I2C_TRANSACTION_PRV_SIZE");

_Static_assert(sizeof(hdl_i2c_private_t) == sizeof(hdl_i2c_t), "In hdl_i2c.h data structure size of hdl_i2c_t doesn't match, check HDL_I2C_PRV_SIZE");
_Static_assert((offsetof(hdl_i2c_private_t, hw_conf) == offsetof(hdl_i2c_t, hw_conf)) &&
               (offsetof(hdl_i2c_private_t, hal) == offsetof(hdl_i2c_t, hal)) &&
               (offsetof(hdl_i2c_private_t, server_event_cb) == offsetof(hdl_i2c_t, server_event_cb)), "In hdl_i2c.h hdl_i2c_t properties order doesn't match");

static uint8_t _hdl_i2c_init(hdl_i2c_private_t *i2c) {
  if(i2c != NULL) {
    linked_list_clear(&i2c->transaction_queue);
    i2c->ovn = HDL_I2C_OVN;
    i2c->current_message = NULL;
    //i2c->state = 
    hdl_i2c((hdl_i2c_t *)i2c, HDL_TRUE);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static inline uint8_t _hdl_i2c_desc_is_invalid(hdl_i2c_private_t *i2c) {
  return (i2c == NULL) || ((i2c->ovn != HDL_I2C_OVN) && !_hdl_i2c_init(i2c));
}

static inline uint8_t _hdl_i2c_tr_desc_is_invalid(hdl_i2c_transaction_private_t *tr) {
  return (tr == NULL) || (tr->ovn != HDL_I2C_TRANSACTION_OVN) || (tr->state != HDL_I2C_TRANSACTION_NEW);
}

void hdl_i2c_client_transaction_init_reset(hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if(tr != NULL) {
    if(tr->ovn == HDL_I2C_TRANSACTION_OVN) {
      if(tr->state == HDL_I2C_TRANSACTION_EXECUTING)
        tr->state = HDL_I2C_TRANSACTION_TERMINATING;
      if(tr->state == HDL_I2C_TRANSACTION_TERMINATING)
        return;
    }
    linked_list_unlink(linked_list_item(tr));
    tr->ovn = HDL_I2C_TRANSACTION_OVN;
    tr->state = HDL_I2C_TRANSACTION_NEW;
    linked_list_clear((linked_list_t *)&tr->message_list);
  }
}

void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message) {
  hdl_i2c_message_private_t *mess = (hdl_i2c_message_private_t *)message;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if(_hdl_i2c_tr_desc_is_invalid(tr) || (tr->state != HDL_I2C_TRANSACTION_NEW) || (mess == NULL))
    return;
  //TODO: check message;
  message->data_transfered = 0;
  linked_list_insert_last(&tr->message_list, linked_list_item(mess));
}

void hdl_i2c_client_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction) {
  hdl_i2c_private_t *i2c = (hdl_i2c_private_t *)desc;
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if(_hdl_i2c_desc_is_invalid(i2c) || _hdl_i2c_tr_desc_is_invalid(tr))
    return;
  if(tr->message_list != NULL) {
    linked_list_insert_last(&i2c->transaction_queue, linked_list_item(tr));
    tr->state = HDL_I2C_TRANSACTION_ENQUEUED;
  }
  else {
    tr->state = HDL_I2C_TRANSACTION_EXECUTED;
  }
}

hdl_i2c_transaction_state_t hdl_i2c_client_transaction_state(hdl_i2c_transaction_t *transaction) {
  hdl_i2c_transaction_private_t *tr = (hdl_i2c_transaction_private_t *)transaction;
  if(_hdl_i2c_tr_desc_is_invalid(tr))
    return HDL_I2C_TRANSACTION_UNKNOWN;
  return tr->state;
}

void hdl_i2c_work(hdl_i2c_t *desc) {
  hdl_i2c_private_t *i2c = (hdl_i2c_private_t *)desc;
  if(_hdl_i2c_desc_is_invalid(i2c))
    return;
  hdl_i2c_transaction_private_t *tr = linked_list_get_object(hdl_i2c_transaction_private_t, i2c->transaction_queue);

  if(i2c->current_message == NULL) {
    if(tr != NULL) {
      tr->state = HDL_I2C_TRANSACTION_EXECUTING;
      i2c->current_message = tr->message_list;
    }
    if(i2c->current_message == NULL)
      return;
  }

  if(i2c->current_message != NULL) {
    if(tr->state == HDL_I2C_TRANSACTION_TERMINATING) {
      hdl_i2c(desc, HDL_TRUE);
      linked_list_unlink(i2c->transaction_queue);
      tr->state = HDL_I2C_TRANSACTION_TERMINATED;
    }
    else {
      hdl_i2c_message_t *mess = (hdl_i2c_message_t *)linked_list_get_object(hdl_i2c_message_private_t, i2c->current_message);
      hdl_i2c_client_xfer_message(desc, mess);
      // if(transfer complete){
      //   i2c->current_message = linked_list_find_next_no_overlap(i2c->current_message, NULL, NULL);
      //   if(i2c->current_message == NULL) {
      //     tr->state = HDL_I2C_TRANSACTION_EXECUTED;
      //   }
      // }
      // if(transfer err) {
      //   i2c->current_message = NULL;
      //   tr->state = HDL_I2C_TRANSACTION_FAILED;
      // }
    }
  }
  if(i2c->current_message == NULL) {
    linked_list_unlink(i2c->transaction_queue);
  }
}
