#include "hdl_portable.h"
#include "Macros.h"

#define IRC28M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC40K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)

#define MAX_SYS_CLOCK             72000000UL
#define PLL_MAX_FREQ              MAX_SYS_CLOCK
#define APB1_MAX_FREQ             72000000UL

static hdl_module_state_t _hdl_clock_osc_en(hdl_clock_t* hdl_clock, rcu_osci_type_enum osc, rcu_flag_enum stb_flag, uint32_t timeout) {
  rcu_osci_on(osc); /* Turn on oscillator */
  uint32_t stb_timer = HXTAL_STARTUP_TIMEOUT;
  FlagStatus osci_statable = RESET;
  while ((RESET == osci_statable) && (stb_timer--)) /* Wait until HXTAL will be stable */
    osci_statable = rcu_flag_get(stb_flag);
  if (osci_statable == RESET)
    return HDL_MODULE_INIT_FAILED;
  return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_clock_hxtal(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_HXTAL, RCU_FLAG_HXTALSTB, HXTAL_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_lxtal(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_LXTAL, RCU_FLAG_LXTALSTB, LXTAL_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_irc8m(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC8M, RCU_FLAG_IRC8MSTB, IRC8M_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_irc40k(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC40K, RCU_FLAG_IRC40KSTB, IRC40K_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_irc28m(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC28M, RCU_FLAG_IRC28MSTB, IRC28M_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_hxtal_prescaler(void *desc, uint8_t enable) {
  if (enable) {
    hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
    if (hdl_prescaler->module.dependencies == NULL || hdl_prescaler->module.dependencies[0] == NULL)
      return HDL_MODULE_INIT_FAILED;
    uint32_t prescaler_value = hdl_prescaler->muldiv_factor;
    if (prescaler_value == 0 || prescaler_value > 16)
      return HDL_MODULE_INIT_FAILED;
      HDL_REG_MODIFY(RCU_CFG1, RCU_CFG1_PREDV, hdl_prescaler->muldiv_factor - 1);
    hdl_clock_calc_div((hdl_clock_t *)hdl_prescaler, (hdl_clock_t *)hdl_prescaler->module.dependencies[0], hdl_prescaler->muldiv_factor);
    return HDL_MODULE_INIT_OK;
  }
  else {
    HDL_REG_MODIFY(RCU_CFG1, RCU_CFG1_PREDV, RCU_PLL_PREDV16);
    return HDL_MODULE_DEINIT_OK;
  }
}

#ifdef GD32F10X_CL
hdl_module_state_t hdl_clock_pll1(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_PLL1_CK, RCU_FLAG_PLL1STB, PLL1_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_pll2(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_PLL2_CK, RCU_FLAG_PLL2STB, PLL2_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}
#endif /* GD32F10X_CL */

hdl_module_state_t hdl_clock_selector_pll(void *desc, uint8_t enable) {
  if (enable) {
    hdl_clock_t *clock = (hdl_clock_t *)desc;
    if (clock->module.dependencies == NULL || 
        clock->module.dependencies[0] == NULL)
      return HDL_MODULE_INIT_FAILED;
    hdl_clock_t *clock_src = (hdl_clock_t *)clock->module.dependencies[0];
    if(clock_src->module.init == &hdl_clock_hxtal_prescaler) {
      HDL_REG_SET(RCU_CFG0, RCU_PLLSRC_HXTAL);
      hdl_clock_calc_div((hdl_clock_t *)clock, clock_src, 1);
    }
    else if(clock_src->module.init == &hdl_clock_irc8m) {
      RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
      RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2);
      hdl_clock_calc_div(clock, clock_src, 2);
    }
    else {
      return HDL_MODULE_INIT_FAILED;
    }
    
    return HDL_MODULE_INIT_OK;
  }
  else {
    /* CMSIS function */
    //RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
    return HDL_MODULE_DEINIT_OK;
  }
}

hdl_module_state_t hdl_clock_pll(void *desc, uint8_t enable) {
  while (enable) {
    hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
    if (hdl_prescaler->module.dependencies == NULL || hdl_prescaler->module.dependencies[0] == NULL) break;
    hdl_clock_t *clock_src = (hdl_clock_t *)hdl_prescaler->module.dependencies[0];
    hdl_clock_calc_mul((hdl_clock_t *)hdl_prescaler, clock_src, hdl_prescaler->muldiv_factor);
    if(hdl_get_clock((hdl_clock_t *)hdl_prescaler) > PLL_MAX_FREQ) break;
    uint32_t pll_cnf = hdl_prescaler->muldiv_factor;
    if((pll_cnf < 2) || (pll_cnf > 32)) break;
    pll_cnf -= (pll_cnf > 15)? 1: 2;
    pll_cnf = ((pll_cnf & 0x0F) << 18) | ((pll_cnf & 0x10) << (27 - 4));
    HDL_REG_MODIFY(RCU_CFG0, (RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4), pll_cnf);
    if(_hdl_clock_osc_en((hdl_clock_t *)desc, RCU_PLL_CK, RCU_FLAG_PLLSTB, PLL_STARTUP_TIMEOUT) != HDL_MODULE_INIT_OK) {
      rcu_osci_off(RCU_PLL_CK);
      break;
    }
    return HDL_MODULE_INIT_OK;
  }
  HDL_REG_CLEAR(RCU_CFG0, (RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4));
  return HDL_MODULE_DEINIT_OK;
}

static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
  rcu_system_clock_source_config(src);
  uint32_t stb_cnt = CK_SYS_STARTUP_TIMEOUT;
  while((src != (RCU_CFG0 & RCU_CFG0_SCSS)) && stb_cnt--) ;
  return (src != (RCU_CFG0 & RCU_CFG0_SCSS))? HDL_MODULE_INIT_FAILED: HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_clock_system(void *desc, uint8_t enable) {
  if (enable) {
    hdl_clock_t *hdl_clock = (hdl_clock_t *)desc;
    if (hdl_clock->module.dependencies == NULL || 
        hdl_clock->module.dependencies[0] == NULL || 
        hdl_clock->module.dependencies[1] == NULL)
      return HDL_MODULE_INIT_FAILED;
    hdl_clock_t *hdl_clock_src = (hdl_clock_t *)hdl_clock->module.dependencies[1];
    hdl_clock_calc_div(hdl_clock, hdl_clock_src, 1);
    if(hdl_clock_src->module.init == &hdl_clock_pll) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_PLL);
    }
    else if(hdl_clock_src->module.init == &hdl_clock_hxtal) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_HXTAL);
    }
    else if(hdl_clock_src->module.init == &hdl_clock_irc8m) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_IRC8M);
    }
    /* TODO: config clock mon */
  }
  /* TODO: switch to irc8 */
  return HDL_MODULE_DEINIT_OK;
}

