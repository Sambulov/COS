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
    const uint32_t af;        // GPIO_AF_0, GPIO_AF_1,  GPIO_AF_2,  GPIO_AF_3,  GPIO_AF_4,  GPIO_AF_5, GPIO_AF_6,  GPIO_AF_7,  GPIO_AF_8,  GPIO_AF_9,  GPIO_AF_10, GPIO_AF_11, GPIO_AF_12, GPIO_AF_13, GPIO_AF_14, GPIO_AF_15.
    const uint32_t pull;      // GPIO_PUPD_NONE,    GPIO_PUPD_PULLUP,    GPIO_PUPD_PULLDOWN.
    const uint32_t type;      // GPIO_MODE_INPUT,  GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG.
    const uint32_t otype;     // GPIO_OTYPE_PP, GPIO_OTYPE_OD.
    const uint32_t ospeed;    // GPIO_OSPEED_2MHZ,  GPIO_OSPEED_25MHZ, GPIO_OSPEED_50MHZ, GPIO_OSPEED_MAX.
  } hdl_gpio_mode_t;

  typedef struct {
    const rcu_periph_enum rcu;       // RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD, RCU_GPIOE, RCU_GPIOF
    const uint32_t port;             // GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF
    const uint32_t pin;              // GPIO_PIN_0,  GPIO_PIN_1,  GPIO_PIN_2,  GPIO_PIN_3,  GPIO_PIN_4,  GPIO_PIN_5,  GPIO_PIN_6,  GPIO_PIN_7,  GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, GPIO_PIN_ALL.
    const hdl_gpio_mode_t *mode;
  } hdl_gpio_t;
#endif

typedef enum {
  HDL_GPIO_LOW = 0,
  HDL_GPIO_HIGH = 1
} hdl_gpio_state;

void hdl_gpio_init(const hdl_gpio_t *gpio);
hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio);
void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state);
void hdl_gpio_toggle(const hdl_gpio_t *gpio);

#endif // HDL_GPIO_H_
