/*
  Module Init Graph (MIG)
*/

#include "bldl.h"

#if defined ( ATB_3500 )
#include "atb_3500_page_transfer.h"

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

#define HDL_HXTAL_CLOCK                   20000000
#define HDL_PLL_MUL_CLOCK                 mod_clock_hxtal               /* Can be clocked by: mod_clock_irc16m, mod_clock_hxtal */
#define HDL_SYS_CLOCK                     mod_clock_pll_p_prescaler     /* Can be clocked by: mod_clock_pll_p_prescaler, mod_clock_hxtal, mod_clock_irc16m */
#define HDL_PLL_VCO_PRESCALER             4                            /* Can be 2, 3 .. 63 */
#define HDL_PLL_N_MULTIPLY                96                            /* Note that, don`t excceed 500MHz; Can be 64, 65 .. 500 */ 
#define HDL_PLL_P_PRESCALER               2                             /* Note that, don`t excceed 240MHz; Can be 2, 4, 6, 8 */
#define HDL_PLL_Q_PRESCALER               10                             /* Note that, don`t excceed 48MHz; Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                             /* Note that, don`t excceed 200MHz; Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                4                             /* Note that, don`t excceed 60MHz; Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                2                             /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
  /* TODO: ... */
};
/***********************************************************
 *                          IRQ
***********************************************************/
hdl_nvic_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};
hdl_nvic_interrupt_t mod_irq_timer0 = {
  .irq_type = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
  .priority = 0,
  .priority_group = 1,
};
hdl_nvic_interrupt_t mod_irq_timer1 = {
  .irq_type = HDL_NVIC_IRQ28_TIMER1,
  .priority = 0,
  .priority_group = 2,
};

hdl_nvic_interrupt_t mod_irq_exti_4 = {
  .irq_type = HDL_NVIC_IRQ10_EXTI4,
  .priority = 0,
  .priority_group = 1,
};
hdl_nvic_interrupt_t mod_irq_spi_3 = {
  .irq_type = HDL_NVIC_IRQ84_SPI3,
  .priority = 0,
  .priority_group = 0,
};
hdl_nvic_interrupt_t mod_irq_adc = {
  .irq_type = HDL_NVIC_IRQ18_ADC,
  .priority = 0,
  .priority_group = 0,
};

/***********************************************************
 *                          EXTI
***********************************************************/
hdl_nvic_exti_t mod_nvic_exti_line_4 = {
  .line = HDL_EXTI_LINE_4,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};
