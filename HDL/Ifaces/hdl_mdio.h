#ifndef HDL_MDIO_H_
#define HDL_MDIO_H_

// typedef enum {
//   HDL_MDIO_STATE_BUSY     = 0x01,
//   HDL_MDIO_STATE_CANCELED = 0x02,
//   HDL_MDIO_STATE_COMPLETE = 0x08,
//   HDL_MDIO_STATE          = 0x0F,
//   HDL_MDIO_ERROR_TIMEOUT  = 0x10,
//   HDL_MDIO_ERROR_CRC      = 0x20,
//   HDL_MDIO_ERROR_BAD_ARG  = 0xE0,
//   HDL_MDIO_ERROR_INTERNAL = 0xF0,
//   HDL_MDIO_ERROR          = 0xF0
// } hdl_mac_msg_status_t;

typedef struct {
  uint8_t dev_addr;
  uint8_t reg_addr;
  uint16_t reg_val;
  void *context;
} hdl_mdio_message_t;

//typedef void (*hdl_mdio_cb_t)(hdl_mac_msg_status_t status, const void *desc, hdl_mac_mdio_message_t *message);

typedef uint8_t (*hdl_mdio_transfer_t)(const void *desc, hdl_mdio_message_t *message);//, hdl_mac_mdio_cb_t cb);

typedef struct {
  hdl_module_initializer_t init;
  hdl_mdio_transfer_t read_reg;
  hdl_mdio_transfer_t write_reg;
} hdl_mdio_iface_t;

hdl_module_new_t(hdl_mdio_t, 0, void *, hdl_mdio_iface_t);

__STATIC_INLINE uint8_t hdl_mdio_read(const void *desc, hdl_mdio_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mdio_t *)desc)->iface->read_reg(desc, message);
}

__STATIC_INLINE uint8_t hdl_mdio_write(const void *desc, hdl_mdio_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mdio_t *)desc)->iface->read_reg(desc, message);
}

#endif /* HDL_MDIO_H_ */
