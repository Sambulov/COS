#ifndef HDL_MAC_H_
#define HDL_MAC_H_

typedef enum {
  HDL_MAC_STATE_BUSY     = 0x01,
  HDL_MAC_STATE_CANCELED = 0x02,
  HDL_MAC_STATE_COMPLETE = 0x08,
  HDL_MAC_STATE          = 0x0F,
  HDL_MAC_ERROR_TIMEOUT  = 0x10,
  HDL_MAC_ERROR_CRC      = 0x20,
  HDL_MAC_ERROR_BAD_ARG  = 0xE0,
  HDL_MAC_ERROR_INTERNAL = 0xF0,
  HDL_MAC_ERROR          = 0xF0
} hdl_mac_msg_status_t;

typedef struct {
  uint8_t dev_addr;
  uint8_t reg_addr;
  uint16_t reg_val;
  void *context;
} hdl_mac_mdio_message_t;

typedef struct {
  void *data;
  uint32_t data_length;
} hdl_mac_data_t;

typedef struct {
  uint8_t mac_addr[6];
  uint32_t speed;
  uint32_t mtu;
  uint8_t half_duplex :1;
} hdl_mac_config_t;

typedef void (*hdl_mac_mdio_cb_t)(hdl_mac_msg_status_t status, const void *desc, hdl_mac_mdio_message_t *message);

typedef uint8_t (*hdl_mac_mdio_transfer_t)(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb);
typedef uint8_t (*hdl_mac_transfer_t)(const void *desc, ETH_BufferTypeDef *data, uint32_t total_len);
typedef uint8_t (*hdl_mac_transfer2_t)(const void *desc, void **data);
typedef uint8_t (*hdl_mac_cnf_t)(const void *desc, hdl_mac_config_t *cnf);

typedef struct {
  hdl_module_initializer_t init;
  hdl_mac_mdio_transfer_t read_reg;
  hdl_mac_mdio_transfer_t write_reg;
  hdl_mac_transfer2_t receive;
  hdl_mac_transfer_t transmit;
  hdl_mac_cnf_t get_cnf;
  hdl_mac_cnf_t set_cnf;
} hdl_mac_iface_t;

hdl_module_new_t(hdl_mac_t, 0, void *, hdl_mac_iface_t);

__STATIC_INLINE uint8_t hdl_mac_receive(const void *desc, void **data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->receive(desc, data);
}

__STATIC_INLINE uint8_t hdl_mac_transmit(const void *desc, ETH_BufferTypeDef *data, uint32_t total_len) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->transmit(desc, data, total_len);
}

__STATIC_INLINE uint8_t hdl_mac_mdio_read(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->read_reg(desc, message, cb);
}

__STATIC_INLINE uint8_t hdl_mac_mdio_write(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->read_reg(desc, message, cb);
}

__STATIC_INLINE uint8_t hdl_mac_get_cnf(const void *desc, hdl_mac_config_t *cnf) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->get_cnf(desc, cnf);
}

__STATIC_INLINE uint8_t hdl_mac_set_cnf(const void *desc, hdl_mac_config_t *cnf) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->set_cnf(desc, cnf);
}



#endif /* HDL_MAC_H_ */
