#include "hdl_portable.h"

void BOD_IRQHandler()        { call_isr(HDL_NVIC_IRQ0_BOD_IRQn, 0); }
void IRC_IRQHandler()        { call_isr(HDL_NVIC_IRQ1_IRC_IRQn, 0); }
void PWRWU_IRQHandler()      { call_isr(HDL_NVIC_IRQ2_PWRWU_IRQn, 0); }
void RAMPE_IRQHandler()      { call_isr(HDL_NVIC_IRQ3_RAMPE_IRQn, 0); }
void CKFAIL_IRQHandler()     { call_isr(HDL_NVIC_IRQ4_CKFAIL_IRQn, 0); }
void ISP_IRQHandler()        { call_isr(HDL_NVIC_IRQ5_ISP_IRQn, 0); }
void RTC_IRQHandler()        { call_isr(HDL_NVIC_IRQ6_RTC_IRQn, 0); }
void TAMPER_IRQHandler()     { call_isr(HDL_NVIC_IRQ7_TAMPER_IRQn, 0); }
void WDT_IRQHandler()        { call_isr(HDL_NVIC_IRQ8_WDT_IRQn, 0); }
void WWDT_IRQHandler()       { call_isr(HDL_NVIC_IRQ9_WWDT_IRQn, 0); }
void EINT0_IRQHandler()      { call_isr(HDL_NVIC_IRQ0_EINT0_IRQn, 0); }
void EINT1_IRQHandler()      { call_isr(HDL_NVIC_IRQ11_EINT1_IRQn, 0); }
void EINT2_IRQHandler()      { call_isr(HDL_NVIC_IRQ12_EINT2_IRQn, 0); }
void EINT3_IRQHandler()      { call_isr(HDL_NVIC_IRQ13_EINT3_IRQn, 0); }
void EINT4_IRQHandler()      { call_isr(HDL_NVIC_IRQ14_EINT4_IRQn, 0); }
void EINT5_IRQHandler()      { call_isr(HDL_NVIC_IRQ15_EINT5_IRQn, 0); }
void GPA_IRQHandler()        { call_isr(HDL_NVIC_IRQ16_GPA_IRQn, 0); }
void GPB_IRQHandler()        { call_isr(HDL_NVIC_IRQ17_GPB_IRQn, 0); }
void GPC_IRQHandler()        { call_isr(HDL_NVIC_IRQ18_GPC_IRQn, 0); }
void GPD_IRQHandler()        { call_isr(HDL_NVIC_IRQ19_GPD_IRQn, 0); }
void GPE_IRQHandler()        { call_isr(HDL_NVIC_IRQ20_GPE_IRQn, 0); }
void GPF_IRQHandler()        { call_isr(HDL_NVIC_IRQ21_GPF_IRQn, 0); }
void QSPI0_IRQHandler()      { call_isr(HDL_NVIC_IRQ22_QSPI0_IRQn, 0); }
void SPI0_IRQHandler()       { call_isr(HDL_NVIC_IRQ23_SPI0_IRQn, 0); }
void BRAKE0_IRQHandler()     { call_isr(HDL_NVIC_IRQ24_BRAKE0_IRQn, 0); }
void EPWM0P0_IRQHandler()    { call_isr(HDL_NVIC_IRQ25_EPWM0P0_IRQn, 0); }
void EPWM0P1_IRQHandler()    { call_isr(HDL_NVIC_IRQ26_EPWM0P1_IRQn, 0); }
void EPWM0P2_IRQHandler()    { call_isr(HDL_NVIC_IRQ27_EPWM0P2_IRQn, 0); }
void BRAKE1_IRQHandler()     { call_isr(HDL_NVIC_IRQ28_BRAKE1_IRQn, 0); }
void EPWM1P0_IRQHandler()    { call_isr(HDL_NVIC_IRQ29_EPWM1P0_IRQn, 0); }
void EPWM1P1_IRQHandler()    { call_isr(HDL_NVIC_IRQ30_EPWM1P1_IRQn, 0); }
void EPWM1P2_IRQHandler()    { call_isr(HDL_NVIC_IRQ31_EPWM1P2_IRQn, 0); }
void TMR0_IRQHandler()       { call_isr(HDL_NVIC_IRQ32_TMR0_IRQn, 0); }
void TMR1_IRQHandler()       { call_isr(HDL_NVIC_IRQ33_TMR1_IRQn, 0); }
void TMR2_IRQHandler()       { call_isr(HDL_NVIC_IRQ34_TMR2_IRQn, 0); }
void TMR3_IRQHandler()       { call_isr(HDL_NVIC_IRQ35_TMR3_IRQn, 0); }
void UART0_IRQHandler()      { call_isr(HDL_NVIC_IRQ36_UART0_IRQn, 0); }
void UART1_IRQHandler()      { call_isr(HDL_NVIC_IRQ37_UART1_IRQn, 0); }
void I2C0_IRQHandler()       { call_isr(HDL_NVIC_IRQ38_I2C0_IRQn, 0); }
void I2C1_IRQHandler()       { call_isr(HDL_NVIC_IRQ39_I2C1_IRQn, 0); }
void PDMA0_IRQHandler()      { call_isr(HDL_NVIC_IRQ40_PDMA0_IRQn, 0); }
void DAC_IRQHandler()        { call_isr(HDL_NVIC_IRQ41_DAC_IRQn, 0); }
void EADC00_IRQHandler()     { call_isr(HDL_NVIC_IRQ42_EADC00_IRQn, 0); }
void EADC01_IRQHandler()     { call_isr(HDL_NVIC_IRQ43_EADC01_IRQn, 0); }
void ACMP01_IRQHandler()     { call_isr(HDL_NVIC_IRQ44_ACMP01_IRQn, 0); }
void ACMP23_IRQHandler()     { call_isr(HDL_NVIC_IRQ45_ACMP23_IRQn, 0); }
void EADC02_IRQHandler()     { call_isr(HDL_NVIC_IRQ46_EADC02_IRQn, 0); }
void EADC03_IRQHandler()     { call_isr(HDL_NVIC_IRQ47_EADC03_IRQn, 0); }
void UART2_IRQHandler()      { call_isr(HDL_NVIC_IRQ48_UART2_IRQn, 0); }
void UART3_IRQHandler()      { call_isr(HDL_NVIC_IRQ49_UART3_IRQn, 0); }
void QSPI1_IRQHandler()      { call_isr(HDL_NVIC_IRQ50_QSPI1_IRQn, 0); }
void SPI1_IRQHandler()       { call_isr(HDL_NVIC_IRQ51_SPI1_IRQn, 0); }
void SPI2_IRQHandler()       { call_isr(HDL_NVIC_IRQ52_SPI2_IRQn, 0); }
void USBD_IRQHandler()       { call_isr(HDL_NVIC_IRQ53_USBD_IRQn, 0); }
void OHCI_IRQHandler()       { call_isr(HDL_NVIC_IRQ54_USBH_IRQn, 0); }
void USBOTG_IRQHandler()     { call_isr(HDL_NVIC_IRQ55_USBOTG_IRQn, 0); }
void BMC_IRQHandler()        { call_isr(HDL_NVIC_IRQ56_BMC_IRQn, 0); }
void SPI5_IRQHandler()       { call_isr(HDL_NVIC_IRQ57_SPI5_IRQn, 0); }
void SC0_IRQHandler()        { call_isr(HDL_NVIC_IRQ58_SC0_IRQn, 0); }
void SC1_IRQHandler()        { call_isr(HDL_NVIC_IRQ59_SC1_IRQn, 0); }
void SC2_IRQHandler()        { call_isr(HDL_NVIC_IRQ60_SC2_IRQn, 0); }
void GPJ_IRQHandler()        { call_isr(HDL_NVIC_IRQ61_GPJ_IRQn, 0); }
void SPI3_IRQHandler()       { call_isr(HDL_NVIC_IRQ62_SPI3_IRQn, 0); }
void SPI4_IRQHandler()       { call_isr(HDL_NVIC_IRQ63_SPI4_IRQn, 0); }
void SDH0_IRQHandler()       { call_isr(HDL_NVIC_IRQ64_SDH0_IRQn, 0); }
void USBD20_IRQHandler()     { call_isr(HDL_NVIC_IRQ65_USBD20_IRQn, 0); }
void EMAC0_IRQHandler()      { call_isr(HDL_NVIC_IRQ66_EMAC0_TXRX_IRQn, 0); }
void I2S0_IRQHandler()       { call_isr(HDL_NVIC_IRQ68_I2S0_IRQn, 0); }
void I2S1_IRQHandler()       { call_isr(HDL_NVIC_IRQ69_I2S1_IRQn, 0); }
void SPI6_IRQHandler()       { call_isr(HDL_NVIC_IRQ70_SPI6_IRQn, 0); }
void CRPT_IRQHandler()       { call_isr(HDL_NVIC_IRQ71_CRPT_IRQn, 0); }
void GPG_IRQHandler()        { call_isr(HDL_NVIC_IRQ72_GPG_IRQn, 0); }
void EINT6_IRQHandler()      { call_isr(HDL_NVIC_IRQ73_EINT6_IRQn, 0); }
void UART4_IRQHandler()      { call_isr(HDL_NVIC_IRQ74_UART4_IRQn, 0); }
void UART5_IRQHandler()      { call_isr(HDL_NVIC_IRQ75_UART5_IRQn, 0); }
void USCI0_IRQHandler()      { call_isr(HDL_NVIC_IRQ76_USCI0_IRQn, 0); }
void SPI7_IRQHandler()       { call_isr(HDL_NVIC_IRQ77_SPI7_IRQn, 0); }
void BPWM0_IRQHandler()      { call_isr(HDL_NVIC_IRQ78_BPWM0_IRQn, 0); }
void BPWM1_IRQHandler()      { call_isr(HDL_NVIC_IRQ79_BPWM1_IRQn, 0); }
void SPIM_IRQHandler()       { call_isr(HDL_NVIC_IRQ80_SPIM_IRQn, 0); }
void CCAP_IRQHandler()       { call_isr(HDL_NVIC_IRQ81_CCAP_IRQn, 0); }
void I2C2_IRQHandler()       { call_isr(HDL_NVIC_IRQ82_I2C2_IRQn, 0); }
void I2C3_IRQHandler()       { call_isr(HDL_NVIC_IRQ83_I2C3_IRQn, 0); }
void EQEI0_IRQHandler()      { call_isr(HDL_NVIC_IRQ84_EQEI0_IRQn, 0); }
void EQEI1_IRQHandler()      { call_isr(HDL_NVIC_IRQ85_EQEI1_IRQn, 0); }
void ECAP0_IRQHandler()      { call_isr(HDL_NVIC_IRQ86_ECAP0_IRQn, 0); }
void ECAP1_IRQHandler()      { call_isr(HDL_NVIC_IRQ87_ECAP1_IRQn, 0); }
void GPH_IRQHandler()        { call_isr(HDL_NVIC_IRQ88_GPH_IRQn, 0); }
void EINT7_IRQHandler()      { call_isr(HDL_NVIC_IRQ89_EINT7_IRQn, 0); }
void SDH1_IRQHandler()       { call_isr(HDL_NVIC_IRQ90_SDH1_IRQn, 0); }
void PSIO_IRQHandler()       { call_isr(HDL_NVIC_IRQ91_PSIO_IRQn, 0); }
void EHCI_IRQHandler()       { call_isr(HDL_NVIC_IRQ92_HSUSBH_IRQn, 0); }
void USBOTG20_IRQHandler()   { call_isr(HDL_NVIC_IRQ93_USBOTG20_IRQn, 0); }
void ECAP2_IRQHandler()      { call_isr(HDL_NVIC_IRQ94_ECAP2_IRQn, 0); }
void ECAP3_IRQHandler()      { call_isr(HDL_NVIC_IRQ95_ECAP3_IRQn, 0); }
//void KPI_IRQHandler()        { call_isr(HDL_NVIC_IRQ96_KPI_IRQn, 0); }
void HBI_IRQHandler()        { call_isr(HDL_NVIC_IRQ97_HBI_IRQn, 0); }
void PDMA1_IRQHandler()      { call_isr(HDL_NVIC_IRQ98_PDMA1_IRQn, 0); }
void UART8_IRQHandler()      { call_isr(HDL_NVIC_IRQ99_UART8_IRQn, 0); }
void UART9_IRQHandler()      { call_isr(HDL_NVIC_IRQ100_UART9_IRQn, 0); }
void TRNG_IRQHandler()       { call_isr(HDL_NVIC_IRQ101_TRNG_IRQn, 0); }
void UART6_IRQHandler()      { call_isr(HDL_NVIC_IRQ102_UART6_IRQn, 0); }
void UART7_IRQHandler()      { call_isr(HDL_NVIC_IRQ103_UART7_IRQn, 0); }
void EADC10_IRQHandler()     { call_isr(HDL_NVIC_IRQ104_EADC10_IRQn, 0); }
void EADC11_IRQHandler()     { call_isr(HDL_NVIC_IRQ105_EADC11_IRQn, 0); }
void EADC12_IRQHandler()     { call_isr(HDL_NVIC_IRQ106_EADC12_IRQn, 0); }
void EADC13_IRQHandler()     { call_isr(HDL_NVIC_IRQ107_EADC13_IRQn, 0); }
void SPI8_IRQHandler()       { call_isr(HDL_NVIC_IRQ108_SPI8_IRQn, 0); }
void KS_IRQHandler()         { call_isr(HDL_NVIC_IRQ109_KS_IRQn, 0); }
void GPI_IRQHandler()        { call_isr(HDL_NVIC_IRQ110_GPI_IRQn, 0); }
void SPI9_IRQHandler()       { call_isr(HDL_NVIC_IRQ111_SPI9_IRQn, 0); }
void CANFD00_IRQHandler()    { call_isr(HDL_NVIC_IRQ112_CANFD00_IRQn, 0); }
void CANFD01_IRQHandler()    { call_isr(HDL_NVIC_IRQ113_CANFD01_IRQn, 0); }
void CANFD10_IRQHandler()    { call_isr(HDL_NVIC_IRQ114_CANFD10_IRQn, 0); }
void CANFD11_IRQHandler()    { call_isr(HDL_NVIC_IRQ115_CANFD11_IRQn, 0); }
void EQEI2_IRQHandler()      { call_isr(HDL_NVIC_IRQ116_EQEI2_IRQn, 0); }
void EQEI3_IRQHandler()      { call_isr(HDL_NVIC_IRQ117_EQEI3_IRQn, 0); }
void I2C4_IRQHandler()       { call_isr(HDL_NVIC_IRQ118_I2C4_IRQn, 0); }
void SPI10_IRQHandler()      { call_isr(HDL_NVIC_IRQ119_SPI10_IRQn, 0); }
void CANFD20_IRQHandler()    { call_isr(HDL_NVIC_IRQ120_CANFD20_IRQn, 0); }
void CANFD21_IRQHandler()    { call_isr(HDL_NVIC_IRQ121_CANFD21_IRQn, 0); }
void CANFD30_IRQHandler()    { call_isr(HDL_NVIC_IRQ122_CANFD30_IRQn, 0); }
void CANFD31_IRQHandler()    { call_isr(HDL_NVIC_IRQ123_CANFD31_IRQn, 0); }
void EADC20_IRQHandler()     { call_isr(HDL_NVIC_IRQ124_EADC20_IRQn, 0); }
void EADC21_IRQHandler()     { call_isr(HDL_NVIC_IRQ125_EADC21_IRQn, 0); }
void EADC22_IRQHandler()     { call_isr(HDL_NVIC_IRQ126_EADC22_IRQn, 0); }
void EADC23_IRQHandler()     { call_isr(HDL_NVIC_IRQ127_EADC23_IRQn, 0); }

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    // /*  Unlock Register */
    //   UnlockRegister();

    // #ifndef ENABLE_SPIM_CACHE
    //   SystemEnableSpimCache();
    // #endif

    // #ifndef __NO_SYSTEM_INIT
    // 	SystemInit();
    // #endif

    // //TODO: turn off JTAG if needed;

    // /* Init POR */
    // #if 0
    // 	ldr	r0, =0x40000024
    // 	ldr	r1, =0x00005AA5
    // 	str	r1, [r0]
    // #endif

    // /* Lock register */
    //   LockRegister();
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}
