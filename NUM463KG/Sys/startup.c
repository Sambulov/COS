extern void *__StackTop;

#ifdef __STARTUP_COPY_MULTIPLE
	extern void *__copy_table_start__; 
  extern void *__copy_table_end__;
#else
	extern void *__etext;
  extern void *__data_start__; 
  extern void *__data_end__;
#endif /*__STARTUP_COPY_MULTIPLE */

#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
  extern void *__zero_table_start__; 
  extern void *__zero_table_end__;
#elif defined (__STARTUP_CLEAR_BSS)
  extern void *__bss_start__; 
  extern void *__bss_end__;
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

void DefaultHandler();
void Reset_Handler();
void NMI_Handler()           __attribute__ ((weak, alias ("DefaultHandler")));
void HardFault_Handler()     __attribute__ ((weak, alias ("DefaultHandler")));
void MemManage_Handler()     __attribute__ ((weak, alias ("DefaultHandler")));
void BusFault_Handler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UsageFault_Handler()    __attribute__ ((weak, alias ("DefaultHandler")));
void SVC_Handler()           __attribute__ ((weak, alias ("DefaultHandler")));
void DebugMon_Handler()      __attribute__ ((weak, alias ("DefaultHandler")));
void PendSV_Handler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SysTick_Handler()       __attribute__ ((weak, alias ("DefaultHandler")));
void BOD_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void IRC_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void PWRWU_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void RAMPE_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void CKFAIL_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void ISP_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void RTC_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void TAMPER_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void WDT_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void WWDT_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void EINT0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EINT1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EINT2_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EINT3_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EINT4_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EINT5_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void GPA_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPB_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPC_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPD_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPE_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPF_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void QSPI0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void SPI0_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void BRAKE0_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM0P0_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM0P1_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM0P2_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void BRAKE1_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM1P0_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM1P1_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EPWM1P2_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void TMR0_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void TMR1_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void TMR2_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void TMR3_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void UART0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void I2C0_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void I2C1_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void PDMA0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void DAC_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void EADC00_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC01_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void ACMP01_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void ACMP23_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC02_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC03_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void UART2_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART3_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void QSPI1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void SPI1_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SPI2_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void USBD_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void OHCI_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void USBOTG_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void BMC_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SPI5_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SC0_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SC1_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SC2_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void GPJ_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SPI3_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SPI4_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SDH0_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void USBD20_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EMAC0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void I2S0_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void I2S1_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SPI6_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void CRPT_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void GPG_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void EINT6_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART4_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART5_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void USCI0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void SPI7_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void BPWM0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void BPWM1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void SPIM_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void CCAP_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void I2C2_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void I2C3_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void EQEI0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EQEI1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void ECAP0_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void ECAP1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void GPH_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void EINT7_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void SDH1_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void PSIO_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void EHCI_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void USBOTG20_IRQHandler()   __attribute__ ((weak, alias ("DefaultHandler")));
void ECAP2_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void ECAP3_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void KPI_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void HBI_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void PDMA1_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART8_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART9_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void TRNG_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void UART6_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void UART7_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EADC10_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC11_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC12_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC13_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void SPI8_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void KS_IRQHandler()         __attribute__ ((weak, alias ("DefaultHandler")));
void GPI_IRQHandler()        __attribute__ ((weak, alias ("DefaultHandler")));
void SPI9_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD00_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD01_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD10_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
//void CANFD10_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EQEI2_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void EQEI3_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void I2C4_IRQHandler()       __attribute__ ((weak, alias ("DefaultHandler")));
void SPI10_IRQHandler()      __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD20_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD21_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void CANFD30_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
//void CANFD30_IRQHandler()    __attribute__ ((weak, alias ("DefaultHandler")));
void EADC20_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC21_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC22_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));
void EADC23_IRQHandler()     __attribute__ ((weak, alias ("DefaultHandler")));

void * __Vectors[0x90] __attribute__ ((section (".vectors"), used)) = {
	&__StackTop,            /* Top of Stack */
	&Reset_Handler,         /* Reset Handler */
	&NMI_Handler,           /* NMI Handler */
	&HardFault_Handler,     /* Hard Fault Handler */
	&MemManage_Handler,     /* MPU Fault Handler */
	&BusFault_Handler,      /* Bus Fault Handler */
	&UsageFault_Handler,    /* Usage Fault Handler */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	&SVC_Handler,           /* SVCall Handler */
	&DebugMon_Handler,      /* Debug Monitor Handler */
	((void *)0),            /* Reserved */
	&PendSV_Handler,        /* PendSV Handler */
	&SysTick_Handler,       /* SysTick Handler */
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

void SystemInit();
void SystemEnableSpimCache();
void UnlockRegister();
void LockRegister();
void __libc_init_array();
void main();

void __attribute__((naked, noreturn)) Reset_Handler() {
	//asm ("ldr sp, =_estack");

	void **pSource, **pDest;

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */
#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	void **pTabItem, **pSourceEnd;
	for (pTabItem = &__copy_table_start__; pTabItem != &__copy_table_end__; pTabItem += 3) {
    pSource = pTabItem[0];
    pDest = pTabItem[1];
    pSourceEnd = pSource + (pTabItem[2] >> 2);
	  for(; pSource != pSourceEnd; pSource++, pDest++) {
      *pDest = *pSource;
    }
  }
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	for (pSource = &__etext, pDest = &__data_start__; pDest != &__data_end__; pSource++, pDest++)
	  *pDest = *pSource;
#endif /*__STARTUP_COPY_MULTIPLE */


/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */

#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
	void **pTabItem;
  for(pTabItem = &__zero_table_start__; pTabItem != &__zero_table_end__; pTabItem += 2) {
  	void **pEnd = pTabItem + (pTabItem[1] >> 2);
    for (pDest = pTabItem[0]; pDest != pEnd; pDest++)
      *pDest = 0;
  }
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
  for (pDest = &__bss_start__; pDest != &__bss_end__; pDest++)
    *pDest = 0;
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

/*  Unlock Register */
  UnlockRegister();

#ifndef ENABLE_SPIM_CACHE
  SystemEnableSpimCache();
#endif

#ifndef __NO_SYSTEM_INIT
	SystemInit();
#endif

//TODO: turn off JTAG if needed;

/* Init POR */
#if 0
	ldr	r0, =0x40000024
	ldr	r1, =0x00005AA5
	str	r1, [r0]
#endif

/* Lock register */
  LockRegister();

  __libc_init_array();

  main();
  for (;;) ;
}

void __attribute__((naked, noreturn)) DefaultHandler() {
	//If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
}
