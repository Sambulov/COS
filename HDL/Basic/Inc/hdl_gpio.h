#ifndef HDL_GPIO_H_
#define HDL_GPIO_H_

#if defined ( NUM46X )
  typedef struct {
    GPIO_T *port;
    const uint32_t pin; 
    const uint32_t mode;   /* GPIO_MODE_INPUT - GPIO_MODE_OUTPUT - GPIO_MODE_OPEN_DRAIN - GPIO_MODE_QUASI */
    const uint32_t pull_mode; /* GPIO_PUSEL_DISABLE - GPIO_PUSEL_PULL_UP - GPIO_PUSEL_PULL_DOWN */
    //const uint32_t int_mode;  /* GPIO_INT_RISING - GPIO_INT_FALLING - GPIO_INT_BOTH_EDGE - GPIO_INT_HIGH - GPIO_INT_LOW */
    const uint32_t slew_mode; /* GPIO_SLEWCTL_NORMAL - GPIO_SLEWCTL_HIGH - GPIO_SLEWCTL_FAST */
  } hdl_gpio_t;
#endif

#if defined ( GD32E23X )
  typedef struct {
    hdl_clock_t *clock;
    const uint32_t periph;   // GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF
  } hdl_gpio_port_t;

  typedef struct {
    const uint32_t af;        // GPIO_AF_0, GPIO_AF_1, ..., GPIO_AF_15.
    const uint32_t pull;      // GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, GPIO_PUPD_PULLDOWN.
    const uint32_t type;      // GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG.
    const uint32_t otype;     // GPIO_OTYPE_PP, GPIO_OTYPE_OD.
    const uint32_t ospeed;    // GPIO_OSPEED_2MHZ, GPIO_OSPEED_25MHZ, GPIO_OSPEED_50MHZ, GPIO_OSPEED_MAX.
  } hdl_gpio_mode_t;

  typedef struct {
    const hdl_gpio_port_t *port;
    const hdl_gpio_mode_t *mode;
    const uint32_t pin;       // GPIO_PIN_0, GPIO_PIN_1, ..., GPIO_PIN_15, GPIO_PIN_ALL.
  } hdl_gpio_t;
#endif

#if defined ( STM32L0XX )
  typedef struct {
    const uint32_t af;        // see stm32l0xx_hal_gpio_ex.h, line 538
    const uint32_t pull;      // GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN
    const uint32_t type;      // GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, GPIO_MODE_AF_OD
    const uint32_t ospeed;    // GPIO_SPEED_FREQ_LOW, GPIO_SPEED_FREQ_MEDIUM, GPIO_SPEED_FREQ_HIGH, GPIO_SPEED_FREQ_VERY_HIGH
  } hdl_gpio_mode_t;

  typedef struct {
    GPIO_TypeDef *port;       // GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH
    const uint16_t pin;             // GPIO_PIN_0, GPIO_PIN_1, ... GPIO_PIN_15, GPIO_PIN_All
    const hdl_gpio_mode_t *mode;
  } hdl_gpio_t;
#endif

typedef enum {
  HDL_GPIO_LOW = 0,
  HDL_GPIO_HIGH = !HDL_GPIO_LOW
} hdl_gpio_state;

_Static_assert(HDL_GPIO_LOW == !HDL_GPIO_HIGH, "Expression (HDL_GPIO_LOW == !HDL_GPIO_HIGH) must be always true");

hdl_init_state_t hdl_gpio(void *desc, const uint8_t enable);
hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio);
void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state);
void hdl_gpio_toggle(const hdl_gpio_t *gpio);

#endif // HDL_GPIO_H_
