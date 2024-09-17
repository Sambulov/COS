#include "app.h"

#define SOM_COMM_STATE_IDLE            0
#define SOM_COMM_STATE_RX_ADDR_HIGH    SOM_COMM_STATE_IDLE
#define SOM_COMM_STATE_RX_ADDR_LOW     1
#define SOM_COMM_STATE_RECEIVE         2

static uint32_t _som_tx_rx_available(void *context) {
  return 65536;
}

static uint32_t _som_trunsmitter(void *context, uint8_t *data, uint32_t length) {
  som_proto_context_t *proto = (som_proto_context_t *)context;
  *data = bldl_som_link_read_byte_cb(proto->current_address);
  proto->current_address++;
  return 1;
}

static uint32_t _som_receiver(void *context, uint8_t *data, uint32_t length) {
  static volatile uint32_t bytes = 0;
  bytes++;
  som_proto_context_t *proto = (som_proto_context_t *)context;
  switch (proto->state) {
    case SOM_COMM_STATE_RX_ADDR_HIGH:
      proto->current_address = ((uint16_t)*data) << 8;
      proto->state = SOM_COMM_STATE_RX_ADDR_LOW;
      break;
    case SOM_COMM_STATE_RX_ADDR_LOW:
      proto->current_address |= ((uint16_t)*data) << 0;
      proto->state = SOM_COMM_STATE_RECEIVE;
      break;
    case SOM_COMM_STATE_RECEIVE:
      bldl_som_link_write_byte_cb(proto->current_address, *data);
    default:
      break;
  }
  return 0;
}

static void _som_xfer_complete(void *context) {
  som_proto_context_t *proto = (som_proto_context_t *)context;
  proto->state = SOM_COMM_STATE_IDLE;
}

void bldl_som_link_init() {
  static som_proto_context_t som_proto = {
    .current_address = 0x0,
    .state = SOM_COMM_STATE_IDLE,
  };
  static hdl_transceiver_t som_comm = {
    .rx_available = &_som_tx_rx_available,
    .rx_data = &_som_receiver,
    .tx_available = &_som_tx_rx_available,
    .tx_empty = &_som_trunsmitter,
    .end_of_transmission = &_som_xfer_complete,
    .proto_context = &som_proto
  };
  hdl_i2c_set_transceiver(&uspd20k_i2c_som, &som_comm);
}
