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
#define HDL_APB2_PRESCALER                4                /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */

//#define BAUD_RATE    115200
#define BAUD_RATE       9600   //по умолчанию. Должно быть можно изменить через команды по spi
#define TMR0_FREQ       120000000     //Clock freq is 120 MHz not 60?
#define UART_DATA_BITS  10  //1 start bit, 8 data bits, 0 parity bits, 1 stop bit
#define DELAY_CNT(freq) ((TMR0_FREQ / freq) * UART_DATA_BITS)   //mks

const uint32_t RS485_TIMER_RELOAD_EVENT = TIMER_INT_FLAG_UP;

const hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_9 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
};

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .config = &mod_sys_core_cnf
};
/***********************************************************
 *                          IRQ
***********************************************************/
hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};
hdl_interrupt_t mod_irq_timer0 = {
  .irq_type = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
  .priority = 0,
  .priority_group = 1,
};
hdl_interrupt_t mod_irq_timer1 = {
  .irq_type = HDL_NVIC_IRQ28_TIMER1,
  .priority = 0,
  .priority_group = 2,
};
hdl_interrupt_t mod_irq_timer4 = {
  .irq_type = HDL_NVIC_IRQ50_TIMER4,
  .priority = 0,
  .priority_group = 2,
};

hdl_interrupt_t mod_irq_i2c_0_ev = {
  .irq_type = HDL_NVIC_IRQ31_I2C0_EV,
  .priority = 0,
  .priority_group = 1,
};
hdl_interrupt_t mod_irq_i2c_0_err = {
  .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
  .priority = 0,
  .priority_group = 2,
};

hdl_interrupt_t mod_irq_exti_4 = {
  .irq_type = HDL_NVIC_IRQ10_EXTI4,
  .priority = 0,
  .priority_group = 1,
};
hdl_interrupt_t mod_irq_exti_5_9 = {
  .irq_type = HDL_NVIC_IRQ23_EXTI5_9,
  .priority = 0,
  .priority_group = 1,
};
hdl_interrupt_t mod_irq_spi_3 = {
  .irq_type = HDL_NVIC_IRQ84_SPI3,
  .priority = 0,
  .priority_group = 0,
};
hdl_interrupt_t mod_irq_adc = {
  .irq_type = HDL_NVIC_IRQ18_ADC,
  .priority = 0,
  .priority_group = 0,
};

/***********************************************************
 *                          NVIC
***********************************************************/
extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

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

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0, &mod_irq_timer1, &mod_irq_exti_4, &mod_irq_spi_3, &mod_irq_adc,
                              &mod_irq_i2c_0_ev, &mod_irq_i2c_0_err, &mod_irq_exti_5_9, &mod_irq_timer4),
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf
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

hdl_exti_controller_t mod_exti = {
  .module.init = &hdl_exti,
  .module.reg = (void *)EXTI,
  .module.dependencies = hdl_module_dependencies(&mod_nvic.module),
  .extis = hdl_extis(&mod_nvic_exti_line_4, &mod_nvic_exti_line_5)
};

/***********************************************************
 *                          CLOCK
***********************************************************/
hdl_clock_t mod_clock_irc16m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC16M, 
    .property.freq = 16000000
  }})
};

hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK
  }})
};

hdl_clock_t mod_pll_sel = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    .property.div = HDL_PLL_VCO_PRESCALER
  }})
};

hdl_clock_t mod_clock_pll_vco = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_pll_sel.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_VCO,
    .property.mul = HDL_PLL_N_MULTIPLY
  }})
};

hdl_clock_t mod_clock_pll_p = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_P,
    .property.div = HDL_PLL_P_PRESCALER
  }})
};

hdl_clock_t mod_clock_pll_q = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_Q,
    .property.div = HDL_PLL_Q_PRESCALER
  }})
};

hdl_clock_t mod_clock_sys_clock = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_clock_irc16m.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYS_SEL,
  }})
};

hdl_clock_t mod_clock_ahb = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys_clock.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PRESCALER
  }})
};

hdl_clock_t mod_clock_apb1 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PRESCALER
  }})
};

hdl_clock_t mod_clock_apb2 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PRESCALER
  }})
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
  .period = DELAY_CNT(BAUD_RATE),
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER1
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = 240000 - 1
};

hdl_tick_counter_t mod_systick_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)SysTick,
  .config.systick = &mod_systick_counter_cnf
};

