/*
  Module Init Graph (MIG)
*/

#ifdef STM32F407VG

#include "hdl_iface.h"

/* Note append mod_clock_pll dependency when using pll */
#define HDL_HXTAL_CLOCK                   16000000
#define HDL_PLL_CLOCK_SRC                 &mod_clock_hxtal  /* Can be clocked by: mod_clock_irc16m, mod_clock_hxtal */
#define HDL_SYS_CLOCK_SRC                 &mod_clock_pll_p, &mod_clock_pll /* Can be clocked by: mod_clock_pll_p, mod_clock_hxtal, mod_clock_irc16m */
#define HDL_PLL_M                         (HDL_HXTAL_CLOCK / 2000000) /* Can be 2, 3 .. 63 */
#define HDL_PLL_N                         168              /* Can be 50, 51, 52, .. 432 */
#define HDL_PLL_P                         2                /* Can be 2, 4, 6, 8 */
#define HDL_PLL_Q                         7                /* Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                4                /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                2                /* Can be 1, 2, 4, 8, 16 */
#define HDL_SYSTICK_PRESCALER             1                /* Can be 1 or 8 */
#define HDL_ADC_PRESCALER                 4                /* Can be 2, 4, 6, 8 (from APB2 clock; 36MHz max)*/

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

hdl_interrupt_t mod_irq_uart_5 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ53_UART5,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_uart_4 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ52_UART4,
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

hdl_interrupt_t mod_irq_can1_tx = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ19_CAN1_TX,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can1_rx0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ20_CAN1_RX0,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can1_rx1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ21_CAN1_RX1,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can1_sce = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ22_CAN1_SCE,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can2_tx = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ63_CAN2_TX,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can2_rx0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ64_CAN2_RX0,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can2_rx1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ65_CAN2_RX1,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_can2_sce = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ66_CAN2_SCE,
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

hdl_interrupt_t mod_irq_exti_10_15 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ40_EXTI15_10,
    .priority = 1,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_spi_2 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ36_SPI2,
    .priority = 0,
    .priority_group = 2
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
	&WWDG_ISR,
	&PVD_ISR,
	&TAMP_STAMP_ISR,
	&RTC_WKUP_ISR,
	&FLASH_ISR,
	&RCC_ISR,
	&EXTI0_ISR,
	&EXTI1_ISR,
	&EXTI2_ISR,
	&EXTI3_ISR,
	&EXTI4_ISR,
	&DMA1_Stream0_ISR,
	&DMA1_Stream1_ISR,
	&DMA1_Stream2_ISR,
	&DMA1_Stream3_ISR,
	&DMA1_Stream4_ISR,
	&DMA1_Stream5_ISR,
	&DMA1_Stream6_ISR,
	&ADC_ISR,
	&CAN1_TX_ISR,
	&CAN1_RX0_ISR,
	&CAN1_RX1_ISR,
	&CAN1_SCE_ISR,
	&EXTI9_5_ISR,
	&TIM1_BRK_TIM9_ISR,
	&TIM1_UP_TIM10_ISR,
	&TIM1_TRG_COM_TIM11_ISR,
	&TIM1_CC_ISR,
	&TIM2_ISR,
	&TIM3_ISR,
	&TIM4_ISR,
	&I2C1_EV_ISR,
	&I2C1_ER_ISR,
	&I2C2_EV_ISR,
	&I2C2_ER_ISR,
	&SPI1_ISR,
	&SPI2_ISR,
	&USART1_ISR,
	&USART2_ISR,
	&USART3_ISR,
	&EXTI15_10_ISR,
	&RTC_Alarm_ISR,
	&OTG_FS_WKUP_ISR,
	&TIM8_BRK_TIM12_ISR,
	&TIM8_UP_TIM13_ISR,
	&TIM8_TRG_COM_TIM14_ISR,
	&TIM8_CC_ISR,
	&DMA1_Stream7_ISR,
	&FSMC_ISR,
	&SDIO_ISR,
	&TIM5_ISR,
	&SPI3_ISR,
	&UART4_ISR,
	&UART5_ISR,
	&TIM6_DAC_ISR,
	&TIM7_ISR,
	&DMA2_Stream0_ISR,
	&DMA2_Stream1_ISR,
	&DMA2_Stream2_ISR,
	&DMA2_Stream3_ISR,
	&DMA2_Stream4_ISR,
	&ETH_ISR,
	&ETH_WKUP_ISR,
	&CAN2_TX_ISR,
	&CAN2_RX0_ISR,
	&CAN2_RX1_ISR,
	&CAN2_SCE_ISR,
	&OTG_FS_ISR,
	&DMA2_Stream5_ISR,
	&DMA2_Stream6_ISR,
	&DMA2_Stream7_ISR,
	&USART6_ISR,
	&I2C3_EV_ISR,
	&I2C3_ER_ISR,
	&OTG_HS_EP1_OUT_ISR,
	&OTG_HS_EP1_IN_ISR,
	&OTG_HS_WKUP_ISR,
	&OTG_HS_ISR,
	&DCMI_ISR,
	&irq_n_handler,
	&HASH_RNG_ISR,
	&FPU_ISR,
};

