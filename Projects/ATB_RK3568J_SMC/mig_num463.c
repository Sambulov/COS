#include "hdl.h"

#if defined ( ATB_RK3568J_SMC_R3 )

void * __Vectors[0x90] __attribute__ ((section (".vectors"), used)) = {
	&_estack,            /* Top of Stack */
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
	/* External interrupts */
	&BOD_IRQHandler,        /*  0: BOD                        */
	&IRC_IRQHandler,        /*  1: IRC                        */
	&PWRWU_IRQHandler,      /*  2: PWRWU                      */
	&RAMPE_IRQHandler,      /*  3: RAMPE                      */
	&CKFAIL_IRQHandler,     /*  4: CKFAIL                     */
	&ISP_IRQHandler,        /*  5: ISP                   */
	&RTC_IRQHandler,        /*  6: RTC                        */
	&TAMPER_IRQHandler,     /*  7: TAMPER                     */
	&WDT_IRQHandler,        /*  8: WDT                        */
	&WWDT_IRQHandler,       /*  9: WWDT                       */
	&EINT0_IRQHandler,      /* 10: EINT0                      */
	&EINT1_IRQHandler,      /* 11: EINT1                      */
	&EINT2_IRQHandler,      /* 12: EINT2                      */
	&EINT3_IRQHandler,      /* 13: EINT3                      */
	&EINT4_IRQHandler,      /* 14: EINT4                      */
	&EINT5_IRQHandler,      /* 15: EINT5                      */
	&GPA_IRQHandler,        /* 16: GPA                        */
	&GPB_IRQHandler,        /* 17: GPB                        */
	&GPC_IRQHandler,        /* 18: GPC                        */
	&GPD_IRQHandler,        /* 19: GPD                        */
	&GPE_IRQHandler,        /* 20: GPE                        */
	&GPF_IRQHandler,        /* 21: GPF                        */
	&QSPI0_IRQHandler,      /* 22: QSPI0                      */
	&SPI0_IRQHandler,       /* 23: SPI0                       */
	&BRAKE0_IRQHandler,     /* 24: BRAKE0                     */
	&EPWM0P0_IRQHandler,    /* 25: EPWM0P0                    */
	&EPWM0P1_IRQHandler,    /* 26: EPWM0P1                    */
	&EPWM0P2_IRQHandler,    /* 27: EPWM0P2                    */
	&BRAKE1_IRQHandler,     /* 28: BRAKE1                     */
	&EPWM1P0_IRQHandler,    /* 29: EPWM1P0                    */
	&EPWM1P1_IRQHandler,    /* 30: EPWM1P1                    */
	&EPWM1P2_IRQHandler,    /* 31: EPWM1P2                    */
	&TMR0_IRQHandler,       /* 32: TIMER0                     */
	&TMR1_IRQHandler,       /* 33: TIMER1                     */
	&TMR2_IRQHandler,       /* 34: TIMER2                     */
	&TMR3_IRQHandler,       /* 35: TIMER3                     */
	&UART0_IRQHandler,      /* 36: UART0                      */
	&UART1_IRQHandler,      /* 37: UART1                      */
	&I2C0_IRQHandler,       /* 38: I2C0                       */
	&I2C1_IRQHandler,       /* 39: I2C1                       */
	&PDMA0_IRQHandler,      /* 40: PDMA0                      */
	&DAC_IRQHandler,        /* 41: DAC                        */
	&EADC00_IRQHandler,     /* 42: EADC00                     */
	&EADC01_IRQHandler,     /* 43: EADC01                     */
	&ACMP01_IRQHandler,     /* 44: ACMP01                     */
	&ACMP23_IRQHandler,     /* 45: ACMP23                     */
	&EADC02_IRQHandler,     /* 46: EADC02                     */
	&EADC03_IRQHandler,     /* 47: EADC03                     */
	&UART2_IRQHandler,      /* 48: UART2                      */
	&UART3_IRQHandler,      /* 49: UART3                      */
	&QSPI1_IRQHandler,      /* 50: QSPI1                      */
	&SPI1_IRQHandler,       /* 51: SPI1                       */
	&SPI2_IRQHandler,       /* 52: SPI2                       */
	&USBD_IRQHandler,       /* 53: USBD                       */
	&OHCI_IRQHandler,       /* 54: OHCI                       */
	&USBOTG_IRQHandler,     /* 55: OTG                        */
	&BMC_IRQHandler,        /* 56: BMC                        */
	&SPI5_IRQHandler,       /* 57: SPI5                       */
	&SC0_IRQHandler,        /* 58: SC0                        */
	&SC1_IRQHandler,        /* 59: SC1                        */
	&SC2_IRQHandler,        /* 60: SC2                        */
	&GPJ_IRQHandler,        /* 61: GPJ                        */
	&SPI3_IRQHandler,       /* 62: SPI3                       */
	&SPI4_IRQHandler,       /* 63: SPI4                       */
	&SDH0_IRQHandler,       /* 64: SDH0                       */
	&USBD20_IRQHandler,     /* 65: HSUSBD                     */
	&EMAC0_IRQHandler,      /* 66: EMAC0                      */
	((void *)0),            /* 67: Reserved                   */
	&I2S0_IRQHandler,       /* 68: I2S0                       */
	&I2S1_IRQHandler,       /* 69: I2S1                       */
	&SPI6_IRQHandler,       /* 70: SPI6                       */
	&CRPT_IRQHandler,       /* 71: CRPT                       */
	&GPG_IRQHandler,        /* 72: GPG                        */
	&EINT6_IRQHandler,      /* 73: EINT6                      */
	&UART4_IRQHandler,      /* 74: UART4                      */
	&UART5_IRQHandler,      /* 75: UART5                      */
	&USCI0_IRQHandler,      /* 76: USCI0                      */
	&SPI7_IRQHandler,       /* 77: SPI7                       */
	&BPWM0_IRQHandler,      /* 78: BPWM0                      */
	&BPWM1_IRQHandler,      /* 79: BPWM1                      */
	&SPIM_IRQHandler,       /* 80: SPIM                       */
	&CCAP_IRQHandler,       /* 81: CCAP                       */
	&I2C2_IRQHandler,       /* 82: I2C2                       */
	&I2C3_IRQHandler,       /* 83: I2C3                       */
	&EQEI0_IRQHandler,      /* 84: EQEI0                      */
	&EQEI1_IRQHandler,      /* 85: EQEI1                      */
	&ECAP0_IRQHandler,      /* 86: ECAP0                      */
	&ECAP1_IRQHandler,      /* 87: ECAP1                      */
	&GPH_IRQHandler,        /* 88: GPH                        */
	&EINT7_IRQHandler,      /* 89: EINT7                      */
	&SDH1_IRQHandler,       /* 90: SDH1                       */
	&PSIO_IRQHandler,       /* 91: PSIO                       */
	&EHCI_IRQHandler,       /* 92: EHCI                       */
	&USBOTG20_IRQHandler,   /* 93: HSOTG                      */
	&ECAP2_IRQHandler,      /* 94: ECAP2                      */
	&ECAP3_IRQHandler,      /* 95: ECAP3                      */
	&KPI_IRQHandler,        /* 96: KPI                        */
	&HBI_IRQHandler,        /* 97: HBI                        */
	&PDMA1_IRQHandler,      /* 98: PDMA1                      */
	&UART8_IRQHandler,      /* 99: UART8                      */
	&UART9_IRQHandler,      /* 100: UART9                     */
	&TRNG_IRQHandler,       /* 101: TRNG                      */
	&UART6_IRQHandler,      /* 102: UART6                     */
	&UART7_IRQHandler,      /* 103: UART7                     */
	&EADC10_IRQHandler,     /* 104: EADC10                    */
	&EADC11_IRQHandler,     /* 105: EADC11                    */
	&EADC12_IRQHandler,     /* 106: EADC12                    */
	&EADC13_IRQHandler,     /* 107: EADC13                    */
	&SPI8_IRQHandler,       /* 108: SPI8                      */
	&KS_IRQHandler,         /* 109: KS                        */
	&GPI_IRQHandler,        /* 110: GPI                       */
	&SPI9_IRQHandler,       /* 111: SPI9                      */
	&CANFD00_IRQHandler,    /* 112: CANFD00                   */
	&CANFD01_IRQHandler,    /* 113: CANFD01                   */
	&CANFD10_IRQHandler,    /* 114: CANFD10                   */
	&CANFD10_IRQHandler,    /* 115: CANFD10                   */
	&EQEI2_IRQHandler,      /* 116: EQEI2                     */
	&EQEI3_IRQHandler,      /* 117: EQEI3                     */
	&I2C4_IRQHandler,       /* 118: I2C4                      */
	&SPI10_IRQHandler,      /* 119: SPI10                     */
	&CANFD20_IRQHandler,    /* 120: CANFD20                   */
	&CANFD21_IRQHandler,    /* 121: CANFD21                   */
	&CANFD30_IRQHandler,    /* 122: CANFD30                   */
	&CANFD30_IRQHandler,    /* 123: CANFD30                   */
	&EADC20_IRQHandler,     /* 124: EADC20                    */
	&EADC21_IRQHandler,     /* 125: EADC21                    */
	&EADC22_IRQHandler,     /* 126: EADC22                    */
	&EADC23_IRQHandler,     /* 127: EADC23                    */
};

#endif