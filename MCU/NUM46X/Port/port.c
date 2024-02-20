#include "hdl_portable.h"
#include "gpio.h"

static volatile uint32_t ticks = 0;

void SysTick_Handler() {
  ticks++;
}

void hdl_gpio_init(const hdl_gpio_t *gpio) {
  switch((uint32_t)gpio->port) {
    case (uint32_t)PA:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPACKEN_Msk;
    break;
    case (uint32_t)PB:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPBCKEN_Msk;
    break;
    case (uint32_t)PC:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPCCKEN_Msk;
    break;
    case (uint32_t)PD:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPDCKEN_Msk;
    break;
    case (uint32_t)PE:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPECKEN_Msk;
    break;
    case (uint32_t)PF:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPFCKEN_Msk;
    break;
    case (uint32_t)PG:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPGCKEN_Msk;
    break;
    case (uint32_t)PH:
      CLK->AHBCLK0 |= CLK_AHBCLK0_GPHCKEN_Msk;
    break;
    // case (uint32_t)PI:
    //   CLK->AHBCLK0 |= CLK_AHBCLK0_GPICKEN_Msk;
    // break;
    // case (uint32_t)PJ:
    //   CLK->AHBCLK0 |= CLK_AHBCLK0_GPJCKEN_Msk;
    // break;
    default:
    return;
  }
  GPIO_SetMode(gpio->port, gpio->pin, gpio->mode);
  GPIO_SetPullCtl(gpio->port, gpio->pin, gpio->pull_mode);
  //GPIO_EnableInt(gpio->port, gpio->u32Pin, gpio->u32IntMode);
  //GPIO_SetSlewCtl(gpio->port, gpio->u32Pin, gpio->u32SlewMode);
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio) {
  return (gpio->port->PIN & gpio->pin)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state) {
  if(state == HDL_GPIO_LOW) {
    gpio->port->DOUT &= ~(gpio->pin);
  }
  else {
    gpio->port->DOUT |= gpio->pin;
  }
}

void hdl_gpio_toggle(const hdl_gpio_t *gpio) {
    gpio->port->DOUT ^= gpio->pin;
}

uint32_t hdl_millis() {
  return ticks;
}

static void _UART0Init(void) {
  /* Set multi-function pins for UART0 RXD and TXD */
  SET_UART0_RXD_PB12();
  SET_UART0_TXD_PB13();
  /* Init UART */
  UART0->LINE = UART_PARITY_NONE | UART_STOP_BIT_1 | UART_WORD_LEN_8;
  UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
}

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

void hdl_system_init(const hdl_sys_t *desc) {
  SYS_UnlockReg();
  _vClockInit();

  /* Enable UART0 module clock */
  // CLK_EnableModuleClock(UART0_MODULE);
  /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
  // CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  //_UART0Init();
}