const hdl_nvic_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_nmi, &mod_irq_exti_4, &mod_irq_spi_2, &mod_irq_spi_3, &mod_irq_adc,
                              &mod_irq_i2c_1_ev, &mod_irq_i2c_1_err, &mod_irq_exti_10_15, &mod_irq_timer4,
                              &mod_irq_usart_1, &mod_irq_usart_2, &mod_irq_usart_3, &mod_irq_uart_4, &mod_irq_uart_5, &mod_irq_usart_6,
                              &mod_irq_can1_tx, &mod_irq_can1_rx0, &mod_irq_can1_rx1, &mod_irq_can1_sce,
                              &mod_irq_can2_tx, &mod_irq_can2_rx0, &mod_irq_can2_rx1, &mod_irq_can2_sce),
  .phy = NVIC
};

const hdl_nvic_t mod_nvic = {
  .iface = &hdl_nvic_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/***********************************************************
 *                          CLOCK
***********************************************************/

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

/***********************************************************
 *                         CRYPTO
***********************************************************/

const hdl_entropy_hw_mcu_t mod_entropy = {
  .iface = &hdl_entropy_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/***********************************************************
 *                          EXTI
***********************************************************/

const hdl_exti_t mod_nvic_exti_line_14 = {
  .line = HDL_EXTI_LINE_14,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

const hdl_exti_controller_t mod_exti = {
  .iface = &hdl_exti_controller_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_exti_controller_config_t,
    .phy = (uint32_t)EXTI,
    .extis = hdl_extis(&mod_nvic_exti_line_14)
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/**************************************************************
 *  Timers & Counters
 *************************************************************/

const hdl_systick_counter_t mod_systick_counter = {
  .iface = &hdl_systick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_systick_counter_config_t,
    .phy = (uint32_t)SysTick,
    .period = HDL_SYSTICK_COUNTER_RELOAD,
    #if (HDL_SYSTICK_PRESCALER == 8)
      .clock_src = 0
    #else
      .clock_src = SysTick_CTRL_CLKSOURCE_Msk
    #endif
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_timertick_counter_t mod_timer2_counter = {
  .iface = &hdl_timertick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1_timers),
  .config = hdl_module_config(hdl_timertick_counter_config_t,
    .phy = (uint32_t)TIM2,
    .rcc = RCC_APB1ENR_TIM2EN,
    .counter_mode = TIM_COUNTERMODE_UP,
    .clock_div = TIM_CLOCKDIVISION_DIV1,
    .prescaler = 20, /* 84MHz / (20+1) = 4MHz */
    .period = 4000 - 1, /* 1 KHz */
    .trgo_mode = TIM_TRGO_OC1REF, /* trigger for adc */
    .repetition_counter = 0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_timertick_counter_t mod_timer4_counter = {
  .iface = &hdl_timertick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1_timers),
  .config = hdl_module_config(hdl_timertick_counter_config_t,
    .phy = (uint32_t)TIM4,
    .rcc = RCC_APB1ENR_TIM4EN,
    .counter_mode = TIM_COUNTERMODE_UP,
    .clock_div = TIM_CLOCKDIVISION_DIV1,
    .prescaler = 6, /* 84MHz / (6+1) = 12MHz */
    .period = 600 - 1, /* 20 KHz */
    .repetition_counter = 0
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

const hdl_gpio_pin_hw_config_t mod_gpio_can1_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF9_CAN1,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_PULL_UP
};

const hdl_gpio_pin_hw_config_t mod_gpio_can2_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF9_CAN2,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_PULL_UP
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

const hdl_gpio_pin_hw_config_t mod_gpio_spi2_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF5_SPI2,
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

const hdl_gpio_pin_hw_config_t mod_gpio_tim4_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF2_TIM4,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_NONE
};

const hdl_gpio_pin_hw_config_t mod_gpio_tim2_mode = {
  .mode = HDL_GPIO_MODE_AF,
  .af = GPIO_AF1_TIM2,
  .ospeed = HDL_GPIO_SPEED_HIGH,
  .otype = HDL_GPIO_OTYPE_PUSH_PULL,
  .pull = HDL_GPIO_PUPD_NONE
};

const hdl_gpio_pin_t mod_gpio_pa0_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_0)
}; //(PA0)

const hdl_gpio_pin_t mod_gpio_pa3_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PA3)

const hdl_gpio_pin_t mod_gpio_pa4_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_4)
}; //(PA4)

