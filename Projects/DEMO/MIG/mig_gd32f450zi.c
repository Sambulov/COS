/*
  Module Init Graph (MIG)
*/

#ifdef GD32F450

#include "app.h"

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

#define HDL_HXTAL_CLOCK                   20000000
#define HDL_PLL_MUL_CLOCK                 mod_clock_hxtal  /* Can be clocked by: mod_clock_irc16m, mod_clock_hxtal */
#define HDL_SYS_CLOCK                     mod_clock_pll_p  /* Can be clocked by: mod_clock_pll_p, mod_clock_hxtal, mod_clock_irc16m */
#define HDL_PLL_VCO_PRESCALER             4                /* Can be 2, 3 .. 63 */
#define HDL_PLL_N_MULTIPLY                96               /* Note that, don`t excceed 500MHz; Can be 64, 65 .. 500 */ 
#define HDL_PLL_P_PRESCALER               2                /* Note that, don`t excceed 240MHz; Can be 2, 4, 6, 8 */
#define HDL_PLL_Q_PRESCALER               10               /* Note that, don`t excceed 48MHz; Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                /* Note that, don`t excceed 200MHz; Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                4                /* Note that, don`t excceed 60MHz; Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                2                /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */

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
  .flash_latency = WS_WSCNT_9
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

hdl_interrupt_t mod_irq_timer0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
    .priority = 0,
    .priority_group = 1
  )
};

hdl_interrupt_t mod_irq_timer1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ28_TIMER1,
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
    .irq_type = HDL_NVIC_IRQ50_TIMER4,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c_0_ev = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ31_I2C0_EV,
    .priority = 0,
    .priority_group = 2
  )
};

