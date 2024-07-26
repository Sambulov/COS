/*
  Module Init Graph (MIG)
*/

#ifdef GD32F450

#include "app.h"

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
#define HDL_APB2_PRESCALER                4                             /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */

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

hdl_nvic_interrupt_t mod_irq_i2c_0_ev = {
  .irq_type = HDL_NVIC_IRQ31_I2C0_EV,
  .priority = 0,
  .priority_group = 1,
};
hdl_nvic_interrupt_t mod_irq_i2c_0_err = {
  .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
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
 *                          NVIC
***********************************************************/
hdl_nvic_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0, &mod_irq_timer1, &mod_irq_exti_4, &mod_irq_spi_3, &mod_irq_adc,
                              &mod_irq_i2c_0_ev, &mod_irq_i2c_0_err),
};

/***********************************************************
 *                          EXTI
***********************************************************/

hdl_exti_t mod_nvic_exti_line_4 = {
  .line = HDL_EXTI_LINE_4,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING,
};

hdl_exti_controller_t mod_exti = {
  .module.init = &hdl_exti,
  .module.reg = (void *)EXTI,
  .module.dependencies = hdl_module_dependencies(&mod_nvic.module),
  .extis = hdl_extis(&mod_nvic_exti_line_4)
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
};


/***********************************************************
 *                          GPIO PORT
***********************************************************/
hdl_gpio_port_hw_config_t mod_gpio_port_config_a = {
  .rcu = RCU_GPIOA
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_b = {
  .rcu = RCU_GPIOB
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_c = {
  .rcu = RCU_GPIOC
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_d = {
  .rcu = RCU_GPIOD
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_e = {
  .rcu = RCU_GPIOE
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_f = {
  .rcu = RCU_GPIOF
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_g = {
  .rcu = RCU_GPIOG
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_h = {
  .rcu = RCU_GPIOH
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_i = {
  .rcu = RCU_GPIOI
};

hdl_gpio_port_t hdl_gpio_port_a = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOA,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_a)
};
hdl_gpio_port_t hdl_gpio_port_b = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOB,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_b)
};
hdl_gpio_port_t hdl_gpio_port_c = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOC,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_c)
};
hdl_gpio_port_t hdl_gpio_port_d = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOD,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_d)
};
hdl_gpio_port_t hdl_gpio_port_e = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOE,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_e)
};
hdl_gpio_port_t hdl_gpio_port_f = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOF,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_f)
};
hdl_gpio_port_t hdl_gpio_port_g = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOG,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_g)
};
/***********************************************************
 *                          GPIO
***********************************************************/
/**************************************
 *            GPIO MODE
***************************************/
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_input_floating = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ
};
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_output_no_pull = {
  .type = GPIO_MODE_OUTPUT,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
};
const hdl_gpio_pin_hw_config_t hdl_gpio_mode_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
};
const hdl_gpio_pin_hw_config_t hdl_gpio_spi_mode = {
  .type = GPIO_MODE_AF,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_MAX,
  .af = GPIO_AF_5,
  .otype = GPIO_OTYPE_PP,
};
const hdl_gpio_pin_hw_config_t mod_gpio_i2c_0_mode = {
  .af = GPIO_AF_4,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};
/***********************************************************
 *                          GPIO PIN
***********************************************************/
hdl_gpio_pin_t mod_gpio_i2c0_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_i2c_0_mode)
};

hdl_gpio_pin_t mod_gpio_i2c0_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_i2c_0_mode)
};
/***********************************************************
 *                          POWER ADJUST
***********************************************************/
hdl_gpio_pin_t mod_do_5v_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_24v_poe_power_on = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_di_24v_poe_power_fault = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_24v_poe_power_good = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
/***********************************************************
 *                          RELAY
***********************************************************/
hdl_gpio_pin_t mod_do_relay1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_relay2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_g.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                        Other
***********************************************************/
hdl_gpio_pin_t mod_do_pci_switch = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_lte_reset = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
hdl_gpio_pin_t mod_do_smarc_reset_in = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_di_smarc_reset_out = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_do_smarc_boot_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_boot_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_boot_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_power_btn = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_smarc_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_output_no_pull)
};

