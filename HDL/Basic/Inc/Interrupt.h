#ifndef HDL_INTERRUPT_H_
#define HDL_INTERRUPT_H_

#if defined ( NUM46X )

#endif

#if defined ( GD32E23X )
  typedef struct {
    IRQn_Type irq;
    uint32_t priority;


  } hdl_interrupt_t;
#endif

#if defined ( STM32L0XX )

#endif


#endif // HDL_INTERRUPT_H_
