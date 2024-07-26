#include "hdl_portable.h"

static volatile uint32_t ticks = 0;

void SysTick_Handler() {
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
  ticks++;
}

// void hdl_gpio_init(const hdl_gpio_t *gpio) {
//   switch((uint32_t)gpio->port) {
//     case (uint32_t)GPIOA:
//       __GPIOA_CLK_ENABLE();
//     break;
//     case (uint32_t)GPIOB:
//       __GPIOB_CLK_ENABLE();
//     break;
//     case (uint32_t)GPIOC:
//       __GPIOC_CLK_ENABLE();
//     break;
//     case (uint32_t)GPIOD:
//       __GPIOD_CLK_ENABLE();
//     break;
//     case (uint32_t)GPIOE:
//       __GPIOE_CLK_ENABLE();
//     break;
//     case (uint32_t)GPIOH:
//       __GPIOH_CLK_ENABLE();
//     break;
//     default:
//     return;
//   }
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	GPIO_InitStructure.Pin = gpio->pin;
// 	GPIO_InitStructure.Mode = gpio->mode->type;
// 	GPIO_InitStructure.Speed = gpio->mode->ospeed;
// 	GPIO_InitStructure.Pull = gpio->mode->pull;
//   GPIO_InitStructure.Alternate = gpio->mode->af;
// 	HAL_GPIO_Init(gpio->port, &GPIO_InitStructure);
// }

// hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio) {
//   return (HAL_GPIO_ReadPin(gpio->port, gpio->pin)==GPIO_PIN_RESET)? HDL_GPIO_LOW: HDL_GPIO_HIGH;
// }

// void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state) {
//   HAL_GPIO_WritePin(gpio->port, gpio->pin, (state==HDL_GPIO_LOW)? GPIO_PIN_RESET: GPIO_PIN_SET);
// }

// void hdl_gpio_toggle(const hdl_gpio_t *gpio) {
//   HAL_GPIO_TogglePin(gpio->port, gpio->pin);
// }

uint32_t hdl_millis() {
  return ticks;
}

// void hdl_system_init(const hdl_sys_t *desc) {
// 	HAL_Init();
// }
