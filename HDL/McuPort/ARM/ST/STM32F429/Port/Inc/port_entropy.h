#ifndef PORT_ENTROPY_H_
#define PORT_ENTROPY_H_

#include "hdl_entropy.h"

/* depends on:
  hdl_clock_t AHB2 (AHB)
 */
hdl_module_new_t(hdl_entropy_hw_mcu_t, 0, void *, hdl_entropy_iface_t);

extern const hdl_entropy_iface_t hdl_entropy_iface;

#endif /* PORT_ENTROPY_H_ */
