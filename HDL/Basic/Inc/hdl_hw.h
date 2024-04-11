#ifndef HDL_HW_H_
#define HDL_HW_H_

#define HDL_HW_PRIVATE_SIZE 20

typedef enum {
  HDL_HW_INIT_ONGOING = 1,
  HDL_HW_INIT_OK = 0,
  HDL_HW_INIT_TERMINATING = -1,
  HDL_HW_INIT_FAILED = -2,
  HDL_HW_INIT_UNKNOWN = HDL_HW_INIT_FAILED,
} hdl_init_state_t;

typedef hdl_init_state_t (*hdl_hardware_initializer_t)(void *desc, uint8_t enable);

typedef struct _HDL_HW_T_{
  uint8_t __private[HDL_HW_PRIVATE_SIZE];
  hdl_hardware_initializer_t init;
  struct _HDL_HW_T_ **dependencies;
  void *periph;
} hdl_hardware_t;

#define hdl_hw_dependencies(...) ((hdl_hardware_t *[]){__VA_ARGS__, NULL})

void hdl_hw_enable(hdl_hardware_t *desc);
void hdl_hw_kill(hdl_hardware_t *desc);
hdl_init_state_t hdl_hw_state(hdl_hardware_t *desc);

#endif // HDL_HW_H_
