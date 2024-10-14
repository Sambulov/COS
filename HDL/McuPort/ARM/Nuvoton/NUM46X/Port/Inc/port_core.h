
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#define HDL_VTOR_TAB_ALIGN         256  //(2 << SCB_VTOR_TBLOFF_Pos)

#define HDL_INTERRUPT_PRV_SIZE       4

typedef enum
{
  /* Cortex-M4 processor exceptions numbers */
  HDL_NVIC_EXCEPTION_NonMaskableInt = -14,   /*!< Non maskable interrupt                                 */
  HDL_NVIC_EXCEPTION_HardFault = -13,        /*!< Hardfault interrupt                                      */
  HDL_NVIC_EXCEPTION_MemoryManagement = -12, /*!< Memory management interrupt                  */
  HDL_NVIC_EXCEPTION_BusFault = -11,         /*!< Bus fault interrupt                          */
  HDL_NVIC_EXCEPTION_UsageFault = -10,       /*!< Usage fault interrupt                        */
  HDL_NVIC_EXCEPTION_SVCall = -5,            /*!< SV call interrupt                           */
  HDL_NVIC_EXCEPTION_DebugMonitor = -4,      /*!< Debug monitor interrupt                     */
  HDL_NVIC_EXCEPTION_PendSV = -2,            /*!< Pend SV interrupt                           */
  HDL_NVIC_EXCEPTION_SysTick = -1,           /*!< System tick interrupt                       */
  /* interruput numbers */
  HDL_NVIC_IRQ0_BOD_IRQn                      = 0,        /*!< Brown Out detection Interrupt                    */
  HDL_NVIC_IRQ1_IRC_IRQn                      = 1,        /*!< Internal RC Interrupt                            */
  HDL_NVIC_IRQ2_PWRWU_IRQn                    = 2,        /*!< Power Down Wake Up Interrupt                     */
  HDL_NVIC_IRQ3_RAMPE_IRQn                    = 3,        /*!< SRAM parity check failed Interrupt               */
  HDL_NVIC_IRQ4_CKFAIL_IRQn                   = 4,        /*!< Clock failed Interrupt                           */
  HDL_NVIC_IRQ5_ISP_IRQn                      = 5,        /*!< FMC ISP Interrupt                                */
  HDL_NVIC_IRQ6_RTC_IRQn                      = 6,        /*!< Real Time Clock Interrupt                        */
  HDL_NVIC_IRQ7_TAMPER_IRQn                   = 7,        /*!< Tamper detection Interrupt                       */
  HDL_NVIC_IRQ8_WDT_IRQn                      = 8,        /*!< Watchdog timer Interrupt                         */
  HDL_NVIC_IRQ9_WWDT_IRQn                     = 9,        /*!< Window Watchdog timer Interrupt                  */
  HDL_NVIC_IRQ0_EINT0_IRQn                    = 10,       /*!< External Input 0 Interrupt                       */
  HDL_NVIC_IRQ11_EINT1_IRQn                   = 11,       /*!< External Input 1 Interrupt                       */
  HDL_NVIC_IRQ12_EINT2_IRQn                   = 12,       /*!< External Input 2 Interrupt                       */
  HDL_NVIC_IRQ13_EINT3_IRQn                   = 13,       /*!< External Input 3 Interrupt                       */
  HDL_NVIC_IRQ14_EINT4_IRQn                   = 14,       /*!< External Input 4 Interrupt                       */
  HDL_NVIC_IRQ15_EINT5_IRQn                   = 15,       /*!< External Input 5 Interrupt                       */
  HDL_NVIC_IRQ16_GPA_IRQn                     = 16,       /*!< GPIO Port A Interrupt                            */
  HDL_NVIC_IRQ17_GPB_IRQn                     = 17,       /*!< GPIO Port B Interrupt                            */
  HDL_NVIC_IRQ18_GPC_IRQn                     = 18,       /*!< GPIO Port C Interrupt                            */
  HDL_NVIC_IRQ19_GPD_IRQn                     = 19,       /*!< GPIO Port D Interrupt                            */
  HDL_NVIC_IRQ20_GPE_IRQn                     = 20,       /*!< GPIO Port E Interrupt                            */
  HDL_NVIC_IRQ21_GPF_IRQn                     = 21,       /*!< GPIO Port F Interrupt                            */
  HDL_NVIC_IRQ22_QSPI0_IRQn                   = 22,       /*!< QSPI0 Interrupt                                   */
  HDL_NVIC_IRQ23_SPI0_IRQn                    = 23,       /*!< SPI0 Interrupt                                   */
  HDL_NVIC_IRQ24_BRAKE0_IRQn                  = 24,       /*!< BRAKE0 Interrupt                                 */
  HDL_NVIC_IRQ25_EPWM0P0_IRQn                 = 25,       /*!< EPWM0P0 Interrupt                                */
  HDL_NVIC_IRQ26_EPWM0P1_IRQn                 = 26,       /*!< EPWM0P1 Interrupt                                */
  HDL_NVIC_IRQ27_EPWM0P2_IRQn                 = 27,       /*!< EPWM0P2 Interrupt                                */
  HDL_NVIC_IRQ28_BRAKE1_IRQn                  = 28,       /*!< BRAKE1 Interrupt                                 */
  HDL_NVIC_IRQ29_EPWM1P0_IRQn                 = 29,       /*!< EPWM1P0 Interrupt                                */
  HDL_NVIC_IRQ30_EPWM1P1_IRQn                 = 30,       /*!< EPWM1P1 Interrupt                                */
  HDL_NVIC_IRQ31_EPWM1P2_IRQn                 = 31,       /*!< EPWM1P2 Interrupt                                */
  HDL_NVIC_IRQ32_TMR0_IRQn                    = 32,       /*!< Timer 0 Interrupt                                */
  HDL_NVIC_IRQ33_TMR1_IRQn                    = 33,       /*!< Timer 1 Interrupt                                */
  HDL_NVIC_IRQ34_TMR2_IRQn                    = 34,       /*!< Timer 2 Interrupt                                */
  HDL_NVIC_IRQ35_TMR3_IRQn                    = 35,       /*!< Timer 3 Interrupt                                */
  HDL_NVIC_IRQ36_UART0_IRQn                   = 36,       /*!< UART 0 Interrupt                                 */
  HDL_NVIC_IRQ37_UART1_IRQn                   = 37,       /*!< UART 1 Interrupt                                 */
  HDL_NVIC_IRQ38_I2C0_IRQn                    = 38,       /*!< I2C 0 Interrupt                                  */
  HDL_NVIC_IRQ39_I2C1_IRQn                    = 39,       /*!< I2C 1 Interrupt                                  */
  HDL_NVIC_IRQ40_PDMA0_IRQn                   = 40,       /*!< Peripheral DMA 0 Interrupt                       */
  HDL_NVIC_IRQ41_DAC_IRQn                     = 41,       /*!< DAC Interrupt                                    */
  HDL_NVIC_IRQ42_EADC00_IRQn                  = 42,       /*!< EADC00 Interrupt                                 */
  HDL_NVIC_IRQ43_EADC01_IRQn                  = 43,       /*!< EADC01 Interrupt                                 */
  HDL_NVIC_IRQ44_ACMP01_IRQn                  = 44,       /*!< Analog Comparator 0 and 1 Interrupt              */
  HDL_NVIC_IRQ45_ACMP23_IRQn                  = 45,       /*!< Analog Comparator 2 and 3 Interrupt              */
  HDL_NVIC_IRQ46_EADC02_IRQn                  = 46,       /*!< EADC02 Interrupt                                 */
  HDL_NVIC_IRQ47_EADC03_IRQn                  = 47,       /*!< EADC03 Interrupt                                 */
  HDL_NVIC_IRQ48_UART2_IRQn                   = 48,       /*!< UART2 Interrupt                                  */
  HDL_NVIC_IRQ49_UART3_IRQn                   = 49,       /*!< UART3 Interrupt                                  */
  HDL_NVIC_IRQ50_QSPI1_IRQn                   = 50,       /*!< QSPI1 Interrupt                                  */
  HDL_NVIC_IRQ51_SPI1_IRQn                    = 51,       /*!< SPI1 Interrupt                                   */
  HDL_NVIC_IRQ52_SPI2_IRQn                    = 52,       /*!< SPI2 Interrupt                                   */
  HDL_NVIC_IRQ53_USBD_IRQn                    = 53,       /*!< USB device Interrupt                             */
  HDL_NVIC_IRQ54_USBH_IRQn                    = 54,       /*!< USB host Interrupt                               */
  HDL_NVIC_IRQ55_USBOTG_IRQn                  = 55,       /*!< USB OTG Interrupt                                */
  HDL_NVIC_IRQ56_BMC_IRQn                     = 56,       /*!< BMC Interrupt                                    */
  HDL_NVIC_IRQ57_SPI5_IRQn                    = 57,       /*!< SPI5 Interrupt                                   */
  HDL_NVIC_IRQ58_SC0_IRQn                     = 58,       /*!< Smart Card 0 Interrupt                           */
  HDL_NVIC_IRQ59_SC1_IRQn                     = 59,       /*!< Smart Card 1 Interrupt                           */
  HDL_NVIC_IRQ60_SC2_IRQn                     = 60,       /*!< Smart Card 2 Interrupt                           */
  HDL_NVIC_IRQ61_GPJ_IRQn                     = 61,       /*!< GPIO Port J Interrupt                            */
  HDL_NVIC_IRQ62_SPI3_IRQn                    = 62,       /*!< SPI3 Interrupt                                   */
  HDL_NVIC_IRQ63_SPI4_IRQn                    = 63,       /*!< SPI4 Interrupt                                   */
  HDL_NVIC_IRQ64_SDH0_IRQn                    = 64,       /*!< Secure Digital Host Controller 0 Interrupt       */
  HDL_NVIC_IRQ65_USBD20_IRQn                  = 65,       /*!< High Speed USB device Interrupt                  */
  HDL_NVIC_IRQ66_EMAC0_TXRX_IRQn              = 66,       /*!< Ethernet MAC 0 Interrupt                         */
  HDL_NVIC_IRQ67                              = 67,       /*!< */
  HDL_NVIC_IRQ68_I2S0_IRQn                    = 68,       /*!< I2S0 Interrupt                                   */
  HDL_NVIC_IRQ69_I2S1_IRQn                    = 69,       /*!< I2S1 Interrupt                                   */
  HDL_NVIC_IRQ70_SPI6_IRQn                    = 70,       /*!< SPI6 Interrupt                                   */
  HDL_NVIC_IRQ71_CRPT_IRQn                    = 71,       /*!< CRPT Interrupt                                   */
  HDL_NVIC_IRQ72_GPG_IRQn                     = 72,       /*!< GPIO Port G Interrupt                            */
  HDL_NVIC_IRQ73_EINT6_IRQn                   = 73,       /*!< External Input 6 Interrupt                       */
  HDL_NVIC_IRQ74_UART4_IRQn                   = 74,       /*!< UART4 Interrupt                                  */
  HDL_NVIC_IRQ75_UART5_IRQn                   = 75,       /*!< UART5 Interrupt                                  */
  HDL_NVIC_IRQ76_USCI0_IRQn                   = 76,       /*!< USCI0 Interrupt                                  */
  HDL_NVIC_IRQ77_SPI7_IRQn                    = 77,       /*!< SPI7 Interrupt                                   */
  HDL_NVIC_IRQ78_BPWM0_IRQn                   = 78,       /*!< BPWM0 Interrupt                                  */
  HDL_NVIC_IRQ79_BPWM1_IRQn                   = 79,       /*!< BPWM1 Interrupt                                  */
  HDL_NVIC_IRQ80_SPIM_IRQn                    = 80,       /*!< SPIM Interrupt                                   */
  HDL_NVIC_IRQ81_CCAP_IRQn                    = 81,       /*!< CCAP Interrupt                                   */
  HDL_NVIC_IRQ82_I2C2_IRQn                    = 82,       /*!< I2C2 Interrupt                                   */
  HDL_NVIC_IRQ83_I2C3_IRQn                    = 83,       /*!< I2C3 Interrupt                                   */
  HDL_NVIC_IRQ84_EQEI0_IRQn                   = 84,       /*!< EQEI0 Interrupt                                  */
  HDL_NVIC_IRQ85_EQEI1_IRQn                   = 85,       /*!< EQEI1 Interrupt                                  */
  HDL_NVIC_IRQ86_ECAP0_IRQn                   = 86,       /*!< ECAP0 Interrupt                                  */
  HDL_NVIC_IRQ87_ECAP1_IRQn                   = 87,       /*!< ECAP1 Interrupt                                  */
  HDL_NVIC_IRQ88_GPH_IRQn                     = 88,       /*!< GPIO Port H Interrupt                            */
  HDL_NVIC_IRQ89_EINT7_IRQn                   = 89,       /*!< External Input 7 Interrupt                       */
  HDL_NVIC_IRQ90_SDH1_IRQn                    = 90,       /*!< Secure Digital Host Controller 1 Interrupt       */
  HDL_NVIC_IRQ91_PSIO_IRQn                    = 91,       /*!< PSIO Interrupt                                   */
  HDL_NVIC_IRQ92_HSUSBH_IRQn                  = 92,       /*!< High speed USB host Interrupt                    */
  HDL_NVIC_IRQ93_USBOTG20_IRQn                = 93,       /*!< High speed USB OTG Interrupt                     */
  HDL_NVIC_IRQ94_ECAP2_IRQn                   = 94,       /*!< ECAP2 Interrupt                                  */
  HDL_NVIC_IRQ95_ECAP3_IRQn                   = 95,       /*!< ECAP3 Interrupt                                  */
  HDL_NVIC_IRQ96_KPI_IRQn                     = 96,       /*!< Keypad Interface Interrupt                       */
  HDL_NVIC_IRQ97_HBI_IRQn                     = 97,       /*!< HBI Interrupt                                    */
  HDL_NVIC_IRQ98_PDMA1_IRQn                   = 98,       /*!< Peripheral DMA 1 Interrupt                       */
  HDL_NVIC_IRQ99_UART8_IRQn                   = 99,       /*!< UART8 Interrupt                                  */
  HDL_NVIC_IRQ100_UART9_IRQn                  = 100,      /*!< UART9 Interrupt                                  */
  HDL_NVIC_IRQ101_TRNG_IRQn                   = 101,      /*!< TRNG Interrupt                                   */
  HDL_NVIC_IRQ102_UART6_IRQn                  = 102,      /*!< UART6 Interrupt                                  */
  HDL_NVIC_IRQ103_UART7_IRQn                  = 103,      /*!< UART7 Interrupt                                  */
  HDL_NVIC_IRQ104_EADC10_IRQn                 = 104,      /*!< EADC10 Interrupt                                 */
  HDL_NVIC_IRQ105_EADC11_IRQn                 = 105,      /*!< EADC11 Interrupt                                 */
  HDL_NVIC_IRQ106_EADC12_IRQn                 = 106,      /*!< EADC12 Interrupt                                 */
  HDL_NVIC_IRQ107_EADC13_IRQn                 = 107,      /*!< EADC13 Interrupt                                 */
  HDL_NVIC_IRQ108_SPI8_IRQn                   = 108,      /*!< SPI8 Interrupt                                   */
  HDL_NVIC_IRQ109_KS_IRQn                     = 109,      /*!< Keystore Interrupt                               */
  HDL_NVIC_IRQ110_GPI_IRQn                    = 110,      /*!< GPIO Port I Interrupt                            */
  HDL_NVIC_IRQ111_SPI9_IRQn                   = 111,      /*!< SPI9 Interrupt                                   */
  HDL_NVIC_IRQ112_CANFD00_IRQn                = 112,      /*!< CANFD00 Interrupt                                */
  HDL_NVIC_IRQ113_CANFD01_IRQn                = 113,      /*!< CANFD01 Interrupt                                */
  HDL_NVIC_IRQ114_CANFD10_IRQn                = 114,      /*!< CANFD10 Interrupt                                */
  HDL_NVIC_IRQ115_CANFD11_IRQn                = 115,      /*!< CANFD11 Interrupt                                */
  HDL_NVIC_IRQ116_EQEI2_IRQn                  = 116,      /*!< EQEI2 Interrupt                                  */
  HDL_NVIC_IRQ117_EQEI3_IRQn                  = 117,      /*!< EQEI3 Interrupt                                  */
  HDL_NVIC_IRQ118_I2C4_IRQn                   = 118,      /*!< I2C4 Interrupt                                   */
  HDL_NVIC_IRQ119_SPI10_IRQn                  = 119,      /*!< SPI10 Interrupt                                  */
  HDL_NVIC_IRQ120_CANFD20_IRQn                = 120,      /*!< CANFD20 Interrupt                                */
  HDL_NVIC_IRQ121_CANFD21_IRQn                = 121,      /*!< CANFD21 Interrupt                                */
  HDL_NVIC_IRQ122_CANFD30_IRQn                = 122,      /*!< CANFD30 Interrupt                                */
  HDL_NVIC_IRQ123_CANFD31_IRQn                = 123,      /*!< CANFD31 Interrupt                                */
  HDL_NVIC_IRQ124_EADC20_IRQn                 = 124,      /*!< EADC20 Interrupt                                 */
  HDL_NVIC_IRQ125_EADC21_IRQn                 = 125,      /*!< EADC21 Interrupt                                 */
  HDL_NVIC_IRQ126_EADC22_IRQn                 = 126,      /*!< EADC22 Interrupt                                 */
  HDL_NVIC_IRQ127_EADC23_IRQn                 = 127,      /*!< EADC23 Interrupt                                 */
} hdl_nvic_irq_n_t;

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  PRIVATE(hw, HDL_INTERRUPT_PRV_SIZE);
} hdl_interrupt_t;

