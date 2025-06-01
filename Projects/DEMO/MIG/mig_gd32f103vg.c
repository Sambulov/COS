/*
  Module Init Graph (MIG)
*/

#ifdef GD32F103

#include "app.h"

#define HDL_HXTAL_CLOCK              8000000
#define HDL_LXTAL_CLOCK              32768
#define HDL_HXTAL_2_PLLSEL_PREDIV    2
#define HDL_PLLMUL                   27
#define HDL_AHB_PREDIV               1
#define HDL_APB1_PREDIV              2
#define HDL_APB2_PREDIV              1
#define HDL_ADC_PREDIV               8
#define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
#define HDL_PLL_SEL_CLOCK            mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
#define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */
#define HDL_SYSTICK_COUNTER_RELOAD   108000 - 1                /* Clocked by AHB   */
#define HDL_I2C_SLAVE_ADDR           0x31

#define TIMER0_RELOAD_VAL            16000-1
#define TIMER1_RELOAD_VAL            16000-1

const hdl_module_base_t hdl_null_module = {
  .iface = NULL,
  .dependencies = NULL,
  .config = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern const void *_heap_start;
extern const void *_heap_size;

const hdl_mem_block_t hdl_heap_regions[] = {
  { &_heap_start, (uint32_t)&_heap_size },
  { NULL, 0 } /* Terminates the array. */  
};

const hdl_core_t mod_sys_core = {
  .iface = &hdl_core_arm_iface,
  .dependencies = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .config = hdl_module_config(hdl_core_config_t, 
    .flash_latency = WS_WSCNT_2,
    .phy = SCB_BASE
  )
};

/**************************************************************
 *  NVIC, IRQ, EXTI
 *************************************************************/

hdl_interrupt_t mod_irq_systick = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_EXCEPTION_SysTick,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_timer0_up = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ25_TIMER0_UP,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_timer1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ28_TIMER1,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_adc0_1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ18_ADC0_1,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c0_er = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c0_ev = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ31_I2C0_EV,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_spi_0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ35_SPI0,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_usart0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ37_USART0,
    .priority = 0,
    .priority_group = 0
  )
};

extern const hdl_nvic_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
	&mod_nvic_cnf,
	&reset_handler,         /* Reset Handler */
	&nmi_handler,           /* NMI Handler */
	&hard_fault_handler,     /* Hard Fault Handler */
	&mem_manage_handler,     /* MPU Fault Handler */
	&bus_fault_handler,      /* Bus Fault Handler */
	&usage_fault_handler,    /* Usage Fault Handler */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	&svc_handler,           /* SVCall Handler */
	&debug_mon_handler,      /* Debug Monitor Handler */
	((void *)0),            /* Reserved */
	&pend_sv_handler,        /* PendSV Handler */
	&systick_handler,       /* SysTick Handler */

  &wwdgt_handler,                    /* IRQ0 */
  &lvd_handler,                      /* IRQ1 */
  &TAMPER_IRQHandler,                   /* IRQ2 */
  &rtc_handler,                      /* IRQ3 */
  &fmc_handler,                      /* IRQ4 */
  &rcu_handler,                      /* IRQ5 */
  &EXTI0_IRQHandler,                    /* IRQ6 */
  &EXTI1_IRQHandler,                    /* IRQ7 */
  &EXTI2_IRQHandler,                    /* IRQ8 */
  &EXTI3_IRQHandler,                    /* IRQ9 */
  &EXTI4_IRQHandler,                    /* IRQ10 */
  &DMA0_Channel0_IRQHandler,            /* IRQ11 */
  &DMA0_Channel1_IRQHandler,            /* IRQ12 */
  &DMA0_Channel2_IRQHandler,            /* IRQ13 */
  &DMA0_Channel3_IRQHandler,            /* IRQ14 */
  &DMA0_Channel4_IRQHandler,            /* IRQ15 */
  &DMA0_Channel5_IRQHandler,            /* IRQ16 */
  &DMA0_Channel6_IRQHandler,            /* IRQ17 */
  &ADC0_1_IRQHandler,                   /* IRQ18 */
