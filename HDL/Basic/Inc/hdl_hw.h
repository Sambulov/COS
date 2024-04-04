#ifndef HDL_HW_H_
#define HDL_HW_H_

#define HDL_HW_PRIVATE_SIZE 20

typedef enum {
  HDL_HW_INIT_OK = 0,
  HDL_HW_INIT_ONGOING = 1,
  HDL_HW_INIT_FAILED = -1,
} hdl_init_state_t;

typedef hdl_init_state_t (*hdl_hardware_initializer_t)(void *desc, uint8_t enable);

typedef struct {
  uint8_t __private[HDL_HW_PRIVATE_SIZE];
  hdl_hardware_initializer_t init;
} hdl_hardware_t;

hdl_init_state_t hdl_hw_enable(hdl_hardware_t *desc, uint8_t enable);
hdl_init_state_t hdl_hw_state(hdl_hardware_t *desc);

#endif // HDL_HW_H_