typedef struct {
  uint32_t prio_bits;
  uint32_t prio_group;
  hdl_interrupt_t **interrupts;
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
  const void * const vector;
} hdl_interrupt_controller_config_t;

typedef struct{
  uint32_t flash_latency;
} hdl_core_config_t;

extern const void * _estack;
extern const void *_sidata, *_sdata, *_edata;
extern const void *_sbss, *_ebss;
extern const void *_eflash;

#define hdl_interrupts(...) ((hdl_nvic_interrupt_t *[]){__VA_ARGS__, NULL})

void call_isr(hdl_nvic_irq_n_t irq, uint32_t event);

void irq_n_handler();

void reset_handler();

void nmi_handler();
void hard_fault_handler();
void pend_sv_handler();
void systick_handler();
void mem_manage_handler();
void bus_fault_handler();
void usage_fault_handler();
void debug_mon_handler();
void svc_handler();

void BOD_IRQHandler();
void IRC_IRQHandler();
void PWRWU_IRQHandler();
void RAMPE_IRQHandler();
void CKFAIL_IRQHandler();
void ISP_IRQHandler();
void RTC_IRQHandler();
void TAMPER_IRQHandler();
void WDT_IRQHandler();
void WWDT_IRQHandler();
void EINT0_IRQHandler();
void EINT1_IRQHandler();
void EINT2_IRQHandler();
void EINT3_IRQHandler();
void EINT4_IRQHandler();
void EINT5_IRQHandler();
void GPA_IRQHandler();
void GPB_IRQHandler();
void GPC_IRQHandler();
void GPD_IRQHandler();
void GPE_IRQHandler();
void GPF_IRQHandler();
void QSPI0_IRQHandler();
void SPI0_IRQHandler();
void BRAKE0_IRQHandler();
void EPWM0P0_IRQHandler();
void EPWM0P1_IRQHandler();
void EPWM0P2_IRQHandler();
void BRAKE1_IRQHandler();
void EPWM1P0_IRQHandler();
void EPWM1P1_IRQHandler();
void EPWM1P2_IRQHandler();
void TMR0_IRQHandler();
void TMR1_IRQHandler();
void TMR2_IRQHandler();
void TMR3_IRQHandler();
void UART0_IRQHandler();
void UART1_IRQHandler();
void I2C0_IRQHandler();
void I2C1_IRQHandler();
void PDMA0_IRQHandler();
void DAC_IRQHandler();
void EADC00_IRQHandler();
void EADC01_IRQHandler();
void ACMP01_IRQHandler();
void ACMP23_IRQHandler();
void EADC02_IRQHandler();
void EADC03_IRQHandler();
void UART2_IRQHandler();
void UART3_IRQHandler();
void QSPI1_IRQHandler();
void SPI1_IRQHandler();
void SPI2_IRQHandler();
void USBD_IRQHandler();
void OHCI_IRQHandler();
void USBOTG_IRQHandler();
void BMC_IRQHandler();
void SPI5_IRQHandler();
void SC0_IRQHandler();
void SC1_IRQHandler();
void SC2_IRQHandler();
void GPJ_IRQHandler();
void SPI3_IRQHandler();
void SPI4_IRQHandler();
void SDH0_IRQHandler();
void USBD20_IRQHandler();
void EMAC0_IRQHandler();
void I2S0_IRQHandler();
void I2S1_IRQHandler();
void SPI6_IRQHandler();
void CRPT_IRQHandler();
void GPG_IRQHandler();
void EINT6_IRQHandler();
void UART4_IRQHandler();
void UART5_IRQHandler();
void USCI0_IRQHandler();
void SPI7_IRQHandler();
void BPWM0_IRQHandler();
void BPWM1_IRQHandler();
void SPIM_IRQHandler();
void CCAP_IRQHandler();
void I2C2_IRQHandler();
void I2C3_IRQHandler();
void EQEI0_IRQHandler();
void EQEI1_IRQHandler();
void ECAP0_IRQHandler();
void ECAP1_IRQHandler();
void GPH_IRQHandler();
void EINT7_IRQHandler();
void SDH1_IRQHandler();
void PSIO_IRQHandler();
void EHCI_IRQHandler();
void USBOTG20_IRQHandler();
void ECAP2_IRQHandler();
void ECAP3_IRQHandler();
void KPI_IRQHandler();
void HBI_IRQHandler();
void PDMA1_IRQHandler();
void UART8_IRQHandler();
void UART9_IRQHandler();
void TRNG_IRQHandler();
void UART6_IRQHandler();
void UART7_IRQHandler();
void EADC10_IRQHandler();
void EADC11_IRQHandler();
void EADC12_IRQHandler();
void EADC13_IRQHandler();
void SPI8_IRQHandler();
void KS_IRQHandler();
void GPI_IRQHandler();
void SPI9_IRQHandler();
void CANFD00_IRQHandler();
void CANFD01_IRQHandler();
void CANFD10_IRQHandler();
void CANFD11_IRQHandler();
void EQEI2_IRQHandler();
void EQEI3_IRQHandler();
void I2C4_IRQHandler();
void SPI10_IRQHandler();
void CANFD20_IRQHandler();
void CANFD21_IRQHandler();
void CANFD30_IRQHandler();
void CANFD31_IRQHandler();
void EADC20_IRQHandler();
void EADC21_IRQHandler();
void EADC22_IRQHandler();
void EADC23_IRQHandler();


#endif // PORT_CORE_H_