/***********************************************************
 *                        X1 Connector
***********************************************************/
hdl_gpio_pin_t mod_di_module_address_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_module_address_5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_d.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_external_periph_irq_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_di_external_periph_irq_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_mode_input_floating)
};
hdl_gpio_pin_t mod_do_external_output_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_external_output_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                          LED
***********************************************************/
/*
      * | * | *  
LEFT  * | * | *  
      * | * | *  
         BOT

      LED_0_2 | LED_1_2 | LED_2_2
LEFT  LED_0_1 | LED_1_1 | LED_2_1
      LED_0_0 | LED_1_0 | LED_2_0
                  BOT
*/
hdl_gpio_pin_t mod_do_led_0_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
    .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_0_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_0_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_1_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
hdl_gpio_pin_t mod_do_led_2_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_output_no_pull)
};
/***********************************************************
 *                    ANALOG PIN
***********************************************************/
hdl_gpio_pin_t mod_adc_pin_24v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_24v_poe = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_5v = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_3v3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_2v5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
};
hdl_gpio_pin_t mod_adc_pin_1v8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_mode_analog)
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
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_6,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_miso = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_sck = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_2,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_3_cs = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_spi_mode)
};


const hdl_spi_server_config_t hdl_spi_slave_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE
};

hdl_spi_server_dma_t mod_spi3_server_dma = {
  .module.reg = (void *)SPI3,
  .module.dependencies = hdl_module_dependencies(&mod_spi_3_mosi.module, &mod_spi_3_miso.module, &mod_spi_3_sck.module,
                                                  &mod_spi_3_cs.module, &mod_clock_apb2.module, &mod_nvic.module, 
                                                  &mod_dma_ch_spi_3_rx.module, &mod_dma_ch_spi_3_tx.module, &mod_exti.module),
  .module.init = &hdl_spi_server_dma,
  .config = &hdl_spi_slave_config,
  .spi_iterrupt = HDL_NVIC_IRQ84_SPI3,
  .nss_iterrupt = HDL_NVIC_IRQ10_EXTI4,
};

const hdl_i2c_config_t mod_i2c_0_config = {
  .addr0 = 0x23,
  .addr1 = 0,
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = HDL_NVIC_IRQ32_I2C0_ER,
  .ev_interrupt = HDL_NVIC_IRQ31_I2C0_EV,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C0
};

hdl_i2c_t mod_i2c0 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_i2c0_scl.module, &mod_gpio_i2c0_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_systick_timer_ms.module),
  .module.reg = (void *)I2C0,
  .config = &mod_i2c_0_config,
};

/**************************************************************
 *                        BLDL
 *************************************************************/

atb3500_power_rail_t rail_24v = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_24v.module,
        &hdl_null_module,
        &hdl_null_module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_24V,
    .adc_src = &mod_adc_source_0_adc_24v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_24vpoe = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_24v_poe.module,
        &rail_24v.module,
        &mod_do_24v_poe_power_on.module, 
        &mod_di_24v_poe_power_fault.module, 
        &mod_di_24v_poe_power_good.module),
    .adc_scale = POWER_RAIL_ADC_SCALE_24V,
    .adc_src = &mod_adc_source_0_adc_24v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_5v = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_5v.module,
        &rail_24v.module,
        &mod_do_5v_power_on.module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_5V,
    .adc_src = &mod_adc_source_2_adc_5v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_5V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_5V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_5V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_5V,
};

atb3500_power_rail_t rail_3v3 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_3v3.module,
        &rail_5v.module,
        &hdl_null_module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_3V3,
    .adc_src = &mod_adc_source_3_adc_3v3,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_3V3,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_3V3,
    .raise_delay = POWER_RAIL_RAISE_DELAY_3V3,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_3V3,
};