hdl_tick_counter_t mod_timer0_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module),
  .module.reg = (void *)TIMER0,
  .config = &mod_tick_counter0_cnf
};

hdl_tick_counter_t mod_timer1_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module),
  .module.reg = (void *)TIMER1,
  .config = &mod_tick_counter1_cnf
};

hdl_tick_counter_t mod_timer4_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module),
  .module.reg = (void *)TIMER4,
  .config = &mod_tick_counter4_cnf
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

hdl_time_counter_t mod_systick_timer_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_systick_timer_cnf
};

hdl_time_counter_t mod_timer0_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_timer0_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_timer0_cnf
};

hdl_time_counter_t mod_timer1_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_timer1_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_timer1_cnf
};

hdl_timer_t mod_watchdog_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_systick_timer_ms.module),
  .module.init = &hdl_timer,
};


/***********************************************************
 *                          GPIO PORT
***********************************************************/
hdl_gpio_port_config_t mod_gpio_port_config_a = {
  .rcu = RCU_GPIOA
};

hdl_gpio_port_config_t mod_gpio_port_config_b = {
  .rcu = RCU_GPIOB
};

hdl_gpio_port_config_t mod_gpio_port_config_c = {
  .rcu = RCU_GPIOC
};

hdl_gpio_port_config_t mod_gpio_port_config_d = {
  .rcu = RCU_GPIOD
};

hdl_gpio_port_config_t mod_gpio_port_config_e = {
  .rcu = RCU_GPIOE
};

hdl_gpio_port_config_t mod_gpio_port_config_f = {
  .rcu = RCU_GPIOF
};

hdl_gpio_port_config_t mod_gpio_port_config_g = {
  .rcu = RCU_GPIOG
};

hdl_gpio_port_config_t mod_gpio_port_config_h = {
  .rcu = RCU_GPIOH
};

hdl_gpio_port_config_t mod_gpio_port_config_i = {
  .rcu = RCU_GPIOI
};

hdl_gpio_port_t hdl_gpio_port_a = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOA,
  .config = &mod_gpio_port_config_a
};
hdl_gpio_port_t hdl_gpio_port_b = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOB,
  .config = &mod_gpio_port_config_b
};
hdl_gpio_port_t hdl_gpio_port_c = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOC,
  .config = &mod_gpio_port_config_c
};
hdl_gpio_port_t hdl_gpio_port_d = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOD,
  .config = &mod_gpio_port_config_d
};
hdl_gpio_port_t hdl_gpio_port_e = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOE,
  .config = &mod_gpio_port_config_e
};
hdl_gpio_port_t hdl_gpio_port_f = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOF,
  .config = &mod_gpio_port_config_f
};
hdl_gpio_port_t hdl_gpio_port_g = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOG,
  .config = &mod_gpio_port_config_g
};
/***********************************************************
 *                          GPIO
***********************************************************/
/**************************************
 *            GPIO MODE
***************************************/
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_input_floating = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ
};
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_output_no_pull = {
  .type = GPIO_MODE_OUTPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
};
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
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
/***********************************************************
 *                          GPIO PIN
***********************************************************/
hdl_gpio_pin_t mod_gpio_i2c0_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_i2c_0_mode)
};

hdl_gpio_pin_t mod_gpio_i2c0_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_i2c_0_mode)
};
/***********************************************************
 *                          POWER ADJUST
***********************************************************/
hdl_gpio_pin_t mod_do_5v_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_24v_poe_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_di_24v_poe_power_fault = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_24v_poe_power_good = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
/***********************************************************
 *                          RELAY
***********************************************************/
hdl_gpio_pin_t mod_do_relay1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_relay2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                        Other
***********************************************************/
hdl_gpio_pin_t mod_do_rs485_dir = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_di_rs485_tx = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_do_pci_switch = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_lte_reset = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
hdl_gpio_pin_t mod_do_smarc_reset_in = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_di_smarc_reset_out = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_do_smarc_boot_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_boot_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_boot_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_power_btn = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};

/***********************************************************
 *                        X1 Connector
***********************************************************/
hdl_gpio_pin_t mod_di_module_address_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_external_periph_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_external_periph_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_do_external_output_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_external_output_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                          LED
***********************************************************/
/*
      * | * | *  
LEFT  * | * | *  
      * | * | *  
         BOT

      LED_0_2 | LED_1_2 | LED_2_2
LEFT  LED_0_1 | LED_1_1 | LED_2_1
      LED_0_0 | LED_1_0 | LED_2_0
                  BOT
*/
hdl_gpio_pin_t mod_do_led_0_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
    .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_0_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_0_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                    ANALOG PIN