/***********************************************************
 *                          NVIC
***********************************************************/
hdl_nvic_t mod_nvic = {
  .module.init = &hdl_nvic,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0, &mod_irq_timer1, &mod_irq_exti_4, &mod_irq_spi_3, &mod_irq_adc),
  .exti_lines = hdl_exti_lines(&mod_nvic_exti_line_4)
};
/***********************************************************
 *                          CLOCK
***********************************************************/
hdl_clock_t mod_clock_irc16m = {
  .module.init = &hdl_clock_irc16m,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .freq = 16000000,
  .div = 1
};
hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock_hxtal,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .freq = HDL_HXTAL_CLOCK,
  .div = 1
};
hdl_clock_prescaler_t mod_pll_sel = {
  .module.init = &hdl_clock_selector_pll,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = 1,
};
hdl_clock_prescaler_t mod_clock_pll_vco_prescaler = {
  .module.init = &hdl_clock_selector_pll_vco,
  .module.dependencies = hdl_module_dependencies(&mod_pll_sel.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_VCO_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_pll_n_multiply = {
  .module.init = &hdl_clock_pll_n,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco_prescaler.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_N_MULTIPLY,
};
hdl_clock_prescaler_t mod_clock_pll_p_prescaler = {
  .module.init = &hdl_clock_pll_p,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_P_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_pll_q_prescaler = {
  .module.init = &hdl_clock_pll_q,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_Q_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_sys_clock = {
  .module.init = &hdl_clock_system,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_Q_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_ahb = {
  .module.init = &hdl_clock_ahb,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys_clock.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_AHB_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_apb1 = {
  .module.init = &hdl_clock_apb1,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_APB1_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_apb2 = {
  .module.init = &hdl_clock_apb2,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_APB2_PRESCALER,
};
/***********************************************************
 *                          COUNTER
***********************************************************/
hdl_clock_counter_t mod_systick_counter = {
  .module.init = &hdl_clock_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),  // add ahb
  .module.reg = (void *)SysTick,
  .diction = HDL_DOWN_COUNTER,
  .counter_reload = 240000 - 1,
};
hdl_clock_counter_t mod_timer0_counter = {
  .module.init = &hdl_clock_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module), // add ahb
  .module.reg = (void *)TIMER0,
  .diction = HDL_DOWN_COUNTER,
  .counter_reload = 16000 - 1,
};
hdl_clock_counter_t mod_timer1_counter = {
.module.init = &hdl_clock_counter,
.module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module), // add ahb
.module.reg = (void *)TIMER1,
.diction = HDL_DOWN_COUNTER,
.counter_reload = 16000 - 1,
};
/***********************************************************
 *                          TIMER
 ***********************************************************/
hdl_timer_t mod_systick_timer_ms = {
  .module.init = hdl_timer,
  .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .reload_iterrupt = HDL_NVIC_EXCEPTION_SysTick,
  .val = 0
};
hdl_timer_t mod_timer0_ms = {
  .module.init = hdl_timer,
  .module.dependencies = hdl_module_dependencies(&mod_timer0_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .reload_iterrupt = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
  .val = 0
};
hdl_timer_t mod_timer1_ms = {
  .module.init = hdl_timer,
  .module.dependencies = hdl_module_dependencies(&mod_timer1_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .reload_iterrupt = HDL_NVIC_IRQ28_TIMER1,
  .val = 0
};
hdl_timer_event_t mod_watchdog_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_systick_timer_ms.module),
  .module.init = &hdl_timer_event,
  .delay = 3000,
};


/***********************************************************
 *                          GPIO PORT
***********************************************************/
hdl_gpio_port_t hdl_gpio_port_a = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOA,
};
hdl_gpio_port_t hdl_gpio_port_b = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOB,
};
hdl_gpio_port_t hdl_gpio_port_c = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOC,
};
hdl_gpio_port_t hdl_gpio_port_d = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOD,
};
hdl_gpio_port_t hdl_gpio_port_e = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOE,
};
hdl_gpio_port_t hdl_gpio_port_f = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOF,
};
hdl_gpio_port_t hdl_gpio_port_g = {
  .init = &hdl_gpio_port,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .reg = (void *)GPIOG,
};
/***********************************************************
 *                          GPIO
***********************************************************/
/**************************************
 *            GPIO MODE
***************************************/
hdl_gpio_mode_t hdl_gpio_mode_input_floating = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ
};
hdl_gpio_mode_t hdl_gpio_mode_output_no_pull = {
  .type = GPIO_MODE_OUTPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
};
hdl_gpio_mode_t hdl_gpio_mode_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
};
hdl_gpio_mode_t hdl_gpio_spi_mode = {
    .type = GPIO_MODE_AF,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_MAX,
    .af = GPIO_AF_5,
    .otype = GPIO_OTYPE_PP,
};
/***********************************************************
 *                          GPIO PIN
***********************************************************/
/***********************************************************
 *                          POWER ADJUST
***********************************************************/
hdl_gpio_pin_t mod_do_5v_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_2,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_24v_poe_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_3,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_24v_poe_power_fault = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_HIGH,
};
hdl_gpio_pin_t mod_di_24v_poe_power_good = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_5,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *                          RELAY
***********************************************************/
hdl_gpio_pin_t mod_do_relay1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_relay2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g),
  .module.reg = (void *)GPIO_PIN_1,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *                        Other
***********************************************************/
hdl_gpio_pin_t mod_do_pci_switch = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_9,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_lte_reset = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_6,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
hdl_gpio_pin_t mod_do_smarc_reset_in = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_smarc_reset_out = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_8,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_HIGH,
};
hdl_gpio_pin_t mod_do_smarc_boot_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_13,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_smarc_boot_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_14,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_smarc_boot_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_15,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_smarc_power_btn = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_12,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_smarc_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_10,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_HIGH,
};
hdl_gpio_pin_t mod_do_smarc_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_11,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_HIGH,
};

