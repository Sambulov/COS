#include "hdl_portable.h"
#include "gpio.h"

static volatile uint32_t ticks = 0;

void SysTick_Handler() {
  ticks++;
}

// uint32_t hdl_millis() {
//   return ticks;
// }

// static void _UART0Init(void) {
//   /* Set multi-function pins for UART0 RXD and TXD */
//   SET_UART0_RXD_PB12();
//   SET_UART0_TXD_PB13();
//   /* Init UART */
//   UART0->LINE = UART_PARITY_NONE | UART_STOP_BIT_1 | UART_WORD_LEN_8;
//   UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
// }

static void _vClockInit(void) {
  /* Enable HIRC clock */
  CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
  /* Wait for HIRC clock ready */
  CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  /* Set core clock to 192MHz */
  CLK_SetCoreClock(192000000);

  SysTick->LOAD = CyclesPerUs * 1000;
  SysTick->VAL  = 0x0UL;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  NVIC_SetPriority(SysTick_IRQn, 0);
  NVIC_EnableIRQ(SysTick_IRQn);
}

//#define LED_INIT()  (PH->MODE = ((PH->MODE &(~(0x3ful << 4*2))) | (0x15ul << 4 *2)))
//#define LED_RED     PH4
//#define LED_YELLOW  PH5
//#define LED_GREEN   PH6

void hdl_system_init() {
  SYS_UnlockReg();
  _vClockInit();

  /* Enable UART0 module clock */
  // CLK_EnableModuleClock(UART0_MODULE);
  /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
  // CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  //_UART0Init();
}
