#include "app.h"

extern const app_mem_map_item_t app_r_mem_map[APP_R_MEM_MAP_SIZE];
extern const app_mem_map_item_t app_w_mem_map[APP_W_MEM_MAP_SIZE];

typedef struct {
  uint16_t current_address;
  uint8_t state;
} som_proto_context_t;

#define SOM_COMM_STATE_IDLE            0
#define SOM_COMM_STATE_RX_ADDR_HIGH    SOM_COMM_STATE_IDLE
#define SOM_COMM_STATE_RX_ADDR_LOW     1
#define SOM_COMM_STATE_RECEIVE         2

static uint8_t _bldl_mem_map_rw(const app_mem_map_item_t map[], uint8_t map_size, uint16_t map_addr, uint8_t *w_data) {
  static uint8_t current = 0;
  for(uint8_t i = 0; i < map_size; i++) {
    if(current >= map_size)
      current = 0;
    if((map_addr >= map[current].map_addr_start) && (map_addr <= map[current].map_addr_end)) {
      uint8_t *data = &(map[current].data_ptr[map_addr - map[current].map_addr_start]);
      if(w_data != NULL) *data = *w_data;
      return *data;
    }
    current++;
  }
  return 0;
}

static uint8_t _bldl_som_link_read_byte_cb(uint16_t addr) {
  return _bldl_mem_map_rw(app_r_mem_map, APP_R_MEM_MAP_SIZE, addr, NULL);
}

static void _bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data) {
  _bldl_mem_map_rw(app_w_mem_map, APP_W_MEM_MAP_SIZE, addr, &data);
}

static uint32_t _som_tx_rx_available(void *context) {
  return 65536;
}

static uint32_t _som_trunsmitter(void *context, uint8_t *data, uint32_t length) {
  som_proto_context_t *proto = (som_proto_context_t *)context;
  *data = _bldl_som_link_read_byte_cb(proto->current_address);
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
      _bldl_som_link_write_byte_cb(proto->current_address, *data);
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