hdl_interrupt_t mod_irq_i2c_0_err = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
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
    .irq_type = HDL_NVIC_IRQ23_EXTI5_9,
    .priority = 1,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_spi_3 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ84_SPI3,
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
  &reset_handler,                           /* Vector 1 */
  &nmi_handler,                             /* Vector 2 IRQ -14 */
  &hard_fault_handler,                       /* Vector 3 IRQ -13 */
  &mem_manage_handler,                       /* Vector 4 IRQ -12 */
  &bus_fault_handler,                        /* Vector 5 IRQ -11 */
  &usage_fault_handler,                      /* Vector 6 IRQ -10 */
  (void *)NULL,                             /* Vector 7 IRQ -9 */
  (void *)NULL,                             /* Vector 8 IRQ -8 */
  (void *)NULL,                             /* Vector 9 IRQ -7 */
  (void *)NULL,                             /* Vector 10 IRQ -6 */
  &svc_handler,                             /* Vector 11 IRQ -5 */
  &debug_mon_handler,                        /* Vector 12 IRQ -4 */
  (void *)NULL,                             /* Vector 13 IRQ -3 */
  &pend_sv_handler,                          /* Vector 14 IRQ -2 */
  &systick_handler,                         /* Vector 15 IRQ -1 */
  /* Peripheral */
  &wwdgt_handler,                        /* Vector 16 IRQ 0 */
  &lvd_handler,                          /* Vector 17 IRQ 1 */
  &TAMPER_STAMP_IRQHandler,                 /* Vector 18 IRQ 2 */
  &RTC_WKUP_IRQHandler,                     /* Vector 19 IRQ 3 */
  &fmc_handler,                          /* Vector 20 IRQ 4 */
  &RCU_CTC_IRQHandler,                      /* Vector 21 IRQ 5 */
  &EXTI0_IRQHandler,                        /* Vector 22 IRQ 6 */
  &EXTI1_IRQHandler,                        /* Vector 23 IRQ 7 */
  &EXTI2_IRQHandler,                        /* Vector 24 IRQ 8 */
  &EXTI3_IRQHandler,                        /* Vector 25 IRQ 9 */
  &EXTI4_IRQHandler,                        /* Vector 26 IRQ 10 */
  &DMA0_Channel0_IRQHandler,                /* Vector 27 IRQ 11 */
  &DMA0_Channel1_IRQHandler,                /* Vector 28 IRQ 12 */
  &DMA0_Channel2_IRQHandler,                /* Vector 29 IRQ 13 */
  &DMA0_Channel3_IRQHandler,                /* Vector 30 IRQ 14 */
  &DMA0_Channel4_IRQHandler,                /* Vector 31 IRQ 15 */
  &DMA0_Channel5_IRQHandler,                /* Vector 32 IRQ 16 */
  &DMA0_Channel6_IRQHandler,                /* Vector 33 IRQ 17 */
  &ADC_IRQHandler,                          /* Vector 34 IRQ 18 */
  &CAN0_TX_IRQHandler,                      /* Vector 35 IRQ 19 */
  &CAN0_RX0_IRQHandler,                     /* Vector 36 IRQ 20 */
  &CAN0_RX1_IRQHandler,                     /* Vector 37 IRQ 21 */
  &CAN0_EWMC_IRQHandler,                    /* Vector 38 IRQ 22 */
  &EXTI5_9_IRQHandler,                      /* Vector 39 IRQ 23 */
  &TIMER0_BRK_TIMER8_IRQHandler,            /* Vector 40 IRQ 24 */
  &TIMER0_UP_TIMER9_IRQHandler,             /* Vector 41 IRQ 25 */
  &TIMER0_TRG_CMT_TIMER10_IRQHandler,       /* Vector 42 IRQ 26 */
  &timer0_channel_handler,               /* Vector 43 IRQ 27 */
  &TIMER1_IRQHandler,                       /* Vector 44 IRQ 28 */
  &timer2_handler,                       /* Vector 45 IRQ 29 */
  &TIMER3_IRQHandler,                       /* Vector 46 IRQ 30 */
  &i2c0_ev_handler,                      /* Vector 47 IRQ 31 */
  &i2c0_er_handler,                      /* Vector 48 IRQ 32 */
  &i2c1_ev_handler,                      /* Vector 49 IRQ 33 */
  &i2c1_er_handler,                      /* Vector 50 IRQ 34 */
  &spi0_handler,                         /* Vector 51 IRQ 35 */
  &spi1_handler,                         /* Vector 52 IRQ 36 */
  &usart0_handler,                       /* Vector 53 IRQ 37 */
  &usart1_handler,                       /* Vector 54 IRQ 38 */
  &USART2_IRQHandler,                       /* Vector 55 IRQ 39 */
  &EXTI10_15_IRQHandler,                    /* Vector 56 IRQ 40 */
  &RTC_Alarm_IRQHandler,                    /* Vector 57 IRQ 41 */
  &USBFS_WKUP_IRQHandler,                   /* Vector 58 IRQ 42 */
  &TIMER7_BRK_TIMER11_IRQHandler,           /* Vector 59 IRQ 43 */
  &TIMER7_UP_TIMER12_IRQHandler,            /* Vector 60 IRQ 44 */
  &TIMER7_TRG_CMT_TIMER13_IRQHandler,       /* Vector 61 IRQ 45 */
  &TIMER7_Channel_IRQHandler,               /* Vector 62 IRQ 46 */
  &DMA0_Channel7_IRQHandler,                /* Vector 63 IRQ 47 */
#if defined (GD32F450) || defined (GD32F470)
  &EXMC_IRQHandler,                         /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &ENET_IRQHandler,                         /* Vector 77 IRQ 61 */
  &ENET_WKUP_IRQHandler,                    /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &UART6_IRQHandler,                        /* Vector 98 IRQ 82 */
  &UART7_IRQHandler,                        /* Vector 99 IRQ 83 */
  &SPI3_IRQHandler,                         /* Vector 100 IRQ 84 */
  &SPI4_IRQHandler,                         /* Vector 101 IRQ 85 */
  &SPI5_IRQHandler,                         /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &TLI_IRQHandler,                          /* Vector 104 IRQ 88 */
  &TLI_ER_IRQHandler,                       /* Vector 105 IRQ 89 */
  &IPA_IRQHandler,                          /* Vector 106 IRQ 90 */