static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_prescaler_t *hdl_prescaler, 
                                              uint32_t bit_from, uint32_t bit_to, 
                                              uint32_t check_frec, hdl_module_initializer_t base_clock) {
  if ((hdl_prescaler->muldiv_factor == 0) || 
    (hdl_prescaler->module.dependencies == NULL) || 
    (hdl_prescaler->module.dependencies[0] == NULL))
    return HDL_MODULE_INIT_FAILED;
  hdl_clock_t *clock_src = (hdl_clock_t *)hdl_prescaler->module.dependencies[0];
  if(clock_src->module.init != base_clock)
    return HDL_MODULE_INIT_FAILED;
  hdl_clock_calc_div((hdl_clock_t *)hdl_prescaler, clock_src, hdl_prescaler->muldiv_factor);
  if(hdl_get_clock((hdl_clock_t *)hdl_prescaler) > check_frec)
    return HDL_MODULE_INIT_FAILED;
  uint32_t div_cnf = 31 - __CLZ(hdl_prescaler->muldiv_factor);
  if (div_cnf)
    div_cnf = ((div_cnf - 1) << bit_from) | (1UL << bit_to);
  HDL_REG_MODIFY(RCU_CFG0, BITS(bit_from, bit_to), div_cnf);
  return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_clock_ahb(void *desc, uint8_t enable) {
  if (enable) {
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 4, 7, MAX_SYS_CLOCK, &hdl_clock_system);
  }
  rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV512);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_apb1(void *desc, uint8_t enable) {
  if (enable) {
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 8, 10, APB1_MAX_FREQ, &hdl_clock_ahb);
  }
  rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV16);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_apb2(void *desc, uint8_t enable) {
  if (enable) {
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 11, 13, MAX_SYS_CLOCK, &hdl_clock_ahb);
  }
  rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV16);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_selector_rtc(void *desc, uint8_t enable) {
  if (enable) {
    hdl_clock_t *clock = (hdl_clock_t *)desc;
    if (clock->module.dependencies == NULL || clock->module.dependencies[0] == NULL)
      return HDL_MODULE_INIT_FAILED;
    hdl_clock_t *clock_src = (hdl_clock_t *)clock->module.dependencies[0];
    if(clock_src->module.init == &hdl_clock_hxtal) {
      /* RTC is cloking by HXTAL / 32 */
      rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
      hdl_clock_calc_div(clock, clock_src, 32);
    }
    else if(clock_src->module.init == &hdl_clock_lxtal) {
      rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
      hdl_clock_calc_div(clock, clock_src, 1);
    }
    else if (clock_src->module.init == &hdl_clock_irc40k) {
      rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
      hdl_clock_calc_div(clock, clock_src, 1);
    }
    else {
      return HDL_MODULE_INIT_FAILED;
    }
    return HDL_MODULE_INIT_OK;
  }
  else {
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);
    return HDL_MODULE_DEINIT_OK;
  }
}