/***********************************************************
 *                        X1 Connector
***********************************************************/
hdl_gpio_pin_t mod_di_module_address_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .module.reg = (void *)GPIO_PIN_8,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_module_address_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .module.reg = (void *)GPIO_PIN_9,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_module_address_3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .module.reg = (void *)GPIO_PIN_10,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_module_address_4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .module.reg = (void *)GPIO_PIN_11,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_module_address_5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d),
  .module.reg = (void *)GPIO_PIN_12,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_di_external_periph_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_10,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_HIGH,
};
hdl_gpio_pin_t mod_di_external_periph_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_11,
  .mode = &hdl_gpio_mode_input_floating,
  .inactive_default = HDL_GPIO_HIGH,
};
hdl_gpio_pin_t mod_do_external_output_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_3,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_external_output_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *                          LED
***********************************************************/
/*
      * | * | *  
LEFT  * | * | *  
      * | * | *  
         BOT
LED_2_0 | LED_2_1 | LED_2_2
LED_1_0 | LED_1_1 | LED_1_2
LED_0_0 | LED_0_1 | LED_0_2
*/
hdl_gpio_pin_t mod_do_led_0_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
    .module.reg = (void *)GPIO_PIN_3,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_0_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_6,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_0_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_1_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_1_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_7,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_1_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_1,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_2_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_5,
  .mode = &hdl_gpio_mode_output_no_pull,
};
hdl_gpio_pin_t mod_do_led_2_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_8,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_do_led_2_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_2,
  .mode = &hdl_gpio_mode_output_no_pull,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *                    ANALOG PIN
***********************************************************/
hdl_gpio_pin_t mod_adc_pin_24v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_adc_pin_24v_poe = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_1,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_adc_pin_5v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_2,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_adc_pin_3v3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_3,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_adc_pin_2v5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
hdl_gpio_pin_t mod_adc_pin_1v8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .module.reg = (void *)GPIO_PIN_5,
  .mode = &hdl_gpio_mode_analog,
  .inactive_default = HDL_GPIO_LOW,
};
/***********************************************************
 *                          DMA
***********************************************************/
hdl_dma_t mod_dma = {
  .module.init = &hdl_dma,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void*)DMA1,
};
hdl_dma_channel_t mod_adc_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH0,
  .direction = HDL_DMA_DIRECTION_P2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT,
  .mode = HDL_DMA_MODE_CIRCULAR,
  .channel_periphery = HDL_DMA_CHANNEL_PERIPHERY_0,
  .priority = HDL_DMA_PRIORITY_LOW
};
hdl_dma_channel_t mod_dma_ch_spi_3_rx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH3,
  .direction = HDL_DMA_DIRECTION_P2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .channel_periphery = HDL_DMA_CHANNEL_PERIPHERY_5,
  .priority = HDL_DMA_PRIORITY_MEDIUM
};

  hdl_dma_channel_t mod_dma_ch_spi_3_tx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH4,
  .direction = HDL_DMA_DIRECTION_M2P,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .channel_periphery = HDL_DMA_CHANNEL_PERIPHERY_5,
  .priority = HDL_DMA_PRIORITY_ULTRA_HIGH,
};

  hdl_dma_channel_t mod_dma_ch_spi_3_m2m = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .direction = HDL_DMA_DIRECTION_M2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_ON,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .channel_periphery = HDL_DMA_CHANNEL_PERIPHERY_0,
  .priority = HDL_DMA_PRIORITY_ULTRA_HIGH
};
/**************************************************************
 *                        ADC
 *************************************************************/
