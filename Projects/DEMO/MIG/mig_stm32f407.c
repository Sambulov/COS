/*
  Module Init Graph (MIG)
*/

#ifdef STM32F407VG

#include "hdl_iface.h"

/* Note append mod_clock_pll dependency when using pll */
#define HDL_HXTAL_CLOCK                   8000000
#define HDL_PLL_CLOCK_SRC                 &mod_clock_hxtal  /* Can be clocked by: mod_clock_irc16m, mod_clock_hxtal */
#define HDL_SYS_CLOCK_SRC                 &mod_clock_pll_p, &mod_clock_pll /* Can be clocked by: mod_clock_pll_p, mod_clock_hxtal, mod_clock_irc16m */
#define HDL_PLL_M                         4                /* Can be 2, 3 .. 63 */
#define HDL_PLL_N                         168              /* Can be 50, 51, 52, .. 432 */
#define HDL_PLL_P                         2                /* Can be 2, 4, 6, 8 */
#define HDL_PLL_Q                         7                /* Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                4                /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                2                /* Can be 1, 2, 4, 8, 16 */
#define HDL_SYSTICK_PRESCALER             1                /* Can be 1 or 8 */

#define HDL_SYSTICK_COUNTER_RELOAD        168000 - 1       /* Clocked by SYS */


#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern const void *_heap_start;
extern const void *_heap_size;

const hdl_mem_block_t hdl_heap_regions[] = {
  { &_heap_start, (uint32_t)&_heap_size },
  { NULL, 0 } /* Terminates the array. */  
};

const hdl_module_base_t hdl_null_module = {
  .iface = NULL,
  .dependencies = NULL,
  .config = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_core_config_t mod_sys_core_cnf = {
  .phy = SCB_BASE,
  .flash_latency = FLASH_LATENCY_7,
  .compensation_cell = 1
};

const hdl_core_arm_t mod_sys_core = {
  .iface = &hdl_core_arm_iface,
  .dependencies = NULL,
  .config = &mod_sys_core_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};
/***********************************************************
 *                          IRQ
***********************************************************/
hdl_interrupt_t mod_irq_systick = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_EXCEPTION_SysTick,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_timer1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ25_TIM1_UP_TIM10,
    .priority = 0,
    .priority_group = 1
  )
};

hdl_interrupt_t mod_irq_timer2 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ28_TIM2,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_nmi = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_EXCEPTION_NonMaskableInt,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_timer4 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ30_TIM4,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c_1_ev = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ31_I2C1_EV,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_usart_1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ37_USART1,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_usart_3 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ39_USART3,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_usart_6 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ71_USART6,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_usart_2 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ38_USART2,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_i2c_1_err = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ32_I2C1_ER,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_exti_4 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ10_EXTI4,
    .priority = 0,
    .priority_group = 1
  )
};

hdl_interrupt_t mod_irq_exti_5_9 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ23_EXTI9_5,
    .priority = 1,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_spi_3 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ51_SPI3,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_adc = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ18_ADC,
    .priority = 0,
    .priority_group = 2
  )
};

/***********************************************************
 *                          NVIC
***********************************************************/
extern const hdl_nvic_config_t mod_nvic_cnf;

