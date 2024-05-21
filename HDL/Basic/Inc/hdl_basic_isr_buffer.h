#ifndef TRANSCEIVER_BUFFER_H_
#define TRANSCEIVER_BUFFER_H_

#define HDL_ISR_BUFFER_PRIVATE_SIZE       48

typedef struct {
  uint8_t __private[HDL_ISR_BUFFER_PRIVATE_SIZE];
} hdl_isr_buffer_t;

typedef struct {
  uint8_t *rx_buffer;
  uint8_t *tx_buffer;
  uint16_t rx_buffer_size;
  uint16_t tx_buffer_size;    
} hdl_isr_buffer_congig_t;

hdl_transceiver_t *hdl_get_isr_transceiver_handler(hdl_isr_buffer_t *desc, hdl_isr_buffer_congig_t *cnf);

#endif /* TRANSCEIVER_BUFFER_H_ */