#endif /* GD32F450_470 */
#if defined (GD32F405) || defined (GD32F425)
  &irq_n_handler,                           /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &irq_n_handler,                           /* Vector 77 IRQ 61 */
  &irq_n_handler,                           /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &irq_n_handler,                           /* Vector 98 IRQ 82 */
  &irq_n_handler,                           /* Vector 99 IRQ 83 */
  &irq_n_handler,                           /* Vector 100 IRQ 84 */
  &irq_n_handler,                           /* Vector 101 IRQ 85 */
  &irq_n_handler,                           /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &irq_n_handler,                           /* Vector 104 IRQ 88 */
  &irq_n_handler,                           /* Vector 105 IRQ 89 */
  &irq_n_handler,                           /* Vector 106 IRQ 90 */
#endif /* GD32F405_425 */
#if defined (GD32F407) || defined (GD32F427)
  &EXMC_IRQHandler,                         /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &ENET_IRQHandler,                         /* Vector 77 IRQ 61 */
  &ENET_WKUP_IRQHandler,                    /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &irq_n_handler,                           /* Vector 98 IRQ 82 */
  &irq_n_handler,                           /* Vector 99 IRQ 83 */
  &irq_n_handler,                           /* Vector 100 IRQ 84 */
  &irq_n_handler,                           /* Vector 101 IRQ 85 */
  &irq_n_handler,                           /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &irq_n_handler,                           /* Vector 104 IRQ 88 */
  &irq_n_handler,                           /* Vector 105 IRQ 89 */
  &IPA_IRQHandler,                          /* Vector 106 IRQ 90 */  
#endif /* GD32F407_427 */
  &irq_n_handler,                           /* Vector 107 IRQ 91 */
  &irq_n_handler,                           /* Vector 108 IRQ 92 */
  &irq_n_handler,                           /* Vector 109 IRQ 93 */
  &irq_n_handler,                           /* Vector 110 IRQ 94 */
  &irq_n_handler,                           /* Vector 111 IRQ 95 */
};

const hdl_nvic_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_nmi, &mod_irq_exti_4, &mod_irq_spi_3, &mod_irq_adc,
                              &mod_irq_i2c_0_ev, &mod_irq_i2c_0_err, &mod_irq_exti_5_9, &mod_irq_timer4),
  .phy = NVIC
};

const hdl_nvic_t mod_nvic = {
  .iface = &hdl_nvic_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/***********************************************************
 *                          EXTI
***********************************************************/

hdl_exti_t mod_nvic_exti_line_4 = {
  .line = HDL_EXTI_LINE_4,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

hdl_exti_t mod_nvic_exti_line_5 = {
  .line = HDL_EXTI_LINE_5,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PD,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

hdl_exti_controller_config_t mod_exti_cnf = {
  .extis = hdl_extis(&mod_nvic_exti_line_4, &mod_nvic_exti_line_5),
  .phy = EXTI
};

hdl_exti_controller_t mod_exti = {
  .iface = &hdl_exti_controller_iface,
  .dependencies = hdl_module_dependencies(&mod_nvic),
  .config = &mod_exti_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
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
    .phy = RCU
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
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_pll_sel = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    .property.div = HDL_PLL_VCO_PRESCALER,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_vco = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_pll_sel),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_VCO,
    .property.mul = HDL_PLL_N_MULTIPLY,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_p = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_vco),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_P,
    .property.div = HDL_PLL_P_PRESCALER,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_q = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_vco),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_Q,
    .property.div = HDL_PLL_Q_PRESCALER,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_sys_clock = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core, &mod_clock_irc16m, &HDL_SYS_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_SYS_SEL,
    .phy = RCU
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
    .phy = RCU
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
    .phy = RCU
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
    .phy = RCU
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
    .phy = RCU
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
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_adc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_ADC,
    .property.div = 8,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

/***********************************************************
 *                          COUNTER
***********************************************************/
const hdl_tick_counter_timer_config_t mod_tick_counter0_cnf = {
  .alignedmode = TIMER_COUNTER_EDGE,
  .clockdivision = TIMER_CKDIV_DIV1,
  .counterdirection = TIMER_COUNTER_UP,
  .period = 16000 - 1,
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER0
};

const hdl_tick_counter_timer_config_t mod_tick_counter1_cnf = {
  .alignedmode = TIMER_COUNTER_EDGE,
  .clockdivision = TIMER_CKDIV_DIV1,
  .counterdirection = TIMER_COUNTER_UP,
  .period = 16000 - 1,
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER1
};

const hdl_tick_counter_timer_config_t mod_tick_counter4_cnf = {
  .alignedmode = TIMER_COUNTER_EDGE,
  .clockdivision = TIMER_CKDIV_DIV1,
  .counterdirection = TIMER_COUNTER_UP,
  .period = 0,
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER4
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = 240000 - 1
};

const hdl_tick_counter_t mod_systick_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .type.systick = &mod_systick_counter_cnf,
    .phy = (uint32_t)SysTick
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_tick_counter_t mod_timer0_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb2),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .type.timer = &mod_tick_counter0_cnf,
    .phy = TIMER0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_tick_counter_t mod_timer1_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .type.timer = &mod_tick_counter1_cnf,
    .phy = TIMER1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_tick_counter_t mod_timer4_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_apb1_timers),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .type.timer = &mod_tick_counter4_cnf,
    .phy = TIMER4
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};
/***********************************************************
 *                          TIMER
 ***********************************************************/