***********************************************************/
hdl_gpio_pin_t mod_adc_pin_24v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_24v_poe = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_5v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_3v3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_2v5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_1v8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
/***********************************************************
 *                          DMA
***********************************************************/
const hdl_dma_config_t mod_dma_config = {
  .rcu = RCU_DMA1
};

hdl_dma_t mod_dma = {
  .module.init = &hdl_dma,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void*)DMA1,
  .config = &mod_dma_config
};

const hdl_dma_channel_config_t mod_adc_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .channel_periphery = DMA_SUBPERI0,
  .direction = DMA_PERIPH_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_16BIT,
  .periph_width = DMA_PERIPH_WIDTH_16BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 1
};

const hdl_dma_channel_config_t mod_m2m_dma_ch_config = {
  .priority = DMA_PRIORITY_ULTRA_HIGH,
  .channel_periphery = DMA_SUBPERI0,
  .direction = DMA_MEMORY_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 1,
  .circular = 0
};

hdl_dma_channel_t mod_adc_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH0,
  .config = &mod_adc_dma_ch_config
};

const hdl_dma_channel_config_t mod_dma_ch_spi_3_rx_config = {
  .priority = DMA_PRIORITY_MEDIUM,
  .channel_periphery = DMA_SUBPERI5,
  .direction = DMA_PERIPH_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 0
};

hdl_dma_channel_t mod_dma_ch_spi_3_rx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH3,
  .config = &mod_dma_ch_spi_3_rx_config
};

const hdl_dma_channel_config_t mod_dma_ch_spi_3_tx_config = {
  .priority = DMA_PRIORITY_MEDIUM,
  .channel_periphery = DMA_SUBPERI5,
  .direction = DMA_MEMORY_TO_PERIPH,
  .memory_width = DMA_MEMORY_WIDTH_8BIT,
  .periph_width = DMA_PERIPH_WIDTH_8BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 0
};

hdl_dma_channel_t mod_dma_ch_spi_3_tx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH4,
  .config = &mod_dma_ch_spi_3_tx_config
};

  hdl_dma_channel_t mod_dma_ch_spi_3_m2m = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .config = &mod_m2m_dma_ch_config
};
/**************************************************************
 *                        ADC
 *************************************************************/
hdl_adc_source_t mod_adc_source_0_adc_24v = {
  .channel = HDL_ADC_CHANNEL_0,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_1_adc_24v_poe = {
  .channel = HDL_ADC_CHANNEL_1,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_2_adc_5v = {
  .channel = HDL_ADC_CHANNEL_2,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_3_adc_3v3 = {
  .channel = HDL_ADC_CHANNEL_3,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_4_adc_2v5 = {
  .channel = HDL_ADC_CHANNEL_4,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_5_adc_1v8 = {
  .channel = HDL_ADC_CHANNEL_5,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};

const hdl_adc_config_t mod_adc_cnf = {
  .adc_interrupt = &mod_irq_adc,
  .resolution = HDL_ADC_RESOLUTION_12BIT,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_0_adc_24v, &mod_adc_source_1_adc_24v_poe, &mod_adc_source_2_adc_5v,
                          &mod_adc_source_3_adc_3v3, &mod_adc_source_4_adc_2v5, &mod_adc_source_5_adc_1v8),
};

hdl_adc_t mod_adc = {
  .module.init = &hdl_adc,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module, &mod_systick_timer_ms.module, &mod_adc_dma_ch.module, &mod_nvic.module),
  .module.reg = (void *)ADC0,
  .config = &mod_adc_cnf
};
/**************************************************************
 *                        SPI
 *************************************************************/
  hdl_gpio_pin_t mod_spi_3_mosi = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_6,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_miso = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_sck = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_2,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_cs = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_spi_mode)
};


const hdl_spi_server_config_t hdl_spi_slave_config = {
  .rcu = RCU_SPI3,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE,
  .spi_interrupt = &mod_irq_spi_3,
  .nss_interrupt = &mod_irq_exti_4,
};

hdl_spi_server_dma_t mod_spi3_server_dma = {
  .module.reg = (void *)SPI3,
  .module.dependencies = hdl_module_dependencies(&mod_spi_3_mosi.module, &mod_spi_3_miso.module, &mod_spi_3_sck.module,
                                                  &mod_spi_3_cs.module, &mod_clock_apb2.module, &mod_nvic.module, 
                                                  &mod_dma_ch_spi_3_rx.module, &mod_dma_ch_spi_3_tx.module, &mod_exti.module),
  .module.init = &hdl_spi_server_dma,
  .config = &hdl_spi_slave_config,
};

const hdl_i2c_config_t mod_i2c_0_config = {
  .addr0 = 0x23,
  .addr1 = 0,
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = &mod_irq_i2c_0_err,
  .ev_interrupt = &mod_irq_i2c_0_ev,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C0
};

hdl_i2c_t mod_i2c0 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_i2c0_scl.module, &mod_gpio_i2c0_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_systick_timer_ms.module),
  .module.reg = (void *)I2C0,
  .config = &mod_i2c_0_config,
};

