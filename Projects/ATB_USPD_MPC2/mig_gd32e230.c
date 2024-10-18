/*
  Module Init Graph (MIG)
*/

#include "hdl.h"

#define HDL_I2C_SLAVE_ADDR           0x23

#define HDL_HXTAL_CLOCK              25000000
#define HDL_LXTAL_CLOCK              32768
#define HDL_HXTAL_2_PLLSEL_PREDIV    5                         /* Can be 1 to 16 */
#define HDL_PLLMUL                   14                        /* Can be 2 to 32 */
#define HDL_AHB_PREDIV               1                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
#define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
#define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel         /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
#define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

#define HDL_SYSTICK_COUNTER_RELOAD  70000 - 1                  /* Clocked by AHB   */
#define HDL_TIMER0_COUNTER_RELOAD   8000 - 1                   /* Clocked by APB 2 */
#define HDL_TIMER2_COUNTER_RELOAD   8000 - 1                   /* Clocked by APB 1 */

//_Static_assert( (HDL_SYS_CLOCK == mod_clock_pll) && (HDL_HXTAL_CLOCK * HDL_PLLMUL / HDL_HXTAL_2_PLLSEL_PREDIV) <= MAX_SYS_CLOCK, "Sys overclocking!");

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

const hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
};

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .config = &mod_sys_core_cnf
};

/**************************************************************
 *  NVIC, IRQ, EXTI
 *************************************************************/

hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_timer0 = {
  .irq_type = HDL_NVIC_IRQ13_TIMER0_BRK_UP_TRG_COM,
  .priority = 0,
  .priority_group = 1,
};

hdl_interrupt_t mod_irq_timer2 = {
  .irq_type = HDL_NVIC_IRQ16_TIMER2,
  .priority = 2,
  .priority_group = 2,
};

hdl_interrupt_t mod_irq_exti_0_1 = {
  .irq_type = HDL_NVIC_IRQ5_EXTI0_1,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_exti_2_3 = {
  .irq_type = HDL_NVIC_IRQ6_EXTI2_3,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_exti_4_15 = {
  .irq_type = HDL_NVIC_IRQ7_EXTI4_15,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_usart_0 = {
  .irq_type = HDL_NVIC_IRQ27_USART0,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_spi_0 = {
  .irq_type = HDL_NVIC_IRQ25_SPI0,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_spi_1 = {
  .irq_type = HDL_NVIC_IRQ26_SPI1,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_ev = {
  .irq_type = HDL_NVIC_IRQ23_I2C0_EV,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_er = {
  .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1_ev = {
  .irq_type = HDL_NVIC_IRQ24_I2C1_EV,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1_er = {
  .irq_type = HDL_NVIC_IRQ34_I2C1_ER,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_svc = {
  .irq_type = HDL_NVIC_EXCEPTION_SVCall,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_adc = {
  .irq_type = HDL_NVIC_IRQ12_ADC_CMP,
  .priority = 0,
  .priority_group = 0,
};

extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
  &mod_nvic_cnf,
  &reset_handler,
  &nmi_handler,
  &hard_fault_handler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &svc_handler,
  ((void *)0),
  ((void *)0),
  &pend_sv_handler,
  &systick_handler,
  &wwdgt_handler,                    /* IRQ0 */ 
  &lvd_handler,                      /* IRQ1 */
  &rtc_handler,                      /* IRQ2 */
  &fmc_handler,                      /* IRQ3 */
  &rcu_handler,                      /* IRQ4 */
  &exti0_1_handler,                  /* IRQ5 */
  &exti2_3_IRQHandler,               /* IRQ6 */
  &exti4_15_handler,                 /* IRQ7 */
  &irq_n_handler,                    /* IRQ8 */
  &dma_channel0_handler,             /* IRQ9 */
  &dma_channel1_2_handler,           /* IRQ10 */
  &dma_channel3_4_handler,           /* IRQ11 */
  &adc_cmp_handler,                  /* IRQ12 */
  &timer0_brk_up_trg_com_handler,    /* IRQ13 */
  &timer0_channel_handler,           /* IRQ14 */
  &irq_n_handler,                    /* IRQ15 */
  &timer2_handler,                   /* IRQ16 */
  &timer5_handler,                   /* IRQ17 */
  &irq_n_handler,                    /* IRQ18 */
  &timer13_handler,                  /* IRQ19 */
  &timer14_handler,                  /* IRQ21 */
  &timer15_handler,                  /* IRQ22 */
  &timer16_handler,                  /* IRQ23 */
  &i2c0_ev_handler,                  /* IRQ24 */
  &i2c1_ev_handler,                  /* IRQ25 */
  &spi0_handler,                     /* IRQ26 */
  &spi1_handler,                     /* IRQ27 */
  &usart0_handler,                   /* IRQ28 */
  &usart1_handler,                   /* IRQ29 */
  &irq_n_handler,                    /* IRQ30 */
  &irq_n_handler,                    /* IRQ31 */
  &irq_n_handler,                    /* IRQ32 */
  &i2c0_er_handler,                  /* IRQ33 */
  &irq_n_handler,                    /* IRQ34 */
  &i2c1_er_handler,                  /* IRQ35 */
};

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .irq_latency = 0, /* TODO: define static assert */
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_exti_0_1, &mod_irq_exti_2_3, &mod_irq_exti_4_15,
                               &mod_irq_timer0, &mod_irq_timer2, &mod_irq_usart_0, &mod_irq_spi_0, &mod_irq_spi_1,
                               &mod_irq_i2c0_ev, &mod_irq_i2c0_er, &mod_irq_i2c1_ev, &mod_irq_i2c1_er, &mod_irq_svc,
                               &mod_irq_adc),
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf,
};

/**************************************************************
 *  EXTI
 *************************************************************/

hdl_exti_t mod_nvic_exti_line_0 = {
  .line = HDL_EXTI_LINE_0,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PA,
  .trigger = HDL_EXTI_TRIGGER_RISING_FALLING
};

hdl_exti_t mod_nvic_exti_line_8 = {
  .line = HDL_EXTI_LINE_8,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PB,
  .trigger = HDL_EXTI_TRIGGER_FALLING
};

hdl_exti_t mod_nvic_exti_line_15 = {
  .line = HDL_EXTI_LINE_15,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PA,
  .trigger = HDL_EXTI_TRIGGER_RISING
};

hdl_exti_controller_t mod_exti = {
  .module.dependencies = hdl_module_dependencies(&mod_nvic.module),
  .module.init = &hdl_exti,
  .module.reg = (void *)EXTI,
  .extis = hdl_extis(&mod_nvic_exti_line_0, &mod_nvic_exti_line_8, &mod_nvic_exti_line_15)
};

/**************************************************************
 *  Clock
 *************************************************************/

hdl_clock_t mod_clock_irc8m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000
  }})
};

hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_lxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_irc28m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC28M, 
    .property.freq = 28000000
  }})
};