#ifdef GD32F10X_MD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &timer0_channel_handler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &timer2_handler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &i2c0_ev_handler,                  /* IRQ31 */
  &i2c0_er_handler,                  /* IRQ32 */
  &i2c1_ev_handler,                  /* IRQ33 */
  &i2c1_er_handler,                  /* IRQ34 */
  &spi0_handler,                     /* IRQ35 */
  &spi1_handler,                     /* IRQ36 */
  &usart0_handler,                   /* IRQ37 */
  &usart1_handler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &irq_n_handler,                       /* IRQ43 */
  &irq_n_handler,                       /* IRQ44 */
  &irq_n_handler,                       /* IRQ45 */
  &irq_n_handler,                       /* IRQ46 */
  &irq_n_handler,                       /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &irq_n_handler,                       /* IRQ49 */
  &irq_n_handler,                       /* IRQ50 */
  &irq_n_handler,                       /* IRQ51 */
  &irq_n_handler,                       /* IRQ52 */
  &irq_n_handler,                       /* IRQ53 */
  &irq_n_handler,                       /* IRQ54 */
  &irq_n_handler,                       /* IRQ55 */
  &irq_n_handler,                       /* IRQ56 */
  &irq_n_handler,                       /* IRQ57 */
  &irq_n_handler,                       /* IRQ58 */
  &irq_n_handler,                       /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_MD */
#ifdef GD32F10X_HD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &timer0_channel_handler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &timer2_handler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &i2c0_ev_handler,                  /* IRQ31 */
  &i2c0_er_handler,                  /* IRQ32 */
  &i2c1_ev_handler,                  /* IRQ33 */
  &i2c1_er_handler,                  /* IRQ34 */
  &spi0_handler,                     /* IRQ35 */
  &spi1_handler,                     /* IRQ36 */
  &usart0_handler,                   /* IRQ37 */
  &usart1_handler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &TIMER7_BRK_IRQHandler,               /* IRQ43 */
  &TIMER7_UP_IRQHandler,                /* IRQ44 */
  &TIMER7_TRG_CMT_IRQHandler,           /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */
  &ADC2_IRQHandler,                     /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &SDIO_IRQHandler,                     /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &timer5_handler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_4_IRQHandler,          /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_HD */
#ifdef GD32F10X_XD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_TIMER8_IRQHandler,        /* IRQ24 */
  &TIMER0_UP_TIMER9_IRQHandler,         /* IRQ25 */
  &TIMER0_TRG_CMT_TIMER10_IRQHandler,   /* IRQ26 */
  &timer0_channel_handler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &timer2_handler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &i2c0_ev_handler,                  /* IRQ31 */
  &i2c0_er_handler,                  /* IRQ32 */
  &i2c1_ev_handler,                  /* IRQ33 */
  &i2c1_er_handler,                  /* IRQ34 */
  &spi0_handler,                     /* IRQ35 */
  &spi1_handler,                     /* IRQ36 */
  &usart0_handler,                   /* IRQ37 */
  &usart1_handler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &TIMER7_BRK_TIMER11_IRQHandler,       /* IRQ43 */
  &TIMER7_UP_TIMER12_IRQHandler,        /* IRQ44 */
  &TIMER7_TRG_CMT_TIMER13_IRQHandler,   /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */
  &ADC2_IRQHandler,                     /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &SDIO_IRQHandler,                     /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &timer5_handler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_4_IRQHandler,          /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_XD */
#ifdef GD32F10X_CL
  &CAN0_TX_IRQHandler,                  /* IRQ19 */
  &CAN0_RX0_IRQHandler,                 /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &timer0_channel_handler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &timer2_handler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &i2c0_ev_handler,                  /* IRQ31 */
  &i2c0_er_handler,                  /* IRQ32 */
  &i2c1_ev_handler,                  /* IRQ33 */
  &i2c1_er_handler,                  /* IRQ34 */
  &spi0_handler,                     /* IRQ35 */
  &spi1_handler,                     /* IRQ36 */
  &usart0_handler,                   /* IRQ37 */
  &usart1_handler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBFS_WKUP_IRQHandler,               /* IRQ42 */
  &TIMER7_BRK_IRQHandler,               /* IRQ43 */
  &TIMER7_UP_IRQHandler,                /* IRQ44 */
  &TIMER7_TRG_CMT_IRQHandler,           /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */ 
  &irq_n_handler,                       /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &irq_n_handler,                       /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &timer5_handler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_IRQHandler,            /* IRQ59 */
  &DMA1_Channel4_IRQHandler,            /* IRQ60 */
  &ENET_IRQHandler,                     /* IRQ61 */
  &ENET_WKUP_IRQHandler,                /* IRQ62 */
  &CAN1_TX_IRQHandler,                  /* IRQ63 */
  &CAN1_RX0_IRQHandler,                 /* IRQ64 */
  &CAN1_RX1_IRQHandler,                 /* IRQ65 */
  &CAN1_EWMC_IRQHandler,                /* IRQ66 */
  &USBFS_IRQHandler,                    /* IRQ67 */
