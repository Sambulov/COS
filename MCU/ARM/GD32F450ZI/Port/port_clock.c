#include "hdl_portable.h"
#include "Macros.h"

#define IRC48M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
//#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC32K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)

#define MAX_SYS_CLOCK             240000000UL
#define PLL_MAX_FREQ              MAX_SYS_CLOCK
#define APB1_MAX_FREQ             120000000UL
#define APB2_MAX_FREQ             60000000UL

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

hdl_module_state_t hdl_clock_irc16m(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC16M, RCU_FLAG_IRC16MSTB, IRC16M_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_irc40k(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC32K, RCU_FLAG_IRC32KSTB, IRC32K_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_irc48m(void *desc, uint8_t enable) {
  if(enable)
    return _hdl_clock_osc_en((hdl_clock_t *)desc, RCU_IRC48M, RCU_FLAG_IRC48MSTB, IRC48M_STARTUP_TIMEOUT);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_selector_pll(void *desc, uint8_t enable) {
  if (enable) {
    hdl_clock_t *clock = (hdl_clock_t *)desc;
    if (clock->module.dependencies == NULL || 
        clock->module.dependencies[0] == NULL)
      return HDL_MODULE_INIT_FAILED;
    hdl_clock_t *clock_src = (hdl_clock_t *)clock->module.dependencies[0];
    if(clock_src->module.init == &hdl_clock_hxtal) {
      HDL_REG_SET(RCU_PLL, RCU_PLLSRC_HXTAL);
    }
    else if(clock_src->module.init == &hdl_clock_irc16m) {
      HDL_REG_CLEAR(RCU_PLL, RCU_PLLSRC_HXTAL);
    }
    else {
      return HDL_MODULE_INIT_FAILED;
    }
    hdl_clock_calc_div(clock, clock_src, 1);
    return HDL_MODULE_INIT_OK;
  }
  else {
    return HDL_MODULE_DEINIT_OK;
  }
}

static hdl_module_state_t _hdl_clock_pll_init(hdl_clock_prescaler_t *hdl_prescaler, uint32_t max_freq, 
                                              uint32_t factor, 
                                              uint32_t bits_from, uint32_t bits_to,
                                              hdl_module_initializer_t base_clock) {
  while (HDL_TRUE) {
    if (hdl_prescaler->module.dependencies == NULL || hdl_prescaler->module.dependencies[0] == NULL) break;
    hdl_clock_t *clock_src = (hdl_clock_t *)hdl_prescaler->module.dependencies[0];
    if(clock_src->module.init != base_clock) break;
    if(max_freq) {
      hdl_clock_calc_mul((hdl_clock_t *)hdl_prescaler, clock_src, hdl_prescaler->muldiv_factor);
      if(hdl_get_clock((hdl_clock_t *)hdl_prescaler) > max_freq) break;
    }
    else {
      hdl_clock_calc_div((hdl_clock_t *)hdl_prescaler, clock_src, hdl_prescaler->muldiv_factor);
    }
    HDL_REG_MODIFY(RCU_PLL, BITS(bits_from, bits_to), (hdl_prescaler->muldiv_factor) << bits_from);
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_selector_pll_vco(void *desc, uint8_t enable) {
  hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if(enable && CHECK_PLL_PSC_VALID(hdl_prescaler->muldiv_factor))
    return _hdl_clock_pll_init(hdl_prescaler, 0, hdl_prescaler->muldiv_factor, 0, 5, &hdl_clock_selector_pll);
  HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLPSC, RCU_PLLPSC_DIV_MAX);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_pll_n(void *desc, uint8_t enable) {
  return HDL_MODULE_INIT_OK;
  hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if(enable) {
    uint32_t ss_modulation_inc = 0U;
    uint32_t ss_modulation_reg = RCU_PLLSSCTL;
    /* calculate the minimum factor of PLLN */
    if((ss_modulation_reg & RCU_PLLSSCTL_SSCGON) == RCU_PLLSSCTL_SSCGON) {
        if((ss_modulation_reg & RCU_SS_TYPE_DOWN) == RCU_SS_TYPE_DOWN) {
            ss_modulation_inc += RCU_SS_MODULATION_DOWN_INC;
        } else {
            ss_modulation_inc += RCU_SS_MODULATION_CENTER_INC;
        }
    }
    if(CHECK_PLL_N_VALID(hdl_prescaler->muldiv_factor, ss_modulation_inc))
      return _hdl_clock_pll_init(hdl_prescaler, -1, (hdl_prescaler->muldiv_factor << 6), 6, 14, &hdl_clock_selector_pll_vco);
  }
  HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLN, RCU_PLLN_MUL_MIN << 6);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_pll_p(void *desc, uint8_t enable) {
  hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if(enable && CHECK_PLL_P_VALID(hdl_prescaler->muldiv_factor))
    return _hdl_clock_pll_init((hdl_clock_prescaler_t *)desc, 0, (((hdl_prescaler->muldiv_factor >> 1) - 1U) << 16), 16, 17, &hdl_clock_pll_n);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_pll_q(void *desc, uint8_t enable) {
  hdl_clock_prescaler_t *hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if(enable && CHECK_PLL_Q_VALID(hdl_prescaler->muldiv_factor))
    return _hdl_clock_pll_init((hdl_clock_prescaler_t *)desc, 0, (hdl_prescaler->muldiv_factor << 24), 24, 27, &hdl_clock_pll_n);
  return HDL_MODULE_DEINIT_OK;
}

// hdl_module_state_t hdl_clock_pll_i2s(void *desc, uint8_t enable) {
//   return HDL_MODULE_DEINIT_OK;
// }

// hdl_module_state_t hdl_clock_pll_sai(void *desc, uint8_t enable) {
//   return HDL_MODULE_DEINIT_OK;
// }


static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
  rcu_system_clock_source_config(src);
  uint32_t stb_cnt = CK_SYS_STARTUP_TIMEOUT;
  src <<= 2;
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
    if(hdl_clock_src->module.init == &hdl_clock_pll_p) {
      RCU_APB1EN |= RCU_APB1EN_PMUEN;
      PMU_CTL |= PMU_CTL_LDOVS;
      /* enable PLL */
      RCU_CTL |= RCU_CTL_PLLEN;
      /* wait until PLL is stable */
      while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
      }
      /* Enable the high-drive to extend the clock frequency to 240 Mhz */
      PMU_CTL |= PMU_CTL_HDEN;
      while(0U == (PMU_CS & PMU_CS_HDRF)){
      }
      /* select the high-drive mode */
      PMU_CTL |= PMU_CTL_HDS;
      while(0U == (PMU_CS & PMU_CS_HDSRF)){
      } 
      RCU_PLL = (25U | (480U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                (RCU_PLLSRC_HXTAL) | (10U << 24U));

          /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }
      return _hdl_clock_system_switch(RCU_CKSYSSRC_PLLP);
    }
    else if(hdl_clock_src->module.init == &hdl_clock_hxtal) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_HXTAL);
    }
    else if(hdl_clock_src->module.init == &hdl_clock_irc16m) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_IRC16M);
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
    hdl_clock_prescaler_t *prescaler = (hdl_clock_prescaler_t *)desc;
    if(prescaler->muldiv_factor >= 64)
      prescaler->muldiv_factor >>= 1;
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 4, 7, MAX_SYS_CLOCK, &hdl_clock_system);
  }
  rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV512);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_apb1(void *desc, uint8_t enable) {
  if (enable) {
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 10, 12, APB1_MAX_FREQ, &hdl_clock_ahb);
  }
  rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV16);
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_clock_apb2(void *desc, uint8_t enable) {
  if (enable) {
    return _hdl_bus_clock_cnf((hdl_clock_prescaler_t *)desc, 13, 15, APB2_MAX_FREQ, &hdl_clock_ahb);
  }
  rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV16);
  return HDL_MODULE_DEINIT_OK;
}

// hdl_module_state_t hdl_clock_selector_rtc(void *desc, uint8_t enable) {
//   if (enable) {
//     hdl_clock_t *clock = (hdl_clock_t *)desc;
//     if (clock->module.dependencies == NULL || clock->module.dependencies[0] == NULL)
//       return HDL_MODULE_INIT_FAILED;
//     hdl_clock_t *clock_src = (hdl_clock_t *)clock->module.dependencies[0];
//     if(clock_src->module.init == &hdl_clock_hxtal) {
//       /* RTC is cloking by HXTAL / 32 */
//       rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
//       hdl_clock_calc_div(clock, clock_src, 32);
//     }
//     else if(clock_src->module.init == &hdl_clock_lxtal) {
//       rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
//       hdl_clock_calc_div(clock, clock_src, 1);
//     }
//     else if (clock_src->module.init == &hdl_clock_irc40k) {
//       rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
//       hdl_clock_calc_div(clock, clock_src, 1);
//     }
//     else {
//       return HDL_MODULE_INIT_FAILED;
//     }
//     return HDL_MODULE_INIT_OK;
//   }
//   else {
//     rcu_rtc_clock_config(RCU_RTCSRC_NONE);
//     return HDL_MODULE_DEINIT_OK;
//   }
// }
