#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

typedef int32_t(*hdl_transceiver_xfer_cb_t)(void *context, uint8_t *data, uint16_t length);
typedef int32_t(*hdl_transceiver_size_cb_t)(void *context);
typedef void(*hdl_transceiver_eot_cb_t)(void *context);

typedef struct {
  hdl_transceiver_eot_cb_t end_of_transmission;
  hdl_transceiver_xfer_cb_t rx_data;
  hdl_transceiver_size_cb_t rx_available;
  hdl_transceiver_xfer_cb_t tx_empty;
  hdl_transceiver_size_cb_t tx_available;
  void *receiver_context;
  void *transmitter_context;
} hdl_transceiver_t;

typedef uint8_t (*hdl_set_transceiver_t)(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id);
typedef hdl_transceiver_t *(*hdl_get_transceiver_t)(const void *desc);

#endif