hdl_clock_t mod_clock_irc40k = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000
  }})
};

hdl_clock_t mod_clock_pll_sel = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV
  }})
};

hdl_clock_t mod_clock_pll = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_sel.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL
  }})
};

hdl_clock_t mod_clock_system = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_clock_irc8m.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYS_SEL
  }})
};

hdl_clock_t mod_clock_rtc = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_RTC_SEL
  }})
};

hdl_clock_t mod_clock_ahb = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_system.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV
  }})
};

hdl_clock_t mod_clock_apb1 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV
  }})
};

hdl_clock_t mod_clock_apb2 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV
  }})
};

/**************************************************************
 *  DMA
 *************************************************************/
const hdl_dma_config_t mod_dma_config = {
  .rcu = RCU_DMA
};

hdl_dma_t mod_dma = {
  .module.init = &hdl_dma,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void*)DMA_BASE,
  .config = &mod_dma_config
};

const hdl_dma_channel_config_t mod_adc_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .direction = DMA_PERIPHERAL_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_32BIT,
  .periph_width = DMA_PERIPHERAL_WIDTH_16BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 1
};

const hdl_dma_channel_config_t mod_m2m_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .direction = DMA_PERIPHERAL_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_32BIT,
  .periph_width = DMA_PERIPHERAL_WIDTH_32BIT,
  .memory_inc = 1,
  .periph_inc = 1,
  .m2m_direction = 1,
  .circular = 0
};

hdl_dma_channel_t mod_adc_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH0,
  .config = &mod_adc_dma_ch_config
};

hdl_dma_channel_t mod_m2m_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .config = &mod_m2m_dma_ch_config
};

const hdl_tick_counter_timer_config_t mod_tick_counter0_cnf = {
  .alignedmode = TIMER_COUNTER_EDGE,
  .clockdivision = TIMER_CKDIV_DIV1,
  .counterdirection = TIMER_COUNTER_UP,
  .period = HDL_TIMER0_COUNTER_RELOAD,
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER0
};

const hdl_tick_counter_timer_config_t mod_tick_counter2_cnf = {
  .alignedmode = TIMER_COUNTER_EDGE,
  .clockdivision = TIMER_CKDIV_DIV1,
  .counterdirection = TIMER_COUNTER_UP,
  .period = HDL_TIMER2_COUNTER_RELOAD,
  .prescaler = 0,
  .repetitioncounter = 0,
  .rcu = RCU_TIMER2
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = HDL_SYSTICK_COUNTER_RELOAD
  //TODO: prediv 8
};

