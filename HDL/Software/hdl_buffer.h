#ifndef TRANSCEIVER_BUFFER_H_
#define TRANSCEIVER_BUFFER_H_

#define HDL_ISR_BUFFER_PRIVATE_SIZE       52

typedef struct {
  uint8_t *data;
  uint16_t size;   
} hdl_basic_buffer_t;

typedef struct {
  uint8_t *data[2];
  uint16_t size;
  uint8_t active_buffer : 1;
} hdl_double_buffer_t;

typedef struct {
  PRIVATE(hdl, HDL_ISR_BUFFER_PRIVATE_SIZE);
} hdl_isr_buffer_t;

typedef struct {
  uint8_t *rx_buffer;
  uint8_t *tx_buffer;
  uint16_t rx_buffer_size;
  uint16_t tx_buffer_size;    
} hdl_isr_buffer_config_t;

static inline void hdl_double_buffer_switch(hdl_double_buffer_t *buffer) {
  if(buffer != NULL) {
    buffer->active_buffer = !buffer->active_buffer;
  }
}

uint16_t hdl_isr_buffer_read(hdl_isr_buffer_t *desc, uint8_t *data, uint16_t lenght);
uint16_t hdl_isr_buffer_write(hdl_isr_buffer_t *desc, uint8_t *data, uint16_t lenght);
hdl_transceiver_t *hdl_get_isr_transceiver_handler(hdl_isr_buffer_t *desc, hdl_isr_buffer_config_t *cnf);

#endif /* TRANSCEIVER_BUFFER_H_ */