const void * const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
  &mod_nvic_cnf,
	&reset_handler,
	&nmi_handler,
	&hard_fault_handler,
	&mem_manage_handler,
	&bus_fault_handler,
	&usage_fault_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	&svc_handler,
	&debug_mon_handler,
	NULL,
	&pend_sv_handler,
	&systick_handler,
	&WWDG_IRQHandler,
	&PVD_IRQHandler,
	&TAMP_STAMP_IRQHandler,
	&RTC_WKUP_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_IRQHandler,
	&EXTI0_IRQHandler,
	&EXTI1_IRQHandler,
	&EXTI2_IRQHandler,
	&EXTI3_IRQHandler,
	&EXTI4_IRQHandler,
	&DMA1_Stream0_IRQHandler,
	&DMA1_Stream1_IRQHandler,
	&DMA1_Stream2_IRQHandler,
	&DMA1_Stream3_IRQHandler,
	&DMA1_Stream4_IRQHandler,
	&DMA1_Stream5_IRQHandler,
	&DMA1_Stream6_IRQHandler,
	&ADC_IRQHandler,
	&CAN1_TX_IRQHandler,
	&CAN1_RX0_IRQHandler,
	&CAN1_RX1_IRQHandler,
	&CAN1_SCE_IRQHandler,
	&EXTI9_5_IRQHandler,
	&TIM1_BRK_TIM9_IRQHandler,
	&TIM1_UP_TIM10_IRQHandler,
	&TIM1_TRG_COM_TIM11_IRQHandler,
	&TIM1_CC_IRQHandler,
	&TIM2_IRQHandler,
	&TIM3_IRQHandler,
	&TIM4_IRQHandler,
	&I2C1_EV_IRQHandler,
	&I2C1_ER_IRQHandler,
	&I2C2_EV_IRQHandler,
	&I2C2_ER_IRQHandler,
	&SPI1_IRQHandler,
	&SPI2_IRQHandler,
	&USART1_IRQHandler,
	&USART2_IRQHandler,
	&USART3_IRQHandler,
	&EXTI15_10_IRQHandler,
	&RTC_Alarm_IRQHandler,
	&OTG_FS_WKUP_IRQHandler,
	&TIM8_BRK_TIM12_IRQHandler,
	&TIM8_UP_TIM13_IRQHandler,
	&TIM8_TRG_COM_TIM14_IRQHandler,
	&TIM8_CC_IRQHandler,
	&DMA1_Stream7_IRQHandler,
	&FSMC_IRQHandler,
	&SDIO_IRQHandler,
	&TIM5_IRQHandler,
	&SPI3_IRQHandler,
	&UART4_IRQHandler,
	&UART5_IRQHandler,
	&TIM6_DAC_IRQHandler,
	&TIM7_IRQHandler,
	&DMA2_Stream0_IRQHandler,
	&DMA2_Stream1_IRQHandler,
	&DMA2_Stream2_IRQHandler,
	&DMA2_Stream3_IRQHandler,
	&DMA2_Stream4_IRQHandler,
	&ETH_IRQHandler,
	&ETH_WKUP_IRQHandler,
	&CAN2_TX_IRQHandler,
	&CAN2_RX0_IRQHandler,
	&CAN2_RX1_IRQHandler,
	&CAN2_SCE_IRQHandler,
	&OTG_FS_IRQHandler,
	&DMA2_Stream5_IRQHandler,
	&DMA2_Stream6_IRQHandler,
	&DMA2_Stream7_IRQHandler,
	&USART6_IRQHandler,
	&I2C3_EV_IRQHandler,
	&I2C3_ER_IRQHandler,
	&OTG_HS_EP1_OUT_IRQHandler,
	&OTG_HS_EP1_IN_IRQHandler,
	&OTG_HS_WKUP_IRQHandler,
	&OTG_HS_IRQHandler,
	&DCMI_IRQHandler,
	&irq_n_handler,
	&HASH_RNG_IRQHandler,
	&FPU_IRQHandler,
};

const hdl_nvic_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_nmi, &mod_irq_exti_4, &mod_irq_spi_3, &mod_irq_adc,
                              &mod_irq_i2c_1_ev, &mod_irq_i2c_1_err, &mod_irq_exti_5_9, &mod_irq_timer4,
                              &mod_irq_usart_1, &mod_irq_usart_2, &mod_irq_usart_6, &mod_irq_usart_3),
  .phy = NVIC
};

const hdl_nvic_t mod_nvic = {
  .iface = &hdl_nvic_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/***********************************************************
 *                          EXTI
***********************************************************/

const hdl_exti_t mod_nvic_exti_line_4 = {
  .line = HDL_EXTI_LINE_4,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

const hdl_exti_t mod_nvic_exti_line_5 = {
  .line = HDL_EXTI_LINE_5,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PD,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

const hdl_exti_controller_config_t mod_exti_cnf = {
  .extis = hdl_extis(&mod_nvic_exti_line_4, &mod_nvic_exti_line_5),
  .phy = (uint32_t)EXTI
};

const hdl_exti_controller_t mod_exti = {
  .iface = &hdl_exti_controller_iface,
  .dependencies = hdl_module_dependencies(&mod_nvic),
  .config = &mod_exti_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_clock_mcu_t mod_clock_irc16m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC16M, 
    .property.freq = 16000000,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_hxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_pll_m = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(HDL_PLL_CLOCK_SRC),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_M, 
    .property.div = HDL_PLL_M,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_n = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_pll_m),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_N,
    .property.mul = HDL_PLL_N,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_p = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_n),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_P,
    .property.div = HDL_PLL_P,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_q = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_n),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_Q,
    .property.div = HDL_PLL_Q,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_pll_t mod_clock_pll = {
  .iface = &hdl_clock_mcu_pll_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_n, &mod_clock_pll_p, &mod_clock_pll_q),
  .config = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_clock_mcu_t mod_clock_sys_clock = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core, &mod_clock_irc16m, HDL_SYS_CLOCK_SRC),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_SYS_SEL,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_ahb = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_sys_clock),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PRESCALER,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb1 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PRESCALER,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb2 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PRESCALER,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb1_timers = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB1_TIMERS,
    .property.mul = 4,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb2_timers = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB2_TIMERS,
    .property.mul = 2,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_mco1 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_p, &mod_clock_pll),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_MCO1,
    .property.div = 4,
    .phy = (uint32_t)RCC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