const hdl_gpio_pin_t mod_gpio_pa5 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_od_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PA5)

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
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PA8)

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

const hdl_gpio_pin_t mod_gpio_pa11 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11)
}; //(PA11)

const hdl_gpio_pin_t mod_gpio_pa12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PA12)

const hdl_gpio_pin_t mod_gpio_pa15_tim2_ch1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_tim2_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PA15)


const hdl_gpio_pin_t mod_gpio_pb1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_1)
}; //(PB1)

const hdl_gpio_pin_t mod_gpio_pb2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_2)
}; //(PB2)

const hdl_gpio_pin_t mod_gpio_pb3_spi_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_3)
}; //(PB3)

const hdl_gpio_pin_t mod_gpio_pb3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PB3)

const hdl_gpio_pin_t mod_gpio_pb4 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_4)
}; //(PB4)

const hdl_gpio_pin_t mod_gpio_pb4_spi_miso = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PB4)

const hdl_gpio_pin_t mod_gpio_pb5 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5)
}; //(PB5)

const hdl_gpio_pin_t mod_gpio_pb5_can2_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_can2_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PB5)

const hdl_gpio_pin_t mod_gpio_pb6_can2_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_can2_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PB6)

const hdl_gpio_pin_t mod_gpio_pb7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PB7)

const hdl_gpio_pin_t mod_gpio_pb8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PB8)

const hdl_gpio_pin_t mod_gpio_pb9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PB9)

const hdl_gpio_pin_t mod_gpio_pb10_spi_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi2_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PB10)

const hdl_gpio_pin_t mod_gpio_pb13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_13)
}; //(PB13)

const hdl_gpio_pin_t mod_gpio_pb14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_14)
}; //(PB14)

const hdl_gpio_pin_t mod_gpio_pb15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_15)
}; //(PB15)

const hdl_gpio_pin_t mod_gpio_pc0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PC0)

const hdl_gpio_pin_t mod_gpio_pc2_spi_miso = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi2_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_2)
}; //(PC2)

const hdl_gpio_pin_t mod_gpio_pc3_spi_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi2_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_3)
}; //(PC3)

const hdl_gpio_pin_t mod_gpio_pc6_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PC6)

const hdl_gpio_pin_t mod_gpio_pc6 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
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

const hdl_gpio_pin_t mod_gpio_pc7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PC7)

const hdl_gpio_pin_t mod_gpio_pc8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_8)
}; //(PC8)

const hdl_gpio_pin_t mod_gpio_pc9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PC9)

const hdl_gpio_pin_t mod_gpio_pc10_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PC10)

const hdl_gpio_pin_t mod_gpio_pc10_spi_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PC10)

const hdl_gpio_pin_t mod_gpio_pc11_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_11)
}; //(PC11)

const hdl_gpio_pin_t mod_gpio_pc12_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_12)
}; //(PC12)

const hdl_gpio_pin_t mod_gpio_pc12_spi_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_spi3_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_12)
}; //(PC12)

const hdl_gpio_pin_t mod_gpio_pd0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PD0)

const hdl_gpio_pin_t mod_gpio_pd0_can1_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_can1_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PD0)

const hdl_gpio_pin_t mod_gpio_pd1_can1_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_can1_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_1)
}; //(PD1)


const hdl_gpio_pin_t mod_gpio_pd2_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart4_5_6_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_2)
}; //(PD2)

const hdl_gpio_pin_t mod_gpio_pd3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_3)
}; //(PD3)

const hdl_gpio_pin_t mod_gpio_pd4 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PD4)

const hdl_gpio_pin_t mod_gpio_pd5 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PD5)

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

const hdl_gpio_pin_t mod_gpio_pd6 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6)
}; //(PD6)