#endif /* GD32F10X_CL */
  &irq_n_handler,                       /* IRQ68 */
  &irq_n_handler,                       /* IRQ69 */
  &irq_n_handler,                       /* IRQ70 */
  &irq_n_handler,                       /* IRQ71 */
  &irq_n_handler,                       /* IRQ72 */
  &irq_n_handler,                       /* IRQ73 */
  &irq_n_handler,                       /* IRQ74 */
  &irq_n_handler,                       /* IRQ75 */
  &irq_n_handler,                       /* IRQ76 */
  &irq_n_handler,                       /* IRQ77 */
  &irq_n_handler,                       /* IRQ78 */
  &irq_n_handler,                       /* IRQ79 */
};

const hdl_nvic_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .irq_latency = 0, /* TODO: define static assert */
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0_up, &mod_irq_timer1, &mod_irq_adc0_1, 
                               &mod_irq_i2c0_er, &mod_irq_i2c0_ev, &mod_irq_spi_0, &mod_irq_usart0),
  .phy = NVIC,
};

const hdl_nvic_t mod_nvic = {
  .iface = &hdl_nvic_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/**************************************************************
 *  Clock
 *************************************************************/

const hdl_clock_mcu_t mod_clock_irc8m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000,
    .phy = (uint32_t)RCU
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
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_lxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_irc40k = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_sel = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&HDL_PLL_SEL_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_sel),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_system = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core, &mod_clock_irc8m, &HDL_SYS_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_SYS_SEL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_rtc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_RTC_SEL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_ahb = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_system),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb1 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb2 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_adc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_ADC,
    .property.div = HDL_ADC_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_tick_counter_t mod_systick_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .phy = (uint32_t)SysTick,
    .type.systick = hdl_module_config(hdl_tick_counter_systick_config_t, .period = HDL_SYSTICK_COUNTER_RELOAD)
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_tick_counter_t mod_timer0_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .phy = (uint32_t)TIMER0,
    .type.timer = hdl_module_config(hdl_tick_counter_timer_config_t, 
      .alignedmode = TIMER_COUNTER_EDGE,
      .clockdivision = TIMER_CKDIV_DIV1,
      .counterdirection = TIMER_COUNTER_UP,
      .period = TIMER0_RELOAD_VAL,
      .prescaler = 0,
      .repetitioncounter = 0,
      .rcu = RCU_TIMER0
    )
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_tick_counter_t mod_timer1_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .phy = (uint32_t)TIMER1,
    .type.timer = hdl_module_config(hdl_tick_counter_timer_config_t, 
      .alignedmode = TIMER_COUNTER_EDGE,
      .clockdivision = TIMER_CKDIV_DIV1,
      .counterdirection = TIMER_COUNTER_UP,
      .period = TIMER1_RELOAD_VAL,
      .prescaler = 0,
      .repetitioncounter = 0,
      .rcu = RCU_TIMER1
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

const hdl_time_counter_t mod_timer0_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_timer0_counter, &mod_nvic),
  .config = hdl_module_config(hdl_time_counter_config_t, .reload_interrupt = &mod_irq_timer0_up),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE),
};

const hdl_time_counter_t mod_timer1_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_timer1_counter, &mod_nvic),
  .config = hdl_module_config(hdl_time_counter_config_t, .reload_interrupt = &mod_irq_timer1),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE),
};

/**************************************************************
 *  DMA
 *************************************************************/