const hdl_time_counter_config_t mod_systick_timer_cnf = {
  .reload_interrupt = &mod_irq_systick,
};

const hdl_time_counter_config_t mod_timer0_cnf = {
  .reload_interrupt = &mod_irq_timer0,
};

const hdl_time_counter_config_t mod_timer1_cnf = {
  .reload_interrupt = &mod_irq_timer1,
};

const hdl_time_counter_config_t mod_timer4_cnf = {
  .reload_interrupt = &mod_irq_timer4,
};

const hdl_time_counter_t mod_systick_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_systick_counter, &mod_nvic),
  .config = &mod_systick_timer_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE)
};

const hdl_time_counter_t mod_timer0 = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_timer0_counter, &mod_nvic),
  .config = &mod_timer0_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE)
};

const hdl_time_counter_t mod_timer1 = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_timer1_counter, &mod_nvic),
  .config = &mod_timer1_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE)
};

const hdl_timer_t mod_watchdog_timer = {
  .iface = &hdl_timer_iface,
  .config = hdl_module_config(hdl_timer_config_t,
    .mode = HDL_TIMER_MODE_IDLE,
    .delay = 0
  ),
  .dependencies = hdl_module_dependencies(&mod_systick_timer),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIMER_VAR_SIZE)
};


/***********************************************************
 *                          GPIO PORT
***********************************************************/
const hdl_gpio_port_config_t mod_gpio_port_config_a = {
  .rcu = RCU_GPIOA,
  .phy = GPIOA
};

const hdl_gpio_port_config_t mod_gpio_port_config_b = {
  .rcu = RCU_GPIOB,
  .phy = GPIOB
};

const hdl_gpio_port_config_t mod_gpio_port_config_c = {
  .rcu = RCU_GPIOC,
  .phy = GPIOC
};

const hdl_gpio_port_config_t mod_gpio_port_config_d = {
  .rcu = RCU_GPIOD,
  .phy = GPIOD
};

const hdl_gpio_port_config_t mod_gpio_port_config_e = {
  .rcu = RCU_GPIOE,
  .phy = GPIOE
};

const hdl_gpio_port_config_t mod_gpio_port_config_f = {
  .rcu = RCU_GPIOF,
  .phy = GPIOF
};

const hdl_gpio_port_config_t mod_gpio_port_config_g = {
  .rcu = RCU_GPIOG,
  .phy = GPIOG
};

const hdl_gpio_port_config_t mod_gpio_port_config_h = {
  .rcu = RCU_GPIOH,
  .phy = GPIOH
};

const hdl_gpio_port_config_t mod_gpio_port_config_i = {
  .rcu = RCU_GPIOI,
  .phy = GPIOI
};

const hdl_gpio_port_mcu_t hdl_gpio_port_a = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_a,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_b = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_b,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_c = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_c,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_d = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_d,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_e = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_e,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_f = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_f,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_gpio_port_mcu_t hdl_gpio_port_g = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_gpio_port_config_g,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};
/***********************************************************
 *                          GPIO
***********************************************************/
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_input_floating = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
  .af = GPIO_AF_0
};

