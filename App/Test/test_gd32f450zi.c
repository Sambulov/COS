#include "app.h"
#include "CodeLib.h"

#if defined( GD32F450 )
void test() {
    SystemInit();
    // rcu_periph_clock_enable(RCU_GPIOF);
    // gpio_af_set(GPIOF, GPIO_AF_0, GPIO_PIN_13);
    // gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    // gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    // gpio_bit_write(GPIOF, GPIO_PIN_13, 0);
    // gpio_bit_write(GPIOF, GPIO_PIN_13, 1);
    while (1)
    {
      __NOP();
    }
    // rcu_periph_clock_enable(RCU_GPIOC);
    //  gpio_mode_set(RCU_GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_3);
    //  gpio_output_options_set(RCU_GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_3);
    //  gpio_mode_set(RCU_GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_6);
    //  gpio_output_options_set(RCU_GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_6);
     
    //  gpio_bit_set(RCU_GPIOC, GPIO_PIN_3);
    //  gpio_bit_set(RCU_GPIOC, GPIO_PIN_6);
}
#endif