hdl_tick_counter_t mod_tick_counter0 = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module),
  .module.reg = (void *)TIMER0,
  .config = &mod_tick_counter0_cnf
};

hdl_tick_counter_t mod_tick2_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module),
  .module.reg = (void *)TIMER2,
  .config = &mod_tick_counter2_cnf
};

hdl_tick_counter_t mod_systick_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)SysTick,
  .config.systick = &mod_systick_counter_cnf
};

const hdl_time_counter_config_t mod_timer_ms_cnf = {
  .reload_interrupt = &mod_irq_systick,
};

hdl_time_counter_t mod_timer_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_timer_ms_cnf
};

/**************************************************************
 *  ADC
 *************************************************************/

hdl_adc_source_t mod_adc_ain1 = {
  .channel = HDL_ADC_CHANNEL_4,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_ain2 = {
  .channel = HDL_ADC_CHANNEL_6,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_ain3 = {
  .channel = HDL_ADC_CHANNEL_7,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_ain4 = {
  .channel = HDL_ADC_CHANNEL_5,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_gpio_pin_t mod_pin_ain1;
hdl_gpio_pin_t mod_pin_ain2;
hdl_gpio_pin_t mod_pin_ain3;
hdl_gpio_pin_t mod_pin_ain4;

const hdl_adc_config_t mod_adc_cnf = {
  .adc_interrupt = &mod_irq_adc,
  .resolution = HDL_ADC_RESOLUTION_12BIT,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_ain1, &mod_adc_ain2, &mod_adc_ain3, &mod_adc_ain4),
};

//TODO: init mod_pin_ain1-4
hdl_adc_t mod_adc = {
  .module.init = &hdl_adc,
  .module.dependencies = hdl_module_dependencies(&mod_clock_irc28m.module, &mod_timer_ms.module, &mod_adc_dma_ch.module, &mod_nvic.module,
                                                 &mod_pin_ain1.module, &mod_pin_ain2.module, &mod_pin_ain3.module, &mod_pin_ain4.module),
  .module.reg = (void*)ADC,
  .config = &mod_adc_cnf
};

/**************************************************************
 *  GPIO
 *************************************************************/
const hdl_gpio_port_hw_config_t hdl_port_config_a = {
  .rcu = RCU_GPIOA
};

const hdl_gpio_port_hw_config_t hdl_port_config_b = {
  .rcu = RCU_GPIOB
};

const hdl_gpio_port_hw_config_t hdl_port_config_c = {
  .rcu = RCU_GPIOC
};

const hdl_gpio_port_hw_config_t hdl_port_config_f = {
  .rcu = RCU_GPIOF
};

hdl_gpio_port_t hdl_gpio_port_a = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOA,
  .config = hdl_gpio_port_config(.hwc = &hdl_port_config_a)
};

hdl_gpio_port_t hdl_gpio_port_b = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOB,
  .config = hdl_gpio_port_config(.hwc = &hdl_port_config_b)
};

hdl_gpio_port_t hdl_gpio_port_c = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOC,
  .config = hdl_gpio_port_config(.hwc = &hdl_port_config_c)
};

hdl_gpio_port_t hdl_gpio_port_f = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOF,
  .config = hdl_gpio_port_config(.hwc = &hdl_port_config_f)
};

const hdl_gpio_pin_hw_config_t mod_gpio_output_pp_mode = {
  .type = GPIO_MODE_OUTPUT,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_pullup_mode = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_PULLUP,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
};