const hdl_gpio_pin_hw_config_t hdl_gpio_mode_input_pu = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_PULLUP,
  .ospeed = GPIO_OSPEED_2MHZ,
  .af = GPIO_AF_0
};

const hdl_gpio_pin_hw_config_t hdl_gpio_mode_output_pp = {
  .type = GPIO_MODE_OUTPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
  .otype = GPIO_OTYPE_PP,
  .af = GPIO_AF_0
};

const hdl_gpio_pin_hw_config_t hdl_gpio_mode_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
  .af = GPIO_AF_0
};

const hdl_gpio_pin_hw_config_t hdl_gpio_spi_mode = {
  .type = GPIO_MODE_AF,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_MAX,
  .af = GPIO_AF_5,
  .otype = GPIO_OTYPE_PP,
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c_0_mode = {
  .af = GPIO_AF_4,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

const hdl_gpio_pin_t mod_gpio_pa0 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_0, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA0)

const hdl_gpio_pin_t mod_gpio_pa1 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_1, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA1)

const hdl_gpio_pin_t mod_gpio_pa2 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_2, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA2)

const hdl_gpio_pin_t mod_gpio_pa3 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_3, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA3)

const hdl_gpio_pin_t mod_gpio_pa4 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA4)

const hdl_gpio_pin_t mod_gpio_pa5 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_5, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_analog
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PA5)







const hdl_gpio_pin_t mod_gpio_pb0 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_0, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PB0)

const hdl_gpio_pin_t mod_gpio_pb6_scl = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_6, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &mod_gpio_i2c_0_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PB6)

const hdl_gpio_pin_t mod_gpio_pb7_sda = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_7, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &mod_gpio_i2c_0_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PB7)

const hdl_gpio_pin_t mod_gpio_pc0 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_0, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC0)

const hdl_gpio_pin_t mod_gpio_pc1 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_1, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC1)

const hdl_gpio_pin_t mod_gpio_pc2 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_2, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC2)

const hdl_gpio_pin_t mod_gpio_pc3 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_3, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC3)

const hdl_gpio_pin_t mod_gpio_pc4 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC4)

const hdl_gpio_pin_t mod_gpio_pc5 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_5, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC5)

const hdl_gpio_pin_t mod_gpio_pc6 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_6, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC6)

const hdl_gpio_pin_t mod_gpio_pc7 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_7, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC7)

const hdl_gpio_pin_t mod_gpio_pc8 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_8, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC8)

const hdl_gpio_pin_t mod_gpio_pc9 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_9, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC9)

const hdl_gpio_pin_t mod_gpio_pc10 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_10, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC10)

const hdl_gpio_pin_t mod_gpio_pc11 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_11, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PC11)

const hdl_gpio_pin_t mod_gpio_pd4 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD4)

const hdl_gpio_pin_t mod_gpio_pd5 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_5, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD5)

const hdl_gpio_pin_t mod_gpio_pd8 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_8, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD8)

const hdl_gpio_pin_t mod_gpio_pd9 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_9, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD9)

const hdl_gpio_pin_t mod_gpio_pd10 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_10, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD10)

const hdl_gpio_pin_t mod_gpio_pd11 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_11, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD11)

const hdl_gpio_pin_t mod_gpio_pd12 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_12, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PD12)

const hdl_gpio_pin_t mod_gpio_pe2_spi_sck = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_2, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_spi_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PE2)

const hdl_gpio_pin_t mod_gpio_pe5_spi_miso = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_5, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_spi_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PE5)

const hdl_gpio_pin_t mod_gpio_pe6_spi_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_6, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_spi_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PE6)

const hdl_gpio_pin_t mod_gpio_pf3 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_3, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF3)

const hdl_gpio_pin_t mod_gpio_pf4 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF4)

const hdl_gpio_pin_t mod_gpio_pf6 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_6, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF6)

const hdl_gpio_pin_t mod_gpio_pf8 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_8, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF8)