hdl_adc_source_t mod_adc_source_0_adc_24v = {
  .channel = HDL_ADC_CHANNEL_0,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_1_adc_24v_poe = {
  .channel = HDL_ADC_CHANNEL_1,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_2_adc_5v = {
  .channel = HDL_ADC_CHANNEL_2,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_3_adc_3v3 = {
  .channel = HDL_ADC_CHANNEL_3,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_4_adc_2v5 = {
  .channel = HDL_ADC_CHANNEL_4,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_source_t mod_adc_source_5_adc_1v8 = {
  .channel = HDL_ADC_CHANNEL_5,
  .sample_time = HDL_ADC_SAMPLETIME_3,
};
hdl_adc_t mod_adc = {
  .module.init = &hdl_adc,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module, &mod_systick_timer_ms.module, &mod_adc_dma_ch.module, &mod_nvic.module),
  .module.reg = (void *)ADC0,
  .adc_iterrupt = HDL_NVIC_IRQ18_ADC,
  .resolution = HDL_ADC_RESOLUTION_12BIT,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_0_adc_24v, &mod_adc_source_1_adc_24v_poe, &mod_adc_source_2_adc_5v,
                          &mod_adc_source_3_adc_3v3, &mod_adc_source_4_adc_2v5, &mod_adc_source_5_adc_1v8),
};
/**************************************************************
 *                        SPI
 *************************************************************/
  hdl_gpio_pin_t mod_spi_3_mosi = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
    .module.reg = (void *)GPIO_PIN_6,
    .mode = &hdl_gpio_spi_mode,
};

hdl_gpio_pin_t mod_spi_3_miso = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
    .module.reg = (void *)GPIO_PIN_5,
    .mode = &hdl_gpio_spi_mode,
};

hdl_gpio_pin_t mod_spi_3_sck = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
    .module.reg = (void *)GPIO_PIN_2,
    .mode = &hdl_gpio_spi_mode,
};

hdl_gpio_pin_t mod_spi_3_cs = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
    .module.reg = (void *)GPIO_PIN_4,
    .mode = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_HIGH,
};


hdl_spi_server_config_t hdl_spi_3_slave_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE
};
uint8_t spi_dma_rx_0_buf[20] = {};
uint8_t spi_dma_rx_1_buf[20] = {};
uint8_t spi_dma_tx_0_buf[20] = {0x3A, 0x55, 0x5A, 0xA5};
uint8_t spi_dma_tx_1_buf[20] = {0x3A, 0x55, 0x5A, 0xA5};

hdl_double_buffer_t spi_rx_buffer = {
    .data[0] = spi_dma_rx_0_buf,
    .data[1] = spi_dma_rx_1_buf,
    .size = sizeof(spi_dma_rx_0_buf),
};
hdl_double_buffer_t spi_tx_buffer = {
    .data[0] = spi_dma_tx_0_buf,
    .data[1] = spi_dma_tx_1_buf,
    .size = sizeof(spi_dma_tx_0_buf),
};

