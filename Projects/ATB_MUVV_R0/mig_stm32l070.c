#include "hdl.h"

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
	&mod_nvic_cnf,
	&reset_handler,
	&nmi_handler,
	&hard_fault_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&svc_handler,
	NULL,
	NULL,
	&pend_sv_handler,
	&systick_handler,
	&WWDG_IRQHandler,
	&PVD_IRQHandler,
	&RTC_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_IRQHandler,
	&EXTI0_1_IRQHandler,
	&EXTI2_3_IRQHandler,
	&EXTI4_15_IRQHandler,
	NULL,
	&DMA1_Channel1_IRQHandler,
	&DMA1_Channel2_3_IRQHandler,
	&DMA1_Channel4_5_6_7_IRQHandler,
	&ADC1_COMP_IRQHandler,
	&LPTIM1_IRQHandler,
	&USART4_5_IRQHandler,
	&TIM2_IRQHandler,
	&TIM3_IRQHandler,
	&TIM6_IRQHandler,
	&TIM7_IRQHandler,
	NULL,
	&TIM21_IRQHandler,
	&I2C3_IRQHandler,
	&TIM22_IRQHandler,
	&I2C1_IRQHandler,
	&I2C2_IRQHandler,
	&SPI1_IRQHandler,
	&SPI2_IRQHandler,
	&USART1_IRQHandler,
	&USART2_IRQHandler,
	&LPUART1_IRQHandler,
	NULL,
	NULL,
};

hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1 = {
  .irq_type = HDL_NVIC_IRQ23_I2C1,
  .priority = 0,
  .priority_group = 0,
};

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_i2c1)
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = NULL, //hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf,
};