atb3500_power_rail_t rail_2v5 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_2v5.module,
        &rail_5v.module,
        &hdl_null_module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_2V5,
    .adc_src = &mod_adc_source_4_adc_2v5,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_2V5,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_2V5,
    .raise_delay = POWER_RAIL_RAISE_DELAY_2V5,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_2V5,
};

atb3500_power_rail_t rail_1v8 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &mod_adc_pin_1v8.module,
        &rail_5v.module,
        &hdl_null_module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_1V8,
    .adc_src = &mod_adc_source_5_adc_1v8,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_1V8,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_1V8,
    .raise_delay = POWER_RAIL_RAISE_DELAY_1V8,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_1V8,
};

hdl_smarc_carrier_t mod_smarc = {
    .module.init = &hdl_smarc_carrier,
    .module.dependencies = hdl_module_dependencies(
        &hdl_null_module /* power good */,
        &hdl_null_module /* carrier_power_on */,
        &hdl_null_module /* carrier_stand_by */,
        &mod_do_smarc_reset_in.module, 
        &mod_di_smarc_reset_out.module,
        &mod_do_smarc_boot_0.module, 
        &mod_do_smarc_boot_1.module, 
        &mod_do_smarc_boot_2.module,
        &mod_systick_timer_ms.module),
};

hdl_button_t power_button = {
    .module.init = &hdl_button,
    .module.dependencies = hdl_module_dependencies(&mod_do_smarc_power_btn.module, &mod_systick_timer_ms.module ),
    .debounce_delay = 50,
    .hold_delay = 0,
};

atb3500_io_t mod_carrier_io = {
    .module.init = atb3500_io,
    .module.dependencies = hdl_module_dependencies(
    /***********************************************************
    *                      LED
    ***********************************************************/
        &mod_do_led_0_0.module, &mod_do_led_0_1.module, &mod_do_led_0_2.module,
        &mod_do_led_1_0.module, &mod_do_led_1_1.module, &mod_do_led_1_2.module, 
        &mod_do_led_2_0.module, &mod_do_led_2_1.module, &mod_do_led_2_2.module, 
    /***********************************************************
    *                          RELAY
    ***********************************************************/
        &mod_do_relay1.module, &mod_do_relay2.module,
        
    /***********************************************************
    *                        Other
    ***********************************************************/
        &mod_do_pci_switch.module, &mod_do_lte_reset.module,
    /***********************************************************
    *                        X1 Connector
    ***********************************************************/
        &mod_di_module_address_1.module, &mod_di_module_address_2.module, &mod_di_module_address_3.module,
        &mod_di_module_address_4.module, &mod_di_module_address_5.module, &mod_di_external_periph_irq_1.module,
        &mod_di_external_periph_irq_2.module, &mod_do_external_output_1.module, &mod_do_external_output_2.module
    )
    
};

atb3500_watchdog_t mod_watchdog = {
    .module.init = &atb3500_watchdog,
    .module.dependencies = hdl_module_dependencies(&mod_watchdog_timer.module)
};

atb3500_flash_serial_t mod_serial = {
  .module.init = atb3500_flash_serial,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module)

};

atb3500_i2c_gateway_t mod_i2c_gateway = {
  .module.init = &atb3500_i2c_gateway,
  .module.dependencies = hdl_module_dependencies(&mod_i2c0.module)
};

hdl_module_t power_domain = {
  .dependencies = hdl_module_dependencies( 
    &rail_24v.module,
    &rail_24vpoe.module,
    &rail_5v.module,
    &rail_3v3.module,
    &rail_2v5.module,
    &rail_1v8.module)
};

hdl_module_t app_module = {
    .dependencies = hdl_module_dependencies(
        &power_domain,
        &mod_smarc.module,
        &mod_carrier_io.module,
        &mod_watchdog.module,
        &mod_serial.module,
        &mod_spi3_server_dma.module,
        &mod_i2c_gateway.module
   )
};



#endif