const hdl_gpio_pin_t mod_gpio_pf10 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_10, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF10)

const hdl_gpio_pin_t mod_gpio_pf11 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_11, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF11)

const hdl_gpio_pin_t mod_gpio_pf12 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_12, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF12)

const hdl_gpio_pin_t mod_gpio_pf13 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_13, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF13)

const hdl_gpio_pin_t mod_gpio_pf14 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_14, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF14)

const hdl_gpio_pin_t mod_gpio_pf15 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_15, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PF15)

const hdl_gpio_pin_t mod_gpio_pg0 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_0, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG0)

const hdl_gpio_pin_t mod_gpio_pg1 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_1, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG1)

const hdl_gpio_pin_t mod_gpio_pg2 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_2, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG2)

const hdl_gpio_pin_t mod_gpio_pg3 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_3, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_output_pp
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG3)

const hdl_gpio_pin_t mod_gpio_pg4 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG4)

const hdl_gpio_pin_t mod_gpio_pg5 = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_5, 
    .inactive_default = HDL_GPIO_LOW, 
    .hwc = &hdl_gpio_mode_input_floating
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PG5)


/***********************************************************
 *                          DMA
***********************************************************/
const hdl_dma_config_t mod_dma_config = {
  .rcu = RCU_DMA1,
  .phy = DMA1
};

hdl_dma_t mod_dma = {
  .iface = &hdl_dma_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_dma_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_config_t mod_adc_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .channel_periphery = DMA_SUBPERI0,
  .direction = DMA_PERIPH_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_16BIT,
  .periph_width = DMA_PERIPH_WIDTH_16BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 1,
  .ch_no = DMA_CH0
};

const hdl_dma_channel_config_t mod_m2m_dma_ch_config = {
  .priority = DMA_PRIORITY_ULTRA_HIGH,
  .channel_periphery = DMA_SUBPERI0,
  .direction = DMA_MEMORY_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 1,
  .circular = 0,
  .ch_no = DMA_CH1
};

hdl_dma_channel_t mod_adc_dma_ch = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma),
  .config = &mod_adc_dma_ch_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_config_t mod_dma_ch_spi_3_rx_config = {
  .priority = DMA_PRIORITY_MEDIUM,
  .channel_periphery = DMA_SUBPERI5,
  .direction = DMA_PERIPH_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 0,
  .ch_no = DMA_CH3
};

hdl_dma_channel_t mod_dma_ch_spi_3_rx = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma),
  .config = &mod_dma_ch_spi_3_rx_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_config_t mod_dma_ch_spi_3_tx_config = {
  .priority = DMA_PRIORITY_MEDIUM,
  .channel_periphery = DMA_SUBPERI5,
  .direction = DMA_MEMORY_TO_PERIPH,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 0,
  .ch_no = DMA_CH4
};

hdl_dma_channel_t mod_dma_ch_spi_3_tx = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma),
  .config = &mod_dma_ch_spi_3_tx_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

hdl_dma_channel_t mod_dma_ch_spi_3_m2m = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma),
  .config = &mod_m2m_dma_ch_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

/**************************************************************
 *                        ADC
 *************************************************************/
