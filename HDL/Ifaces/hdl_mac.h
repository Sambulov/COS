#ifndef HDL_MAC_H_
#define HDL_MAC_H_

#include "hdl_eth_phy.h"

typedef struct hdl_mac_buffer_s hdl_mac_buffer_t;

struct hdl_mac_buffer_s {
  void *data;
  uint32_t length;
  hdl_mac_buffer_t *next;
};

typedef struct {
  uint8_t mac_addr[6];
  uint32_t mtu;
} hdl_mac_config_t;

typedef uint8_t (*hdl_mac_transmit_t)(const void *desc, hdl_mac_buffer_t *data, uint32_t total_len);
typedef uint8_t (*hdl_mac_receive_t)(const void *desc, void **data);
typedef uint8_t (*hdl_mac_cnf_t)(const void *desc, hdl_mac_config_t *cnf);
typedef hdl_eth_phy_t *(*hdl_mac_get_phy_t)(const void *desc);
typedef uint8_t (*hdl_mac_buffer_allocator_t)(void *context, void **buffer);
typedef void (*hdl_mac_set_buffer_allocator_t)(const void *desc, hdl_mac_buffer_allocator_t allocator, void *context);

typedef struct {
  hdl_module_initializer_t init;
  hdl_mac_transmit_t transmit;
  hdl_mac_receive_t receive;
  hdl_mac_set_buffer_allocator_t set_buf_alloc;
  hdl_mac_cnf_t set_cnf;
  hdl_mac_cnf_t get_cnf;
  hdl_mac_get_phy_t get_phy;
} hdl_mac_iface_t;

hdl_module_new_t(hdl_mac_t, 0, void *, hdl_mac_iface_t);

__STATIC_INLINE uint8_t hdl_mac_receive(const void *desc, void **data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->receive(desc, data);
}

__STATIC_INLINE uint8_t hdl_mac_transmit(const void *desc, hdl_mac_buffer_t *data, uint32_t total_len) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->transmit(desc, data, total_len);
}

__STATIC_INLINE uint8_t hdl_mac_get_cnf(const void *desc, hdl_mac_config_t *cnf) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->get_cnf(desc, cnf);
}

__STATIC_INLINE uint8_t hdl_mac_set_cnf(const void *desc, hdl_mac_config_t *cnf) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_mac_t *)desc)->iface->set_cnf(desc, cnf);
}

__STATIC_INLINE hdl_eth_phy_t *hdl_mac_get_phy(const void *desc) {
  MODULE_ASSERT(desc, NULL);
  return ((hdl_mac_t *)desc)->iface->get_phy(desc);
}

__STATIC_INLINE void hdl_mac_set_buffer_allocator(const void *desc, hdl_mac_buffer_allocator_t allocator, void *context) {
  MODULE_ASSERT(desc, );
  ((hdl_mac_t *)desc)->iface->set_buf_alloc(desc, allocator, context);
}

#endif /* HDL_MAC_H_ */