/**************************************************************
 *  Timers & Counters
 *************************************************************/

const hdl_tick_counter_t mod_systick_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .phy = (uint32_t)SysTick,
    .type.systick = hdl_module_config(hdl_tick_counter_systick_config_t, 
      .period = HDL_SYSTICK_COUNTER_RELOAD,
      .prediv = HDL_SYSTICK_PRESCALER
    )
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_time_counter_t mod_systick_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_systick_counter, &mod_nvic),
  .config = hdl_module_config(hdl_time_counter_config_t, .reload_interrupt = &mod_irq_systick),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE),
};

/**************************************************************
 *  GPIO
 *************************************************************/

 const hdl_gpio_port_mcu_t hdl_gpio_port_a = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOAEN,
    .phy = (uint32_t)GPIOA
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_b = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOBEN,
    .phy = (uint32_t)GPIOB
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_c = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOCEN,
    .phy = (uint32_t)GPIOC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_d = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIODEN,
    .phy = (uint32_t)GPIOD
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_e = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOEEN,
    .phy = (uint32_t)GPIOE
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_f = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOFEN,
    .phy = (uint32_t)GPIOF
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_g = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOGEN,
    .phy = (uint32_t)GPIOG
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_h = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOHEN,
    .phy = (uint32_t)GPIOH
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_i = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCC_AHB1ENR_GPIOIEN,
    .phy = (uint32_t)GPIOI
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_pin_hw_config_t mod_gpio_output_od_mode = {
  .mode = HDL_GPIO_MODE_OUTPUT,
  .otype = HDL_GPIO_OTYPE_OPEN_DRAIN,
  .ospeed = HDL_GPIO_SPEED_LOW,
  .pull = HDL_GPIO_PUPD_PULL_UP
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_a_mode = {
  .mode = HDL_GPIO_MODE_ANALOG,
  .pull = HDL_GPIO_PUPD_NONE,
  .ospeed = HDL_GPIO_SPEED_LOW,
};

const hdl_gpio_pin_hw_config_t mod_gpio_output_pp_mode = {
  .mode = HDL_GPIO_MODE_OUTPUT,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .ospeed = HDL_GPIO_SPEED_VERY_HIGH,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_pu_mode = {
  .mode = HDL_GPIO_MODE_INPUT,
  .pull = HDL_GPIO_PUPD_PULL_UP,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_pd_mode = {
  .mode = HDL_GPIO_MODE_INPUT,
  .pull = HDL_GPIO_PUPD_PULL_DOWN,
};

const hdl_gpio_pin_hw_config_t mod_gpio_uart1_2_3_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF7_USART1,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_PULL_UP
};

const hdl_gpio_pin_hw_config_t mod_gpio_uart4_5_6_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF8_USART6,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_PULL_UP
};

const hdl_gpio_pin_hw_config_t mod_gpio_spi3_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF6_SPI3,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_PULL_UP
};

const hdl_gpio_pin_t mod_gpio_pb3_spi_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_3)
}; //(PB3)

const hdl_gpio_pin_t mod_gpio_pb4_spi_miso = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PB4)

const hdl_gpio_pin_t mod_gpio_pb5_spi_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PB5)

const hdl_gpio_pin_t mod_gpio_pd7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PD7)

const hdl_gpio_pin_t mod_gpio_pd12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PD12)

const hdl_gpio_pin_t mod_gpio_pd13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PD13)

const hdl_gpio_pin_t mod_gpio_pd14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_14)
}; //(PD14)

const hdl_gpio_pin_t mod_gpio_pd15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PD15)

const hdl_gpio_pin_t mod_gpio_pa0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pd_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_0)
}; //(PA0)

const hdl_gpio_pin_t mod_gpio_pa9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PA9)

const hdl_gpio_pin_t mod_gpio_pa8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_8)
}; //(PA8)

const hdl_gpio_pin_t mod_gpio_pb9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PB9)

const hdl_gpio_pin_t mod_gpio_pa9_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PA9)

const hdl_gpio_pin_t mod_gpio_pa10_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PA10)