const hdl_dma_mcu_t mod_dma0 = {
  .iface = &hdl_dma_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_dma_config_t,
    .rcu = RCU_DMA0,
    .phy = DMA0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_mcu_t mod_dma0_ch0 = { /* ADC */
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma0),
  .config = hdl_module_config(hdl_dma_channel_config_t,
    .priority = DMA_PRIORITY_LOW,
    .direction = DMA_PERIPHERAL_TO_MEMORY,
    .memory_width = DMA_MEMORY_WIDTH_32BIT,
    .periph_width = DMA_PERIPHERAL_WIDTH_16BIT,
    .memory_inc = 1,
    .periph_inc = 0,
    .circular = 1,
    .ch_no = DMA_CH0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_mcu_t mod_dma0_ch1 = { /* M2M */
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma0),
  .config = hdl_module_config(hdl_dma_channel_config_t,
    .priority = DMA_PRIORITY_LOW,
    .direction = DMA_PERIPHERAL_TO_MEMORY,
    .memory_width = DMA_MEMORY_WIDTH_32BIT,
    .periph_width = DMA_PERIPHERAL_WIDTH_32BIT,
    .memory_inc = 1,
    .periph_inc = 1,
    .m2m_direction = 1,
    .circular = 0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/**************************************************************
 *  GPIO
 *************************************************************/

const hdl_gpio_port_mcu_t hdl_gpio_port_a = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOA,
    .phy = GPIOA
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_b = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOB,
    .phy = GPIOB
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_c = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOC,
    .phy = GPIOC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_d = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOD,
    .phy = GPIOD
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_e = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOE,
    .phy = GPIOE
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_f = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOF,
    .phy = GPIOF
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_g = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOG,
    .phy = GPIOG
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_output_pp = {
  .ospeed = GPIO_OSPEED_2MHZ,
  .type = GPIO_MODE_OUT_PP
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_input_np = {
  .type = GPIO_MODE_IN_FLOATING
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_input_pu = {
  .type = GPIO_MODE_IPU
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_input_analog = {
  .type = GPIO_MODE_AIN
};

hdl_gpio_pin_hw_config_t mod_gpio_alternate_od_slow_mode = {
  .type = GPIO_MODE_AF_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

hdl_gpio_pin_hw_config_t mod_gpio_alternate_pp_fast_mode = {
  .type = GPIO_MODE_AF_PP,
  .ospeed = GPIO_OSPEED_50MHZ
};

hdl_gpio_pin_hw_config_t mod_gpio_alternate_pp_mid_mode = {
  .type = GPIO_MODE_AF_PP,
  .ospeed = GPIO_OSPEED_10MHZ
};

const hdl_gpio_pin_t mod_gpio_pa0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_input_pu,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PA0)

const hdl_gpio_pin_t mod_gpio_pa2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2)
}; //(PA2)

const hdl_gpio_pin_t mod_gpio_pa3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_input_analog, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PA3)

const hdl_gpio_pin_t mod_gpio_pa4_cs = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PA4)

const hdl_gpio_pin_t mod_gpio_pa5_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_pp_fast_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5)
}; //(PA5)

const hdl_gpio_pin_t mod_gpio_pa6_miso = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_pp_fast_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6)
}; //(PA6)

const hdl_gpio_pin_t mod_gpio_pa7_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_pp_fast_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PA7)

const hdl_gpio_pin_t mod_gpio_pa8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PA8)

const hdl_gpio_pin_t mod_gpio_pa9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_9)
}; //(PA9)

const hdl_gpio_pin_t mod_gpio_pa9_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_pp_mid_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9)
}; //(PA9)

const hdl_gpio_pin_t mod_gpio_pa10_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_pp_mid_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PA10)

const hdl_gpio_pin_t mod_gpio_pb6_scl = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_od_slow_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PB6)

const hdl_gpio_pin_t mod_gpio_pb7_sda = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_alternate_od_slow_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PB7)

const hdl_gpio_pin_t mod_gpio_pb12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PB12)

const hdl_gpio_pin_t mod_gpio_pb13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PB13)

const hdl_gpio_pin_t mod_gpio_pb14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_14)
}; //(PB14)

const hdl_gpio_pin_t mod_gpio_pb15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_mode_output_pp, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PB15)


/**************************************************************
 *  I2C
 *************************************************************/

hdl_i2c_channel_t i2c0_ch0 = {.address = HDL_I2C_SLAVE_ADDR};

const hdl_i2c_config_t mod_i2c0_cnf = {
  .channels = hdl_i2c_channels(&i2c0_ch0),
  .hwc = hdl_module_config(hdl_i2c_config_hw_t,
    .addr_10_bits = 0,
    .dtcy = I2C_DTCY_2,
    .dual_address = 0,
    .err_interrupt = &mod_irq_i2c0_er,
    .ev_interrupt = &mod_irq_i2c0_ev,
    .general_call_enable = 0,
    .speed = 400000,
    .stretch_enable = 1,
    .rcu = RCU_I2C0,
    .phy = I2C0
  )
};
 