/**************************************************************
 *                        BLDL
 *************************************************************/

const atb3500_power_rail_config_t rail_24v_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_24V,
  .adc_ch = 0,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
  .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_24v = {
  .module.init = &atb3500_power_rail,
  .module.dependencies = hdl_module_dependencies(
    &mod_systick_timer_ms.module,
    &mod_adc.module,
    &mod_adc_pin_24v.module,
    &hdl_null_module,
    &hdl_null_module,
    &hdl_null_module,
    &hdl_null_module),
  .config = &rail_24v_cnf
};

const atb3500_power_rail_config_t rail_24vpoe_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_24V,
  .adc_ch = 1,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
  .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_24vpoe = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_24v_poe.module,
        &rail_24v.module,
        &mod_do_24v_poe_power_on.module, 
        &mod_di_24v_poe_power_fault.module, 
        &mod_di_24v_poe_power_good.module),
    .config = &rail_24vpoe_cnf
};

const atb3500_power_rail_config_t rail_5v_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_5V,
  .adc_ch = 2,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_5V,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_5V,
  .raise_delay = POWER_RAIL_RAISE_DELAY_5V,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_5V,
};

atb3500_power_rail_t rail_5v = {
  .module.init = &atb3500_power_rail,
  .module.dependencies = hdl_module_dependencies(
    &mod_systick_timer_ms.module,
    &mod_adc.module,
    &mod_adc_pin_5v.module,
    &rail_24v.module,
    &mod_do_5v_power_on.module,
    &hdl_null_module,
    &hdl_null_module),
  .config = &rail_5v_cnf
};

const atb3500_power_rail_config_t rail_3v3_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_3V3,
  .adc_ch = 3,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_3V3,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_3V3,
  .raise_delay = POWER_RAIL_RAISE_DELAY_3V3,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_3V3,
};

atb3500_power_rail_t rail_3v3 = {
  .module.init = &atb3500_power_rail,
  .module.dependencies = hdl_module_dependencies(
    &mod_systick_timer_ms.module,
    &mod_adc.module,
    &mod_adc_pin_3v3.module,
    &rail_5v.module,
    &hdl_null_module,
    &hdl_null_module,
    &hdl_null_module),
  .config = &rail_3v3_cnf
};

const atb3500_power_rail_config_t rail_2v5_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_2V5,
  .adc_ch = 4,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_2V5,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_2V5,
  .raise_delay = POWER_RAIL_RAISE_DELAY_2V5,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_2V5,
};

atb3500_power_rail_t rail_2v5 = {
  .module.init = &atb3500_power_rail,
  .module.dependencies = hdl_module_dependencies(
    &mod_systick_timer_ms.module,
    &mod_adc.module,
    &mod_adc_pin_2v5.module,
    &rail_5v.module,
    &hdl_null_module,
    &hdl_null_module,
    &hdl_null_module),
  .config = &rail_2v5_cnf
};

const atb3500_power_rail_config_t rail_1v8_cnf = {
  .adc_scale = POWER_RAIL_ADC_SCALE_1V8,
  .adc_ch = 5,
  .uv_threshold = POWER_RAIL_UV_TRHESHOLD_1V8,
  .ov_threshold = POWER_RAIL_OV_TRHESHOLD_1V8,
  .raise_delay = POWER_RAIL_RAISE_DELAY_1V8,
  .stabilization_delay = POWER_RAIL_STAB_DELAY_1V8,
};