const hdl_gpio_pin_t mod_gpio_pd7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PD7)

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


const hdl_gpio_pin_t mod_gpio_pd12_tim4_ch1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_tim4_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PD12)

const hdl_gpio_pin_t mod_gpio_pd13_tim4_ch2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_tim4_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PD13)

const hdl_gpio_pin_t mod_gpio_pd14_tim4_ch3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_tim4_mode, 
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

const hdl_gpio_pin_t mod_gpio_pe0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_od_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PE0)

const hdl_gpio_pin_t mod_gpio_pe1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_1)
}; //(PE1)

const hdl_gpio_pin_t mod_gpio_pe2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_2)
}; //(PE2)

const hdl_gpio_pin_t mod_gpio_pe3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_3)
}; //(PE3)

const hdl_gpio_pin_t mod_gpio_pe4 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PE4)

const hdl_gpio_pin_t mod_gpio_pe5 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_5)
}; //(PE5)

const hdl_gpio_pin_t mod_gpio_pe6 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PE6)

const hdl_gpio_pin_t mod_gpio_pe7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PE7)

const hdl_gpio_pin_t mod_gpio_pe8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PE8)

const hdl_gpio_pin_t mod_gpio_pe9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_od_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PE9)

const hdl_gpio_pin_t mod_gpio_pe10 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_10)
}; //(PE10)

const hdl_gpio_pin_t mod_gpio_pe11 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11)
}; //(PE11)

const hdl_gpio_pin_t mod_gpio_pe12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PE12)

const hdl_gpio_pin_t mod_gpio_pe13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PE13)

const hdl_gpio_pin_t mod_gpio_pe14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e, &mod_exti),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pu_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_14)
}; //(PE14)

const hdl_gpio_pin_t mod_gpio_pe15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PE15)

/**************************************************************
 *  DMA
 *************************************************************/

