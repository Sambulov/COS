#include "hdl.h"

void hdl_i2c_client_new_transaction(hdl_i2c_transaction_t *transaction) {

}

void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message) {
  linked_list_insert_last((linked_list_t *)&transaction->message_list, (linked_list_item_t *)&message);
}

void hdl_i2c_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction) {
  //TODO: check desc, lock queue, bloc transaction
  linked_list_insert_last((linked_list_t *)&desc->master_transaction_queue, (linked_list_item_t *)transaction);
}