atb3500_power_rail_t rail_1v8 = {
  .module.init = &atb3500_power_rail,
  .module.dependencies = hdl_module_dependencies(
    &mod_systick_timer_ms.module,
    &mod_adc.module,
    &mod_adc_pin_1v8.module,
    &rail_5v.module,
    &hdl_null_module,
    &hdl_null_module,
    &hdl_null_module),
  .config = &rail_1v8_cnf
};

hdl_smarc_carrier_t mod_smarc = {
    .module.init = &hdl_smarc_carrier,
    .module.dependencies = hdl_module_dependencies(
        &hdl_null_module /* power good */,
        &hdl_null_module /* carrier_power_on */,
        &hdl_null_module /* carrier_stand_by */,
        &mod_do_smarc_reset_in.module, 
        &mod_di_smarc_reset_out.module,
        &mod_do_smarc_boot_0.module, 
        &mod_do_smarc_boot_1.module, 
        &mod_do_smarc_boot_2.module,
        &mod_systick_timer_ms.module),
};

const hdl_button_config_t power_button_cnf = {
  .debounce_delay = 50,
  .hold_delay = 0,
};

hdl_button_t power_button = {
    .module.init = &hdl_button,
    .module.dependencies = hdl_module_dependencies(&mod_do_smarc_power_btn.module, &mod_systick_timer_ms.module ),
    .config = &power_button_cnf
};

atb3500_io_t mod_carrier_io = {
    .module.init = atb3500_io,
    .module.dependencies = hdl_module_dependencies(
    /***********************************************************
    *                      LED
    ***********************************************************/
        &mod_do_led_0_0.module, &mod_do_led_0_1.module, &mod_do_led_0_2.module,
        &mod_do_led_1_0.module, &mod_do_led_1_1.module, &mod_do_led_1_2.module, 
        &mod_do_led_2_0.module, &mod_do_led_2_1.module, &mod_do_led_2_2.module, 
    /***********************************************************
    *                          RELAY
    ***********************************************************/
        &mod_do_relay1.module, &mod_do_relay2.module,
        
    /***********************************************************
    *                        Other
    ***********************************************************/
        &mod_do_pci_switch.module, &mod_do_lte_reset.module,
    /***********************************************************
    *                        X1 Connector
    ***********************************************************/
        &mod_di_module_address_1.module, &mod_di_module_address_2.module, &mod_di_module_address_3.module,
        &mod_di_module_address_4.module, &mod_di_module_address_5.module, &mod_di_external_periph_irq_1.module,
        &mod_di_external_periph_irq_2.module, &mod_do_external_output_1.module, &mod_do_external_output_2.module
    )
    
};

atb3500_watchdog_t mod_watchdog = {
    .module.init = &atb3500_watchdog,
    .module.dependencies = hdl_module_dependencies(&mod_watchdog_timer.module)
};

atb3500_flash_serial_t mod_serial = {
  .module.init = atb3500_flash_serial,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module)

};

atb3500_i2c_gateway_t mod_i2c_gateway = {
  .module.init = &atb3500_i2c_gateway,
  .module.dependencies = hdl_module_dependencies(&mod_i2c0.module)
};

hdl_module_t power_domain = {
  .dependencies = hdl_module_dependencies( 
    &rail_24v.module,
    &rail_24vpoe.module,
    &rail_5v.module,
    &rail_3v3.module,
    &rail_2v5.module,
    &rail_1v8.module)
};

hdl_module_t rs485_ctrl = {
  .dependencies = hdl_module_dependencies(
    &mod_do_rs485_dir.module,
    &mod_di_rs485_tx.module,
    &mod_timer4_counter.module)
};

hdl_module_t app_module = {
  .dependencies = hdl_module_dependencies(
    &power_domain,
    &mod_smarc.module,
    &mod_carrier_io.module,
    &mod_watchdog.module,
    &mod_serial.module,
    &mod_spi3_server_dma.module,
    &mod_i2c_gateway.module,
    &rs485_ctrl
  )
};

extern hdl_interrupt_t rs485_tx_int              __attribute__ ((alias ("mod_irq_exti_5_9")));
extern hdl_interrupt_t rs485_timer_int           __attribute__ ((alias ("mod_irq_timer4")));
extern hdl_tick_counter_t mod_rs485_tick_counter __attribute__ ((alias ("mod_timer4_counter")));
extern hdl_interrupt_controller_t mod_int_ctrlr  __attribute__ ((alias ("mod_nvic")));

#endif