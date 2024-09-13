#include "app.h"
#include "CodeLib.h"


#define SOM_COMM_STATE_IDLE            0
#define SOM_COMM_STATE_RX_ADDR_LOW     1
#define SOM_COMM_STATE_XFER            2

#define MEASURE_VALID   0xCAFEFEED

typedef struct {
  uint32_t ain1;
  uint32_t ain2;
  uint32_t ain3;
  uint32_t ain4;
  uint32_t ain5;
  uint32_t ain6;
  uint32_t timestamp;
  uint32_t valid;
} measure_t;

#define MEASURES_AMOUNT    64

measure_t measures[MEASURES_AMOUNT] = {0};


typedef struct {
  void *mem_ptr;
  uint16_t map_addr;
  uint16_t athomary_block_size;
  uint16_t blocks_amount;
} mem_map_block;


#define MAP_ADDR_MEASURES         0x0000
#define MAP_ADDR_CIRCUIT_CONFIG   0x1000

#define MEM_MAP_ARRAY(arr_ptr, type, addr)    (&(mem_map_block){.mem_ptr = (arr_ptr), .athomary_block_size = sizeof(type), .blocks_amount = sizeof(arr_ptr)/sizeof(void *),.map_addr = addr})

mem_map_block *read_mem_map[] = {
  MEM_MAP_ARRAY(&measures, measure_t, MAP_ADDR_MEASURES),
  NULL
};

mem_map_block write_mem_map[] = {
  NULL
};

// typedef struct {
//   mem_map_block *read_mem_map;
//   mem_map_block *write_mem_map;
//   mem_map_block *current_block;
//   uint16_t offset;
//   uint8_t *fetch_buffer;
// } mem_map_context_t;

// void mem_map_set_addr(mem_map_context_t *mem_map, uint32_t addr) {
//   //mem_map->addr = addr;
// }

// void mem_map_write(mem_map_context_t *mem_map, uint8_t data) {
  
// }

// uint8_t mem_map_read(mem_map_context_t *mem_map) {
//   return 0;
// }

typedef struct {
  uint16_t current_address;
  uint8_t state;
//  mem_map_context_t *mem_map;
} som_proto_context_t;

uint32_t som_receiver(void *context, uint8_t *data, uint32_t length) {
  static volatile uint32_t bytes = 0;
  bytes++;
  som_proto_context_t *proto = (som_proto_context_t *)context;
  switch (proto->state) {
    case SOM_COMM_STATE_IDLE:
      proto->current_address = ((uint16_t)*data) << 8;
      proto->state = SOM_COMM_STATE_RX_ADDR_LOW;
      break;
    case SOM_COMM_STATE_RX_ADDR_LOW:
      proto->current_address |= ((uint16_t)*data) << 0;
      proto->state = SOM_COMM_STATE_XFER;
      //mem_map_set_addr(proto->mem_map, proto->current_address);
      break;
    case SOM_COMM_STATE_XFER:
      //mem_map_write(proto->mem_map, *data);
      break;
    default:
      break;
  }
  return 0;
}

uint32_t som_trunsmitter(void *context, uint8_t *data, uint32_t length) {
  som_proto_context_t *proto = (som_proto_context_t *)context;
  uint16_t mem_end = 2048;
  if(mem_end > proto->current_address) {
    *data = ((uint8_t *)&measures)[proto->current_address++];
  }
  else {
    *data = 0;
  }
  proto->current_address++;
  return 1;
}

uint32_t som_tx_rx_available(void *context) {
  return 65536;
}

void som_xfer_complete(void *context) {
  som_proto_context_t *proto = (som_proto_context_t *)context;
  proto->state = SOM_COMM_STATE_IDLE;
  //mem_map_set_addr(proto->mem_map, 0);
}

void main() {
  hdl_enable(&mod_app);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  som_proto_context_t som_proto = {
    .current_address = 0,
    .state = SOM_COMM_STATE_IDLE
  };

  hdl_transceiver_t som_comm = {
    .rx_available = &som_tx_rx_available,
    .rx_data = &som_receiver,
    .tx_available = &som_tx_rx_available,
    .tx_empty = &som_trunsmitter,
    .end_of_transmission = &som_xfer_complete,
    .proto_context = &som_proto
  };

  hdl_i2c_set_transceiver(&uspd20k_i2c_som, &som_comm);

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_NTC_PU);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_4K3_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_150R_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_1K_PD);

  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_1K_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_FLOATING);

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_FLOATING);

  uint32_t adc_age = 0, ind = 0;
  
  while (1) {
    cooperative_scheduler(false);
    uint32_t age = hdl_adc_ms5194t_age(&mod_adc);
    if(adc_age != age) {
      measures[ind].valid = 0;
      measures[ind].ain1 = hdl_adc_ms5194t_get(&mod_adc, 0);
      measures[ind].ain2 = hdl_adc_ms5194t_get(&mod_adc, 1);
      measures[ind].ain3 = hdl_adc_ms5194t_get(&mod_adc, 2);
      measures[ind].ain4 = hdl_adc_ms5194t_get(&mod_adc, 3);
      measures[ind].ain5 = hdl_adc_ms5194t_get(&mod_adc, 4);
      measures[ind].ain6 = hdl_adc_ms5194t_get(&mod_adc, 5);
      measures[ind].timestamp = age;
      measures[ind].valid = MEASURE_VALID;
      ind++;
      if(ind >= MEASURES_AMOUNT) {
        ind = 0;
      }
      adc_age = age;      
    }

  }
}