const hdl_i2c_mcu_t mod_i2c0 = {
  .iface = &hdl_i2c_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pb6_scl, &mod_gpio_pb7_sda,
                                          &mod_clock_apb1, &mod_nvic, &mod_systick_timer),
  .config = &mod_i2c0_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_I2C_VAR_SIZE)
};

/**************************************************************
 *  SPI
 *************************************************************/

const hdl_spi_client_mcu_t mod_spi0 = {
  .iface = &hdl_spi_client_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pa7_mosi, &mod_gpio_pa6_miso, &mod_gpio_pa5_sck,
                                          &mod_clock_apb1, &mod_nvic),
  .config = hdl_module_config(hdl_spi_client_config_t,
    .phy = SPI0,
    .rcu = RCU_SPI0,
    .endian = HDL_SPI_ENDIAN_MSB,
    .polarity = SPI_CK_PL_HIGH_PH_2EDGE,
    .prescale = HDL_SPI_PSC_2,
    .spi_interrupt = &mod_irq_spi_0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_VAR_SIZE)
};

const hdl_spi_client_ch_mcu_t mod_spi0_ch0 = {
  .iface = &hdl_spi_client_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_spi0, &mod_gpio_pa4_cs, &mod_systick_counter),
  .config = hdl_module_config(hdl_spi_client_ch_config_t, .cs_min_delay = 90),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_VAR_SIZE)
};

/**************************************************************
 *  ADC
 *************************************************************/

hdl_adc_source_t mod_adc_source_0 = {
  .channel = HDL_ADC_CHANNEL_0,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_1 = {
  .channel = HDL_ADC_CHANNEL_1,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_2 = {
  .channel = HDL_ADC_CHANNEL_2,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_3 = {
  .channel = HDL_ADC_CHANNEL_3,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

const hdl_adc_config_t mod_adc0_mcu_cnf = {
  .phy = ADC0,
  .rcu = RCU_ADC0,
  .adc_interrupt = &mod_irq_adc0_1,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_3),
};

const hdl_adc_mcu_t mod_adc0_mcu = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_adc, &mod_systick_timer, &mod_dma0_ch0, &mod_nvic, &mod_gpio_pa3),
  .config = &mod_adc0_mcu_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

/**************************************************************
 *  UART
 *************************************************************/

 const hdl_uart_mcu_t mod_uart0 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pa10_uart_rx,
    &mod_gpio_pa9_uart_tx,
    &mod_clock_apb2,
    &mod_nvic,
    &mod_systick_timer
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)USART0, 
    .rcu = RCU_USART0,
    .interrupt = &mod_irq_usart0,
    .baudrate = 115200,
    .word_len = USART_WL_8BIT,
    .parity = USART_PM_NONE,
    .stop_bits = USART_STB_1BIT
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_UART_VAR_SIZE)
};

/*================================================================*/

extern const hdl_interrupt_controller_t mod_ic                    __attribute__ ((alias ("mod_nvic")));

extern const hdl_tick_counter_t mod_tick_counter                  __attribute__ ((alias ("mod_systick_counter")));

extern const hdl_time_counter_t mod_timer_ms                      __attribute__ ((alias ("mod_systick_timer")));

extern const hdl_gpio_pin_t mod_output1_pin                       __attribute__ ((alias ("mod_gpio_pa8")));
extern const hdl_gpio_pin_t mod_output2_pin                       __attribute__ ((alias ("mod_gpio_pa9")));

extern const hdl_gpio_pin_t mod_led1_pin                          __attribute__ ((alias ("mod_gpio_pb12")));
extern const hdl_gpio_pin_t mod_led2_pin                          __attribute__ ((alias ("mod_gpio_pb13")));
extern const hdl_gpio_pin_t mod_led3_pin                          __attribute__ ((alias ("mod_gpio_pb14")));
extern const hdl_gpio_pin_t mod_led4_pin                          __attribute__ ((alias ("mod_gpio_pb15")));
extern const hdl_gpio_pin_t mod_button_pin                        __attribute__ ((alias ("mod_gpio_pa0")));

extern const hdl_uart_t mod_uart                                  __attribute__ ((alias ("mod_uart0")));
extern const hdl_uart_t mod_rs485                                 __attribute__ ((alias ("hdl_null_module")));

extern const hdl_spi_client_ch_t mod_spi_client                   __attribute__ ((alias ("mod_spi0_ch0")));
extern const hdl_i2c_t mod_i2c                                    __attribute__ ((alias ("mod_i2c0")));

#endif