const hdl_adc_source_t mod_adc_source_0_adc_24v = {
  .channel = HDL_ADC_CHANNEL_0,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_source_t mod_adc_source_1_adc_24v_poe = {
  .channel = HDL_ADC_CHANNEL_1,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_source_t mod_adc_source_2_adc_5v = {
  .channel = HDL_ADC_CHANNEL_2,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_source_t mod_adc_source_3_adc_3v3 = {
  .channel = HDL_ADC_CHANNEL_3,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_source_t mod_adc_source_4_adc_2v5 = {
  .channel = HDL_ADC_CHANNEL_4,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_source_t mod_adc_source_5_adc_1v8 = {
  .channel = HDL_ADC_CHANNEL_5,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_config_t mod_adc_cnf = {
  .phy = ADC0,
  .adc_interrupt = &mod_irq_adc,
  .resolution = HDL_ADC_RESOLUTION_12BIT,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_0_adc_24v, &mod_adc_source_1_adc_24v_poe, &mod_adc_source_2_adc_5v,
                          &mod_adc_source_3_adc_3v3, &mod_adc_source_4_adc_2v5, &mod_adc_source_5_adc_1v8),
};

hdl_adc_mcu_t mod_adc = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_adc, &mod_systick_timer, &mod_adc_dma_ch, &mod_nvic),
  .config = &mod_adc_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

/**************************************************************
 *                        SPI
 *************************************************************/
const hdl_gpio_pin_t mod_gpio_pe6_spi_cs = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
  .config = hdl_module_config(hdl_gpio_pin_config_t, 
    .pin = GPIO_PIN_4, 
    .inactive_default = HDL_GPIO_HIGH, 
    .hwc = &hdl_gpio_spi_mode
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
}; // (PE4)


const hdl_spi_server_config_t hdl_spi_slave_config = {
  .rcu = RCU_SPI3,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE,
  .spi_interrupt = &mod_irq_spi_3,
  .nss_interrupt = &mod_irq_exti_4,
  .phy = SPI3
};

const hdl_spi_server_dma_mcu_t mod_spi3_server_dma = {
  .iface = &hdl_spi_server_dma_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pe6_spi_mosi, &mod_gpio_pe5_spi_miso, &mod_gpio_pe2_spi_sck,
                                          &mod_gpio_pe6_spi_cs, &mod_clock_apb2, &mod_nvic, 
                                          &mod_dma_ch_spi_3_rx, &mod_dma_ch_spi_3_tx, &mod_exti),
  .config = &hdl_spi_slave_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_SERVER_DMA_VAR_SIZE)
};

hdl_i2c_channel_t i2c1_ch0 = {.address = 0x23};

const hdl_i2c_config_t mod_i2c_0_config = {
  .channels = hdl_i2c_channels(&i2c1_ch0),
  .hwc = hdl_module_config(hdl_i2c_config_hw_t,
    .addr_10_bits = 0,
    .dtcy = I2C_DTCY_2,
    .dual_address = 0,
    .err_interrupt = &mod_irq_i2c_0_err,
    .ev_interrupt = &mod_irq_i2c_0_ev,
    .general_call_enable = 0,
    .speed = 400000,
    .stretch_enable = 1,
    .rcu = RCU_I2C0,
    .phy = I2C0
  )
};

hdl_i2c_mcu_t mod_i2c0 = {
  .iface = &hdl_i2c_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pb6_scl, &mod_gpio_pb7_sda,
                                                 &mod_clock_apb1, &mod_nvic, &mod_systick_timer),
  .config = &mod_i2c_0_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_I2C_VAR_SIZE)
};

extern const hdl_interrupt_controller_t mod_ic                    __attribute__ ((alias ("mod_nvic")));

extern const hdl_tick_counter_t mod_tick_counter                  __attribute__ ((alias ("mod_systick_counter")));

extern const hdl_time_counter_t mod_timer_ms                      __attribute__ ((alias ("mod_systick_timer")));


extern const hdl_gpio_pin_t mod_output1_pin                       __attribute__ ((alias ("mod_gpio_pc0")));
extern const hdl_gpio_pin_t mod_output2_pin                       __attribute__ ((alias ("mod_gpio_pc1")));

extern const hdl_gpio_pin_t mod_led1_pin                          __attribute__ ((alias ("mod_gpio_pc5")));
extern const hdl_gpio_pin_t mod_led2_pin                          __attribute__ ((alias ("mod_gpio_pc6")));
extern const hdl_gpio_pin_t mod_led3_pin                          __attribute__ ((alias ("mod_gpio_pc7")));
extern const hdl_gpio_pin_t mod_led4_pin                          __attribute__ ((alias ("mod_gpio_pc8")));
extern const hdl_gpio_pin_t mod_button_pin                        __attribute__ ((alias ("mod_gpio_pf12")));

extern const hdl_uart_t mod_uart                                  __attribute__ ((alias ("hdl_null_module")));

extern const hdl_spi_client_ch_t mod_spi_client                   __attribute__ ((alias ("hdl_null_module")));

#endif