const hdl_gpio_pin_t mod_gpio_pd5_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PD5)

const hdl_gpio_pin_t mod_gpio_pd6_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PD6)

const hdl_gpio_pin_t mod_gpio_pc6_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PC6)

const hdl_gpio_pin_t mod_gpio_pc7_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PC7)

const hdl_gpio_pin_t mod_gpio_pc8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PB9)

const hdl_gpio_pin_t mod_gpio_pd8_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_8)
}; //(PD8)

const hdl_gpio_pin_t mod_gpio_pd9_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_2_3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PD9)


/**************************************************************
 *  UART
 *************************************************************/

 const hdl_uart_mcu_t mod_uart6 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pc7_uart_rx, 
    &mod_gpio_pc6_uart_tx, 
    &mod_clock_apb2, 
    &mod_nvic, 
    &mod_systick_timer,
    &mod_gpio_pc8
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)USART6,
    .rcu = RCC_APB2ENR_USART6EN,
    .interrupt = &mod_irq_usart_6,
    .baudrate = 9600,
    .word_len = HDL_UART_WORD_8BIT,
    .parity = HDL_UART_PARITY_NONE,
    .stop_bits = HDL_UART_STOP_BITS1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_UART_VAR_SIZE)
};

const hdl_uart_mcu_t mod_uart3 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pd9_uart_rx, 
    &mod_gpio_pd8_uart_tx, 
    &mod_clock_apb1, 
    &mod_nvic, 
    &mod_systick_timer,
    &hdl_null_module
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)USART3,
    .rcu = RCC_APB1ENR_USART3EN,
    .interrupt = &mod_irq_usart_3,
    .baudrate = 115200,
    .word_len = HDL_UART_WORD_8BIT,
    .parity = HDL_UART_PARITY_NONE,
    .stop_bits = HDL_UART_STOP_BITS1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_UART_VAR_SIZE)
};

/**************************************************************
 *                        SPI
 *************************************************************/

const hdl_spi_client_config_t hdl_spi_client_config = {
  .phy = (uint32_t)SPI3,
  .rcu = RCC_APB1ENR_SPI3EN,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = HDL_SPI_CK_PL_LOW_PH_1EDGE,
  .prescale = HDL_SPI_PSC_32,
  .interrupt = &mod_irq_spi_3,
};

const hdl_spi_client_mcu_t mod_spi3_client = {
  .iface = &hdl_spi_client_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pb5_spi_mosi, &mod_gpio_pb4_spi_miso, &mod_gpio_pb3_spi_sck,
                                          &mod_clock_apb1, &mod_nvic),
  .config = &hdl_spi_client_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_VAR_SIZE)
};

const hdl_spi_client_ch_mcu_t mod_spi3_ch1 = {
  .iface = &hdl_spi_client_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_spi3_client, &mod_gpio_pd7, &mod_systick_counter),
  .config = hdl_module_config(hdl_spi_client_ch_config_t, 
    .cs_min_delay = 168
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_VAR_SIZE)
};

/*================================================================*/

extern const hdl_interrupt_controller_t mod_ic                    __attribute__ ((alias ("mod_nvic")));

extern const hdl_tick_counter_t mod_tick_counter                  __attribute__ ((alias ("mod_systick_counter")));

extern const hdl_time_counter_t mod_timer_ms                      __attribute__ ((alias ("mod_systick_timer")));

extern const hdl_gpio_pin_t mod_output1_pin                       __attribute__ ((alias ("mod_gpio_pa8")));
extern const hdl_gpio_pin_t mod_output2_pin                       __attribute__ ((alias ("mod_gpio_pa9")));

extern const hdl_gpio_pin_t mod_led1_pin                          __attribute__ ((alias ("mod_gpio_pd12")));
extern const hdl_gpio_pin_t mod_led2_pin                          __attribute__ ((alias ("mod_gpio_pd13")));
extern const hdl_gpio_pin_t mod_led3_pin                          __attribute__ ((alias ("mod_gpio_pd14")));
extern const hdl_gpio_pin_t mod_led4_pin                          __attribute__ ((alias ("mod_gpio_pd15")));
extern const hdl_gpio_pin_t mod_button_pin                        __attribute__ ((alias ("mod_gpio_pa0")));

extern const hdl_uart_t mod_uart                                  __attribute__ ((alias ("mod_uart3")));
extern const hdl_uart_t mod_rs485                                 __attribute__ ((alias ("mod_uart6")));

extern const hdl_spi_client_ch_t mod_spi_client                   __attribute__ ((alias ("mod_spi3_ch1")));

#endif
