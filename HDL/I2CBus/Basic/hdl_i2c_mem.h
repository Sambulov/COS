#ifndef HDL_I2C_MEM_H_
#define HDL_I2C_MEM_H_

#define HDL_I2C_MEM_VAR_SIZE            60

typedef struct {
  uint8_t xfer_retry;
} hdl_i2c_mem_config_t;

typedef enum {
  HDL_I2C_MEM_BUSY       = 0x80,
  HDL_I2C_MEM_XFER_OK    = 0x00,
  HDL_I2C_MEM_XFER_FAIL  = 0x0F
} hdl_i2c_mem_state_t;

typedef hdl_i2c_mem_state_t (*hdl_i2c_mem_get_state_t)(const void *desc);
typedef uint8_t (*hdl_i2c_mem_read_t)(const void *desc, uint8_t dev_addr, uint8_t addr_size, uint32_t addr, uint8_t *data, uint32_t size);
typedef uint8_t (*hdl_i2c_mem_write_t)(const void *desc, uint8_t dev_addr, uint8_t addr_size, uint32_t addr, const uint8_t *data, uint32_t size);

typedef struct {
  hdl_module_initializer_t init;
  hdl_i2c_mem_read_t read;
  hdl_i2c_mem_write_t write;
  hdl_i2c_mem_get_state_t state;
} hdl_i2c_mem_iface_t;

/* depends on
  hdl_i2c
*/
hdl_module_new_t(hdl_i2c_mem_t, HDL_I2C_MEM_VAR_SIZE, hdl_i2c_mem_config_t*, hdl_i2c_mem_iface_t);

extern const hdl_i2c_mem_iface_t hdl_i2c_mem_iface;

__STATIC_INLINE hdl_i2c_mem_state_t hdl_i2c_mem_state(const void *desc) {
  MODULE_ASSERT(desc, HDL_I2C_MEM_XFER_FAIL);
  return ((hdl_i2c_mem_iface_t *)((hdl_module_base_t *)desc)->iface)->state(desc);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_read(const void *desc, uint8_t dev_addr, uint8_t r_addr_sz, uint32_t r_addr, uint8_t *data, uint32_t size) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_mem_iface_t *)((hdl_module_base_t *)desc)->iface)->read(desc, dev_addr, r_addr_sz, r_addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_read_r1(const void *desc, uint8_t dev_addr, uint32_t addr, uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_read(desc, dev_addr, 1, addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_read_r2(const void *desc, uint8_t dev_addr, uint32_t addr, uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_read(desc, dev_addr, 2, addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_read_r4(const void *desc, uint8_t dev_addr, uint32_t addr, uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_read(desc, dev_addr, 4, addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_write(const void *desc, uint8_t dev_addr, uint8_t r_addr_sz, uint32_t r_addr, const uint8_t *data, uint32_t size) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_mem_iface_t *)((hdl_module_base_t *)desc)->iface)->write(desc, dev_addr, r_addr_sz, r_addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_write_r1(const void *desc, uint8_t dev_addr, uint32_t addr, const uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_write(desc, dev_addr, 1, addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_write_r2(const void *desc, uint8_t dev_addr, uint32_t addr, const uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_write(desc, dev_addr, 2, addr, data, size);
}

__STATIC_INLINE uint8_t hdl_i2c_mem_write_r4(const void *desc, uint8_t dev_addr, uint32_t addr, const uint8_t *data, uint32_t size) {
  return hdl_i2c_mem_write(desc, dev_addr, 4, addr, data, size);
}

#endif /* HDL_I2C_MEM_H_ */
