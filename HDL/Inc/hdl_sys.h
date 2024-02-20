#ifndef HDL_SYSTICK_H_
#define HDL_SYSTICK_H_


#if defined ( NUM46X )
  typedef struct {

  } hdl_sys_t;
#endif

#if defined ( GD32E23X )
  typedef struct {
    uint32_t ticks_per_ms;
  } hdl_sys_t;
#endif

#if defined ( STM32L0XX )
  typedef struct {

  } hdl_sys_t;
#endif


void hdl_system_init(const hdl_sys_t *desc);

uint32_t hdl_millis();

void hdl_delay(uint32_t ms);

#endif // HDL_SYSTICK_H_