const hdl_gpio_pin_hw_config_t mod_gpio_alternate_swd_mode = {
  .af = GPIO_AF_0,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c_0_mode = {
  .af = GPIO_AF_1,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c_1_mode = {
  .af = GPIO_AF_5,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

const hdl_gpio_pin_hw_config_t hdl_gpio_spi_mode = {
  .type = GPIO_MODE_AF,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_50MHZ,
  .af = GPIO_AF_0,
  .otype = GPIO_OTYPE_PP,
};

hdl_gpio_pin_t mod_pin_ain1 = {   //(PA4)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_analog)
};

hdl_gpio_pin_t mod_pin_ain2 = {  //(PA6)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_analog)
};

hdl_gpio_pin_t mod_pin_ain3 = {  //(PA7)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_analog)
};

hdl_gpio_pin_t mod_pin_ain4 = {  //(PA5)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_analog)
};

hdl_gpio_pin_t uspd_mpc_in1 = { //(PA3)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in2 = { //(PA2)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in3 = { //(PB1)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in4 = { //(PB0)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in5 = { //(PA10)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in6 = { //(PA9)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in7 = { //(PB9)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_in8 = { //(PB8)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_out1 = { //(PB7)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out2 = { //(PB6)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out3 = { //(PB5)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out4 = { //(PB4)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out5 = { //(PB3)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out6 = { //(PA15)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out7 = { //(PA12)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_out8 = { //(PA11)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t uspd_mpc_pps = { //(PA0)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_input_pullup_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_int = { //(PA1)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t mod_pin_12c1_scl = { //(PB10)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_1_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t mod_pin_12c1_sda = { //(PB11)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_1_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t mod_pin_12c0_scl = { //(PF6)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_0_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t mod_pin_12c0_sda = { //(PF7)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_0_mode, .inactive_default = HDL_GPIO_HIGH)
};

hdl_gpio_pin_t uspd_mpc_eeprom_wp = { //(PC13)
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
};

hdl_gpio_pin_t mod_spi_0_mosi = { //(PA7)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_7,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_0_miso = { //(PA6)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_6,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_0_sck = { //(PA5)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_0_cs = { //(PA4),
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_1_mosi = { //(PB15)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_15,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_1_miso = { //(PB14)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_14,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_1_sck = { //(PB13)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_13,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
};

hdl_gpio_pin_t mod_spi_1_cs = { //(PB12)
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_12,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &hdl_gpio_spi_mode)
};

/**************************************************************
 *  I2C
 *************************************************************/

const hdl_i2c_config_t mod_i2c0_cnf = {
  .addr0 = 0,
  .addr1 = 0,
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = &mod_irq_i2c0_er,
  .ev_interrupt = &mod_irq_i2c0_ev,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C0
};

hdl_i2c_t mod_i2c0 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_pin_12c0_scl.module, &mod_pin_12c0_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
  .module.reg = (void *)I2C0,
  .config = &mod_i2c0_cnf,
};

const hdl_i2c_config_t mod_i2c1_cnf = {
  .addr0 = HDL_I2C_SLAVE_ADDR,
  .addr1 = 0,
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = &mod_irq_i2c1_er,
  .ev_interrupt = &mod_irq_i2c1_ev,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C1
};

hdl_i2c_t mod_i2c1 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_pin_12c1_scl.module, &mod_pin_12c1_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
  .module.reg = (void *)I2C1,
  .config = &mod_i2c1_cnf,
};

/**************************************************************
 *  SPI
 *************************************************************/

const hdl_spi_client_config_t mod_spi0_cnf = {
  .rcu = RCU_SPI0,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE,
  .prescale = HDL_SPI_PSC_32,
  .spi_interrupt = &mod_irq_spi_0
};

hdl_spi_client_t mod_spi0 = {
  .module.init = &hdl_spi_client,
  .module.dependencies = hdl_module_dependencies(&mod_spi_0_mosi.module, &mod_spi_0_miso.module, &mod_spi_0_sck.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module),
  .module.reg = (void *)SPI0,
  .config = &mod_spi0_cnf
};

hdl_spi_client_ch_t mod_spi0_ch0 = {
  .module.init = &hdl_spi_ch,
  .module.dependencies = hdl_module_dependencies(&mod_spi0.module, &mod_spi_0_cs.module),
};

const hdl_spi_client_config_t mod_spi1_cnf = {
  .rcu = RCU_SPI0,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_1EDGE,
  .prescale = HDL_SPI_PSC_2,
  .spi_interrupt = &mod_irq_spi_1
};

hdl_spi_client_t mod_spi1 = {
  .module.init = &hdl_spi_client,
  .module.dependencies = hdl_module_dependencies(&mod_spi_1_mosi.module, &mod_spi_1_miso.module, &mod_spi_1_sck.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module),
  .module.reg = (void *)SPI1,
  .config = &mod_spi1_cnf
};

hdl_spi_client_ch_t mod_spi1_ch0 = {
  .module.init = &hdl_spi_ch,
  .module.dependencies = hdl_module_dependencies(&mod_spi1.module, &mod_spi_1_cs.module),
};

extern hdl_i2c_t uspd_mpc_eeprom_i2c                  __attribute__ ((alias ("mod_i2c0")));
extern hdl_i2c_t uspd_mpc_i2c_if                      __attribute__ ((alias ("mod_i2c1")));
extern hdl_spi_client_ch_t uspd_mpc_adc_spi           __attribute__ ((alias ("mod_spi0_ch0")));
extern hdl_spi_client_ch_t uspd_mpc_gpio_expander_spi __attribute__ ((alias ("mod_spi1_ch0")));