hdl_spi_mem_server_t mod_spi_3 = {
  .module.reg = (void *)SPI3,
  .module.dependencies = hdl_module_dependencies(&mod_spi_3_mosi.module, &mod_spi_3_miso.module, &mod_spi_3_sck.module,
                                                  &mod_spi_3_cs.module, &mod_clock_apb2.module, &mod_nvic.module, 
                                                  &mod_dma_ch_spi_3_rx.module, &mod_dma_ch_spi_3_tx.module, &mod_dma_ch_spi_3_m2m.module,
                                                  &mod_timer0_ms.module),
  .module.init = &hdl_spi_mem_server,
  .config = &hdl_spi_3_slave_config,
  .spi_iterrupt = HDL_NVIC_IRQ84_SPI3,
  .nss_iterrupt = HDL_NVIC_IRQ10_EXTI4,
  .rx_mem = &spi_rx_buffer,
  .tx_mem = &spi_tx_buffer,
};
/***********************************************************
 *                     UNIVERSAL PORT
***********************************************************/
/***********************************
 *     UNIVERSAL MODE
***********************************/
hdl_plc_port_config_t mod_atb3500_port_default_cnf_led = {
  .desc = (PLC_IO_PORT_DO),
  .default_output = HDL_GPIO_LOW,
  .default_option = 0
};
hdl_plc_port_config_t mod_atb3500_port_default_cnf_do = {
  .desc = (PLC_IO_PORT_DO),
  .default_output = HDL_GPIO_LOW,
  .default_option = 0
};
hdl_plc_port_config_t mod_atb3500_port_default_cnf_di = {
  .desc = PLC_IO_PORT_HW_INPUT | PLC_IO_PORT_SW_DISCRETE,
  .default_output = HDL_GPIO_LOW,
  .default_option = 0
};
hdl_plc_port_config_t mod_atb3500_port_default_cnf_ain = {
  .desc = (PLC_IO_PORT_HW_ANALOG | PLC_IO_PORT_SW_RAW_ADC),
  .default_output = HDL_GPIO_LOW,
  .default_option = 0
};
/***********************************************************
 *                   UNIVERSAL DIGITAL OUTPUT
***********************************************************/
bldl_atb3500_do_port_t mod_atb3500_do_external_output_1 = {
  .module.init = &bldl_atb3500_do_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_do_external_output_1.module),
  .default_config = &mod_atb3500_port_default_cnf_do,
  .output = HDL_GPIO_LOW,
};
bldl_atb3500_do_port_t mod_atb3500_do_external_output_2 = {
  .module.init = &bldl_atb3500_do_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_do_external_output_2.module),
  .default_config = &mod_atb3500_port_default_cnf_do,
  .output = HDL_GPIO_LOW,
};
bldl_atb3500_do_port_t mod_atb3500_do_pci_switch = {
  .module.init = &bldl_atb3500_do_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_do_pci_switch.module),
  .default_config = &mod_atb3500_port_default_cnf_do,
  .output = HDL_GPIO_LOW,
};
/***********************************************************
 *                     UNIVERSAL DIGITAL INPUT
***********************************************************/
bldl_atb3500_di_port_t mod_atb3500_di_external_irq_1 = {
  .module.init = &bldl_atb3500_di_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_di_external_periph_irq_1.module),
  .default_config = &mod_atb3500_port_default_cnf_di,
  .output = HDL_GPIO_LOW,
};
bldl_atb3500_di_port_t mod_atb3500_di_external_irq_2 = {
  .module.init = &bldl_atb3500_di_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_di_external_periph_irq_2.module),
  .default_config = &mod_atb3500_port_default_cnf_di,
  .output = HDL_GPIO_LOW,
};
/***********************************************************
 *                     UNIVERSAL LED
***********************************************************/
bldl_atb3500_led_port_t mod_atb3500_led_0_0 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_0_0.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_0_1 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_0_1.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_0_2 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_0_2.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_1_0 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_1_0.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_1_1 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_1_1.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_1_2 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_1_2.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_2_0 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_2_0.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_2_1 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_2_1.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
bldl_atb3500_led_port_t mod_atb3500_led_2_2 = {
  .module.init = &bldl_atb3500_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_do_led_2_2.module),
  .default_config = &mod_atb3500_port_default_cnf_led
};
/***********************************************************
 *                  UNIVERSAL ANALOG INPUT
***********************************************************/
bldl_atb3500_ain_port_t mod_atb3500_ai_ch0 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_0_adc_24v,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_ain_port_t mod_atb3500_ai_ch1 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_1_adc_24v_poe,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_ain_port_t mod_atb3500_ai_ch2 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_2_adc_5v,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_ain_port_t mod_atb3500_ai_ch3 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_3_adc_3v3,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_ain_port_t mod_atb3500_ai_ch4 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_4_adc_2v5,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_ain_port_t mod_atb3500_ai_ch5 = {
  .module.init = &bldl_atb3500_ain_port,
  .module.reg = (void *)&mod_adc_source_5_adc_1v8,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module),
  .default_config = &mod_atb3500_port_default_cnf_ain
};
bldl_atb3500_port_expander_t mod_atb3500_port_expander = {
    .init = &bldl_atb3500_port_expander,
    .dependencies = hdl_module_dependencies(&mod_atb3500_do_external_output_1.module, &mod_atb3500_do_external_output_2.module,
        &mod_atb3500_do_pci_switch.module, &mod_atb3500_di_external_irq_1.module, &mod_atb3500_di_external_irq_1.module,
        &mod_atb3500_led_0_0.module, &mod_atb3500_led_0_1.module, &mod_atb3500_led_0_2.module, &mod_atb3500_led_1_0.module, 
        &mod_atb3500_led_1_1.module, &mod_atb3500_led_1_2.module, &mod_atb3500_led_2_0.module, &mod_atb3500_led_2_1.module, 
        &mod_atb3500_led_2_2.module, &mod_atb3500_ai_ch0.module, &mod_atb3500_ai_ch1.module, &mod_atb3500_ai_ch2.module, 
        &mod_atb3500_ai_ch3.module, &mod_atb3500_ai_ch4.module, &mod_atb3500_ai_ch5.module)};
#endif