const hdl_dma_mcu_t mod_dma2 = {
  .iface = &hdl_dma_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_dma_config_t,
    .rcc = RCC_AHB1ENR_DMA2EN,
    .phy = (uint32_t)DMA2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

hdl_dma_channel_mcu_t mod_dma_ch_adc1 = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma2),
  .config = hdl_module_config(hdl_dma_channel_config_t,
    .priority = DMA_PRIORITY_LOW,
    .direction = DMA_PERIPH_TO_MEMORY,
    .memory_width = DMA_MDATAALIGN_WORD,
    .periph_width = DMA_PDATAALIGN_WORD,
    .memory_inc = 1,
    .periph_inc = 0,
    .periph_ctrl = 0,
    .circular = 1,
    .channel_periphery = DMA_CHANNEL_0,
    .stream = HDL_DMA_STREAM_0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

hdl_dma_channel_mcu_t mod_dma_ch_m2m = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma2),
  .config = hdl_module_config(hdl_dma_channel_config_t,
    .priority = DMA_PRIORITY_LOW,
    .direction = DMA_MEMORY_TO_MEMORY,
    .memory_width = DMA_MDATAALIGN_BYTE,
    .periph_width = DMA_PDATAALIGN_BYTE,
    .memory_inc = 1,
    .periph_inc = 1,
    .periph_ctrl = 0,
    .circular = 0,
    .channel_periphery = DMA_CHANNEL_3,
    .stream = HDL_DMA_STREAM_1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/**************************************************************
 *  PWM
 *************************************************************/

const hdl_pwm_mcu_t mod_pwm_tim4_pd12 = {
  .iface = &hdl_pwm_mcu_iface,
  .dependencies = hdl_module_dependencies(&mod_timer4_counter, &mod_gpio_pd12_tim4_ch1),
  .config = hdl_module_config(hdl_pwm_mcu_config_t,
    .mode = TIM_OCMODE_PWM1,
    .pulse = 300,
    .fast_mode = TIM_OCFAST_DISABLE,
    .polarity = TIM_OCPOLARITY_LOW,
    .channel = TIM_CHANNEL_1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_pwm_mcu_t mod_pwm_tim4_pd13 = {
  .iface = &hdl_pwm_mcu_iface,
  .dependencies = hdl_module_dependencies(&mod_timer4_counter, &mod_gpio_pd13_tim4_ch2),
  .config = hdl_module_config(hdl_pwm_mcu_config_t,
    .mode = TIM_OCMODE_PWM1,
    .pulse = 300,
    .fast_mode = TIM_OCFAST_DISABLE,
    .polarity = TIM_OCPOLARITY_LOW,
    .channel = TIM_CHANNEL_2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_pwm_mcu_t mod_pwm_tim4_pd14 = {
  .iface = &hdl_pwm_mcu_iface,
  .dependencies = hdl_module_dependencies(&mod_timer4_counter, &mod_gpio_pd14_tim4_ch3),
  .config = hdl_module_config(hdl_pwm_mcu_config_t,
    .mode = TIM_OCMODE_PWM1,
    .pulse = 300,
    .fast_mode = TIM_OCFAST_DISABLE,
    .polarity = TIM_OCPOLARITY_LOW,
    .channel = TIM_CHANNEL_3
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_pwm_mcu_t mod_pwm_tim2_pa15 = {
  .iface = &hdl_pwm_mcu_iface,
  .dependencies = hdl_module_dependencies(&mod_timer2_counter, &mod_gpio_pa15_tim2_ch1),
  .config = hdl_module_config(hdl_pwm_mcu_config_t,
    .mode = TIM_OCMODE_PWM1,
    .pulse = 4000,
    .fast_mode = TIM_OCFAST_DISABLE,
    .polarity = TIM_OCPOLARITY_LOW,
    .channel = TIM_CHANNEL_1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/**************************************************************
 *  ADC
 *************************************************************/

const hdl_adc_common_mcu_t mod_adc_common = {
  .iface = &hdl_adc_common_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_adc_common_config_t,
    .phy = (uint32_t)ADC,
    .rcc = RCC_APB2ENR_ADC1EN,
    .prescaler = HDL_ADC_PRESCALER,
    .multi_mode = ADC_MODE_INDEPENDENT,
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_adc_mcu_t mod_adc1 = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_adc_common, &mod_dma_ch_adc1, &hdl_null_module),
  .config = hdl_module_config(hdl_adc_config_t,
    .phy = (uint32_t)ADC1,
    .rcc = RCC_APB2ENR_ADC1EN,
    .adc_interrupt = &mod_irq_adc,
    .resolution = ADC_RESOLUTION_12B,
    .data_alignment = ADC_DATAALIGN_RIGHT,
    .regular_trg_edge = ADC_EXTERNALTRIGCONVEDGE_NONE,
    .injected_trg_edge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE,
    .adc_slots = (uint32_t *)static_malloc(8)
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

const hdl_adc_mcu_t mod_adc2 = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_adc_common, &hdl_null_module, &mod_timer2_counter),
  .config = hdl_module_config(hdl_adc_config_t,
    .phy = (uint32_t)ADC2,
    .rcc = RCC_APB2ENR_ADC2EN,
    .resolution = ADC_RESOLUTION_12B,
    .data_alignment = ADC_DATAALIGN_RIGHT,
    .regular_trg_edge = ADC_EXTERNALTRIGCONVEDGE_NONE,
    .injected_ext_trg = ADC_EXTERNALTRIGINJECCONV_T2_TRGO,
    .injected_trg_edge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING,
    .adc_slots = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

const hdl_adc_mcu_t mod_adc3 = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_adc_common, &hdl_null_module, &mod_timer2_counter),
  .config = hdl_module_config(hdl_adc_config_t,
    .phy = (uint32_t)ADC3,
    .rcc = RCC_APB2ENR_ADC3EN,
    .resolution = ADC_RESOLUTION_12B,
    .data_alignment = ADC_DATAALIGN_RIGHT,
    .regular_trg_edge = ADC_EXTERNALTRIGCONVEDGE_NONE,
    .injected_ext_trg = ADC_EXTERNALTRIGINJECCONV_T2_TRGO,
    .injected_trg_edge = ADC_EXTERNALTRIGINJECCONVEDGE_FALLING,
    .adc_slots = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

const hdl_adc_ch_mcu_t mod_adc1_ch3 = {
  .iface = &hdl_adc_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_adc1, &mod_gpio_pa3_adc),
  .config = hdl_module_config(hdl_adc_ch_config_t,
    .channel = ADC_CHANNEL_3,
    .sample_time = ADC_SAMPLETIME_112CYCLES,
    .rank = HDL_ADC_CH_RANK_REGULAR1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_adc_ch_mcu_t mod_adc1_ch4 = {
  .iface = &hdl_adc_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_adc1, &mod_gpio_pa4_adc),
  .config = hdl_module_config(hdl_adc_ch_config_t,
    .channel = ADC_CHANNEL_4,
    .sample_time = ADC_SAMPLETIME_112CYCLES,
    .rank = HDL_ADC_CH_RANK_REGULAR2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_adc_ch_mcu_t mod_adc2_ch0 = {
  .iface = &hdl_adc_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_adc2, &mod_gpio_pa0_adc),
  .config = hdl_module_config(hdl_adc_ch_config_t,
    .channel = ADC_CHANNEL_0,
    .sample_time = ADC_SAMPLETIME_480CYCLES,
    .rank = HDL_ADC_CH_RANK_INJECTED1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_adc_ch_mcu_t mod_adc3_ch0 = {
  .iface = &hdl_adc_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_adc3, &mod_gpio_pa0_adc),
  .config = hdl_module_config(hdl_adc_ch_config_t,
    .channel = ADC_CHANNEL_0,
    .sample_time = ADC_SAMPLETIME_480CYCLES,
    .rank = HDL_ADC_CH_RANK_INJECTED1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/**************************************************************
 *  CAN
 *************************************************************/

const hdl_can_mcu_base_t mod_can_base = {
  .iface = &hdl_can_mcu_base_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1),
  .config = 14,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_can_filter_t *mod_can1_filters[] = {
  /* Filter ext ID */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 0,
    .id = 0x12345678,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter ext ID by mask */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 1,
    .id = 0x12345679,
    .id_mask = HDL_CAN_IDE_MASK,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter ext ID */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 0,
    .id = 0x1234567A,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter standart ID */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 0,
    .id = 0x734,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter standart ID by mask */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 1,
    .id = 0x735,
    .id_mask = HDL_CAN_ID_MASK,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter standart ID */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 0,
    .id = 0x736,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter standart ID */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 0,
    .id = 0x737,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter standart ID by mask */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 1,
    .id = 0x738,
    .id_mask = HDL_CAN_ID_MASK,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* End of filters chain */
  NULL
};

const hdl_can_mcu_t mod_can1 = {
  .iface = &hdl_can_mcu_iface,
  .dependencies = hdl_module_dependencies(
    &mod_can_base,
    &mod_gpio_pd0_can1_rx,
    &mod_gpio_pd1_can1_tx
  ),
  .config = hdl_module_config(hdl_can_mcu_config_t,
    .phy = (uint32_t)CAN1,
    .rcc = RCC_APB1ENR_CAN1EN,
    .prescaler = 6, /* 42MHz/6 = 7MHz */
    .time_seg1 = CAN_BS1_11TQ,
    .time_seg2 = CAN_BS2_2TQ, /* sample time 12:2 ~ 85.71% */
    .sync_jump_width = CAN_SJW_1TQ, /* Bit time (syn+seg1+seg2) = 14QT; speed: 7MHz/14 = 500000 */
    .mode = CAN_MODE_NORMAL,
    .time_triggered_mode = 0,
    .auto_bus_off = 1,
    .auto_wake_up = 0,
    .auto_retransmission = 0,
    .receive_fifo_locked = 1,
    .transmit_fifo_priority = 1,
    .filters = mod_can1_filters
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CAN_VAR_SIZE)
};

const hdl_can_filter_t *mod_can2_filters[] = {
  /* Filter ext ID */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 0,
    .id = 0x12345678,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter ext ID */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 0,
    .id = 0x1234567A,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter standart ID */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 0,
    .id = 0x737,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter standart ID by mask */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 1,
    .id = 0x738,
    .id_mask = HDL_CAN_ID_MASK,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* Filter ext ID by mask */
  &(const hdl_can_filter_t) {
    .id_ex = 1,
    .masked = 1,
    .id = 0x12345679,
    .id_mask = HDL_CAN_IDE_MASK,
    .rtr = 0,
    .rtr_mask = 0
  },

  /* Filter standart ID */
  &(const hdl_can_filter_t) {
    .id_ex = 0,
    .masked = 0,
    .id = 0x734,
    .rtr = 0,
    .rtr_mask = 0,
  },

  /* End of filters chain */
  NULL
};

const hdl_can_mcu_t mod_can2 = {
  .iface = &hdl_can_mcu_iface,
  .dependencies = hdl_module_dependencies(
    &mod_can_base,
    &mod_gpio_pb5_can2_rx,
    &mod_gpio_pb6_can2_tx
  ),
  .config = hdl_module_config(hdl_can_mcu_config_t,
    .phy = (uint32_t)CAN2,
    .rcc = RCC_APB1ENR_CAN2EN,
    .prescaler = 24, /* 42MHz/24 = 1,75MHz */
    .time_seg1 = CAN_BS1_11TQ,
    .time_seg2 = CAN_BS2_2TQ, /* sample time 12:2 ~ 85.71% */
    .sync_jump_width = CAN_SJW_1TQ, /* Bit time (syn+seg1+seg2) = 14QT; speed: 1,75MHz/14 = 125000 */
    .mode = CAN_MODE_NORMAL,
    .time_triggered_mode = 0,
    .auto_bus_off = 1,
    .auto_wake_up = 0,
    .auto_retransmission = 0,
    .receive_fifo_locked = 1,
    .transmit_fifo_priority = 1,
    .filters = mod_can2_filters
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CAN_VAR_SIZE)
};

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

const hdl_uart_mcu_t mod_uart5 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pd2_uart_rx, 
    &mod_gpio_pc12_uart_tx, 
    &mod_clock_apb1,
    &mod_nvic,
    &mod_systick_timer,
    &hdl_null_module
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)UART5,
    .rcu = RCC_APB1ENR_UART5EN,
    .interrupt = &mod_irq_uart_5,
    .baudrate = 115200,
    .word_len = HDL_UART_WORD_8BIT,
    .parity = HDL_UART_PARITY_NONE,
    .stop_bits = HDL_UART_STOP_BITS1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_UART_VAR_SIZE)
};

const hdl_uart_mcu_t mod_uart4 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pc11_uart_rx, 
    &mod_gpio_pc10_uart_tx, 
    &mod_clock_apb1,
    &mod_nvic,
    &mod_systick_timer,
    &mod_gpio_pa12
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)UART4,
    .rcu = RCC_APB1ENR_UART4EN,
    .interrupt = &mod_irq_uart_4,
    .baudrate = 115200,
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
    &mod_gpio_pa8
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

const hdl_uart_mcu_t mod_uart1 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pa10_uart_rx, 
    &mod_gpio_pa9_uart_tx, 
    &mod_clock_apb2, 
    &mod_nvic, 
    &mod_systick_timer,
    &mod_gpio_pa11
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)USART1,
    .rcu = RCC_APB2ENR_USART1EN,
    .interrupt = &mod_irq_usart_1,
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

const hdl_spi_client_mcu_t mod_spi2_client = {
  .iface = &hdl_spi_client_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pc3_spi_mosi, &mod_gpio_pc2_spi_miso, &mod_gpio_pb10_spi_sck,
                                          &mod_clock_apb1, &mod_nvic),
  .config = hdl_module_config(hdl_spi_client_config_t,
    .phy = (uint32_t)SPI2,
    .rcu = RCC_APB1ENR_SPI2EN,
    .endian = HDL_SPI_ENDIAN_MSB,
    .polarity = HDL_SPI_CK_PL_LOW_PH_1EDGE,
    .prescale = HDL_SPI_PSC_32,
    .interrupt = &mod_irq_spi_2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_VAR_SIZE)
};

const hdl_spi_client_ch_mcu_t mod_spi2_ch1 = {
  .iface = &hdl_spi_client_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_spi2_client, &mod_gpio_pc0),
  .config = hdl_module_config(hdl_spi_client_ch_config_t, 
    .cs_min_delay = 84
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_VAR_SIZE)
};

const hdl_spi_client_mcu_t mod_spi3_client = {
  .iface = &hdl_spi_client_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pc12_spi_mosi, &mod_gpio_pb4_spi_miso, &mod_gpio_pc10_spi_sck,
                                          &mod_clock_apb1, &mod_nvic),
  .config = hdl_module_config(hdl_spi_client_config_t,
    .phy = (uint32_t)SPI3,
    .rcu = RCC_APB1ENR_SPI3EN,
    .endian = HDL_SPI_ENDIAN_MSB,
    .polarity = HDL_SPI_CK_PL_HIGH_PH_2EDGE,
    .prescale = HDL_SPI_PSC_4, /* QCASPI_CLK_SPEED_MIN = 1039062 ~ QCASPI_CLK_SPEED_MAX = 16625000 ??? */
    .interrupt = &mod_irq_spi_3
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_VAR_SIZE)
};

const hdl_spi_client_ch_mcu_t mod_spi3_ch1 = {
  .iface = &hdl_spi_client_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_spi3_client, &mod_gpio_pd0),
  .config = hdl_module_config(hdl_spi_client_ch_config_t, 
    .cs_min_delay = 84
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_VAR_SIZE)
};

const hdl_spi_client_sw_t mod_spi_sw_client = {
  .iface = &hdl_spi_client_sw_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pb7 /* mosi */, 
    &hdl_null_module /* miso */, 
    &mod_gpio_pb8 /* sck */
  ),
  .config = hdl_module_config(hdl_spi_client_sw_config_t,
    .endian = HDL_SPI_SW_LITTLE_ENDIAN,
    .polarity = HDL_SPI_SW_CPOL0_1EDGE,
    .signal_min_delay = 0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_SW_VAR_SIZE)
};

const hdl_spi_client_ch_sw_t mod_spi_sw_ch1 = {
  .iface = &hdl_spi_client_ch_sw_iface,
  .dependencies = hdl_module_dependencies(&mod_spi_sw_client, &mod_gpio_pb9, &mod_systick_counter),
  .config = hdl_module_config(hdl_spi_client_ch_sw_config_t, 
    .cs_min_delay = 0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_SW_VAR_SIZE)
};

/**************************************************************
 *                        GPIO Exp
 *************************************************************/

const hdl_74hc595_port_t mod_gpio_expander = {
  .iface = &hdl_74hc595_port_iface,
  .dependencies = hdl_module_dependencies(&mod_spi_sw_ch1, &hdl_null_module, &hdl_null_module, &hdl_null_module),
  .config = hdl_module_config(hdl_74hc595_port_config_t, 
    .shift_reg_length = 8,
    .port_regs = static_malloc(2)
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_74HC595_PORT_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp0 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 0,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp1 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 1,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp2 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 2,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp3 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 3,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp4 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 4,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp5 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 5,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp6 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 6,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_74hc595_pin_t mod_gpio_exp7 = {
  .iface = &hdl_74hc595_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_expander),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = 7,
    .inactive_default = HDL_GPIO_LOW,
    .hwc = NULL
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/**************************************************************
 *                        FLASH
 *************************************************************/

const hdl_w25x20cl_t mod_w25x20cl = {
  .iface = &hdl_w25x20cl_iface,
  .dependencies = hdl_module_dependencies(&mod_spi2_ch1, &mod_systick_timer, &hdl_null_module, &hdl_null_module),
  .config = hdl_module_config(hdl_w25x20cl_config_t, 
    .write_time = 5,
    .page_size = 256,
    .sector_size = 4096,
    .size = 262144
  ),
  .obj_var = static_malloc(HDL_W25X20CL_VAR_SIZE),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/*================================================================*/

extern const hdl_interrupt_controller_t mod_ic                    __attribute__ ((alias ("mod_nvic")));

extern const hdl_tick_counter_t mod_tick_counter                  __attribute__ ((alias ("mod_systick_counter")));

extern const hdl_time_counter_t mod_timer_ms                      __attribute__ ((alias ("mod_systick_timer")));


extern const hdl_gpio_pin_t mod_output1_pin                       __attribute__ ((alias ("mod_gpio_pa9")));
extern const hdl_gpio_pin_t mod_output2_pin                       __attribute__ ((alias ("mod_gpio_pa11")));

extern const hdl_gpio_pin_t mod_led1_pin                          __attribute__ ((alias ("mod_gpio_pb2")));
extern const hdl_gpio_pin_t mod_led2_pin                          __attribute__ ((alias ("mod_gpio_pb3")));
extern const hdl_gpio_pin_t mod_led3_pin                          __attribute__ ((alias ("mod_gpio_pb4")));
extern const hdl_gpio_pin_t mod_led4_pin                          __attribute__ ((alias ("mod_gpio_pb5")));
extern const hdl_gpio_pin_t mod_button_pin                        __attribute__ ((alias ("mod_gpio_pb13")));

extern const hdl_uart_t mod_uart                                  __attribute__ ((alias ("hdl_null_module")));
extern const hdl_uart_t mod_rs485                                 __attribute__ ((alias ("hdl_null_module")));

extern const hdl_spi_client_ch_t mod_spi_client                   __attribute__ ((alias ("hdl_null_module")));

extern const hdl_i2c_t mod_i2c                                    __attribute__ ((alias ("hdl_null_module")));

#endif
