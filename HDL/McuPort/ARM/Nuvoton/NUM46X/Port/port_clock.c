#include "hdl_portable.h"
#include "Macros.h"

typedef struct {
  hdl_module_t module;
  const hdl_clock_config_t *config;
  hdl_clock_freq_t freq;
} hdl_clock_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_private_t, hdl_clock_t, HDL_CLOCK_PRV_SIZE, port_clock.h);

#define IRC48M_STARTUP_TIMEOUT    ((uint32_t)0xFFFFFFFF)
#define IRC12M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC10K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFFFFFF)
#define HXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)

// #define MAX_SYS_CLOCK             240000000UL
// #define PLL_MAX_FREQ              MAX_SYS_CLOCK
// #define APB2_MAX_FREQ             120000000UL
// #define APB1_MAX_FREQ             60000000UL

// /*!
//     \brief      get flag state
//     \param[in]  flag:
//       \arg        PMU_FLAG_WAKEUP: wakeup flag
//       \arg        PMU_FLAG_STANDBY: standby flag
//       \arg        PMU_FLAG_LVD: lvd flag
//       \arg        PMU_FLAG_BLDORF: backup SRAM LDO ready flag
//       \arg        PMU_FLAG_LDOVSRF: LDO voltage select ready flag
//       \arg        PMU_FLAG_HDRF: high-driver ready flag
//       \arg        PMU_FLAG_HDSRF: high-driver switch ready flag
//       \arg        PMU_FLAG_LDRF: low-driver mode ready flag
//     \retval     HDL_MODULE_FAULT
//     \retval     HDL_MODULE_ACTIVE
// */
// static hdl_module_state_t _hdl_power_manager_wait_stable(uint32_t pmu_flag){
//   uint32_t stb_timer = PMU_STARTUP_TIMEOUT;
//   FlagStatus osci_statable = RESET;
//   while ((RESET == osci_statable) && (stb_timer--))
//     osci_statable = pmu_flag_get(pmu_flag);
//   if (osci_statable == RESET)
//     return HDL_MODULE_FAULT;
//   return HDL_MODULE_ACTIVE;
// }
// /* TODO: This function have to be in other module */
// static hdl_module_state_t _hdl_power_manager_high_frequency(uint8_t enable){
//   if(enable){
//       /* Enable the high-drive to extend the clock frequency to 240 Mhz */
//       PMU_CTL |= PMU_CTL_LDOVS;
//       /* Enable the high-drive to extend the clock frequency to 240 Mhz */
//       PMU_CTL |= PMU_CTL_HDEN;
//       if(_hdl_power_manager_wait_stable(PMU_FLAG_HDRF) != HDL_MODULE_ACTIVE)
//         return HDL_MODULE_FAULT;
//       /* select the high-drive mode */
//       PMU_CTL |= PMU_CTL_HDS;
//       if(_hdl_power_manager_wait_stable(PMU_CS_HDSRF) != HDL_MODULE_ACTIVE)
//         return HDL_MODULE_FAULT;
//       return HDL_MODULE_ACTIVE;
//   }
//   PMU_CTL &= ~(PMU_CTL_LDOVS | PMU_CTL_HDEN | PMU_CTL_HDS);
//   return HDL_MODULE_UNLOADED;
// }

// // hdl_module_state_t hdl_clock_pll_i2s(void *desc, uint8_t enable) {
// //   return HDL_MODULE_UNLOADED;
// // }

// // hdl_module_state_t hdl_clock_pll_sai(void *desc, uint8_t enable) {
// //   return HDL_MODULE_UNLOADED;
// // }


// static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
//   rcu_system_clock_source_config(src);
//   uint32_t stb_cnt = CK_SYS_STARTUP_TIMEOUT;
//   src <<= 2;
//   while((src != (RCU_CFG0 & RCU_CFG0_SCSS)) && stb_cnt--) ;
//   return (src != (RCU_CFG0 & RCU_CFG0_SCSS))? HDL_MODULE_FAULT: HDL_MODULE_ACTIVE;
// }

// static hdl_module_state_t _hdl_clock_system(hdl_clock_private_t *clk, uint8_t enable) {
//   if (enable) {
//     if (clk->module.dependencies == NULL || 
//         clk->module.dependencies[0] == NULL || 
//         clk->module.dependencies[1] == NULL)
//       return HDL_MODULE_FAULT;
//     hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[2];
//     if(clock_src == NULL) clock_src = (hdl_clock_private_t *)clk->module.dependencies[1];
//     hdl_clock_calc_div(&clock_src->freq, 1, &clk->freq);
//     if(clock_src->config->type == HDL_CLOCK_TYPE_PLL_P) {
//       _hdl_power_manager_high_frequency(HDL_TRUE);
//       return _hdl_clock_system_switch(RCU_CKSYSSRC_PLLP);
//     }
//     else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
//       return _hdl_clock_system_switch(RCU_CKSYSSRC_HXTAL);
//     }
//     else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC16M) {
//       return _hdl_clock_system_switch(RCU_CKSYSSRC_IRC16M);
//     }
//     /* TODO: config clock mon */
//   }
//   _hdl_clock_system_switch(RCU_CKSYSSRC_IRC16M);
//   _hdl_power_manager_high_frequency(HDL_FALSE);
//   return HDL_MODULE_UNLOADED;
// }

// static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_private_t *clk, uint32_t bit_from, uint32_t bit_to, 
//                                              uint32_t check_frec, hdl_clock_type_t base_clock) {
//   uint32_t factor = clk->config->property.div;
//   if(factor >= 64) factor >>= 1;

//   if ((factor == 0) || (clk->module.dependencies == NULL) || (clk->module.dependencies[0] == NULL))
//     return HDL_MODULE_FAULT;
//   hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
//   if(clock_src->config->type != base_clock)
//     return HDL_MODULE_FAULT;
//   hdl_clock_calc_div(&clock_src->freq, factor, &clk->freq);
//   if((clk->freq.num / clk->freq.denom) > check_frec) return HDL_MODULE_FAULT;
//   uint32_t div_cnf = 31 - __CLZ(factor);
//   if (div_cnf)
//     div_cnf = ((div_cnf - 1) << bit_from) | (1UL << bit_to);
//   HDL_REG_MODIFY(RCU_CFG0, BITS(bit_from, bit_to), div_cnf);
//   return HDL_MODULE_ACTIVE;
// }

// // hdl_module_state_t hdl_clock_selector_rtc(void *desc, uint8_t enable) {
// //   if (enable) {
// //     hdl_clock_t *clock = (hdl_clock_t *)desc;
// //     if (clock->module.dependencies == NULL || clock->module.dependencies[0] == NULL)
// //       return HDL_MODULE_FAULT;
// //     hdl_clock_t *clock_src = (hdl_clock_t *)clock->module.dependencies[0];
// //     if(clock_src->module.init == &hdl_clock_hxtal) {
// //       /* RTC is cloking by HXTAL / 32 */
// //       rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
// //       hdl_clock_calc_div(clock, clock_src, 32);
// //     }
// //     else if(clock_src->module.init == &hdl_clock_lxtal) {
// //       rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
// //       hdl_clock_calc_div(clock, clock_src, 1);
// //     }
// //     else if (clock_src->module.init == &hdl_clock_irc40k) {
// //       rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
// //       hdl_clock_calc_div(clock, clock_src, 1);
// //     }
// //     else {
// //       return HDL_MODULE_FAULT;
// //     }
// //     return HDL_MODULE_ACTIVE;
// //   }
// //   else {
// //     rcu_rtc_clock_config(RCU_RTCSRC_NONE);
// //     return HDL_MODULE_UNLOADED;
// //   }
// // }


// static hdl_module_state_t _hdl_clock_osc_en(rcu_osci_type_enum osc, rcu_flag_enum stb_flag, uint32_t timeout) {
//   rcu_osci_on(osc); /* Turn on oscillator */
//   uint32_t stb_timer = HXTAL_STARTUP_TIMEOUT;
//   FlagStatus osci_statable = RESET;
//   while ((RESET == osci_statable) && (stb_timer--)) /* Wait until HXTAL will be stable */
//     osci_statable = rcu_flag_get(stb_flag);
//   if (osci_statable == RESET)
//     return HDL_MODULE_FAULT;
//   return HDL_MODULE_ACTIVE;
// }

// static hdl_module_state_t _hdl_clock_pll_init(hdl_clock_private_t *clk, uint32_t max_freq, 
//                                               uint32_t factor, uint32_t bits_from, uint32_t bits_to,
//                                               hdl_clock_type_t base_clock) {
//   while (HDL_TRUE) {
//     if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
//     hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
//     if(clock_src->config->type != base_clock) break;
//     if(max_freq) {
//       hdl_clock_calc_mul(&clock_src->freq, clk->config->property.mul, &clk->freq); //TODO: test ????
//       if((clk->freq.num / clk->freq.denom) > max_freq) break;
//     }
//     else {
//       hdl_clock_calc_div(&clock_src->freq, clk->config->property.div, &clk->freq);
//     }
//     HDL_REG_MODIFY(RCU_PLL, BITS(bits_from, bits_to), (clk->config->property.div) << bits_from);
//     return HDL_MODULE_ACTIVE;
//   }
//   return HDL_MODULE_UNLOADED;
// }

// static hdl_module_state_t _hdl_clock_selector_pll(hdl_clock_private_t *clk, uint8_t enable) {
//   if (enable) {
//     if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) return HDL_MODULE_FAULT;
//     hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
//     if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) HDL_REG_SET(RCU_PLL, RCU_PLLSRC_HXTAL);
//     else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC16M) HDL_REG_CLEAR(RCU_PLL, RCU_PLLSRC_HXTAL);
//     else return HDL_MODULE_FAULT;
//     hdl_clock_calc_div(&clock_src->freq, 1, &clk->freq);
//     if(CHECK_PLL_PSC_VALID(clk->config->property.div))
//       return _hdl_clock_pll_init(clk, 0, clk->config->property.div, 0, 5, clock_src->config->type);
//   }
//   else {
//     // todo: pll disable
//     HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLPSC, RCU_PLLPSC_DIV_MAX);
//     return HDL_MODULE_UNLOADED;
//   }
// }

// static hdl_module_state_t _hdl_clock_pll_vco(hdl_clock_private_t *clk, uint8_t enable) {
//   if(enable) {
//     uint32_t ss_modulation_inc = 0U;
//     uint32_t ss_modulation_reg = RCU_PLLSSCTL;
//     /* calculate the minimum factor of PLLN */
//     if((ss_modulation_reg & RCU_PLLSSCTL_SSCGON) == RCU_PLLSSCTL_SSCGON) {
//         if((ss_modulation_reg & RCU_SS_TYPE_DOWN) == RCU_SS_TYPE_DOWN) {
//             ss_modulation_inc += RCU_SS_MODULATION_DOWN_INC;
//         } else {
//             ss_modulation_inc += RCU_SS_MODULATION_CENTER_INC;
//         }
//     }
//     if(CHECK_PLL_N_VALID(clk->config->property.mul, ss_modulation_inc))
//       if(_hdl_clock_pll_init(clk, -1, (clk->config->property.mul << 6), 6, 14, HDL_CLOCK_TYPE_PLL_SEL) == HDL_MODULE_ACTIVE)
//         return _hdl_clock_osc_en(RCU_PLL_CK, RCU_FLAG_PLLSTB, PLL_STARTUP_TIMEOUT);
//   }
//   HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLN, RCU_PLLN_MUL_MIN << 6);
//   return HDL_MODULE_UNLOADED;
// }

static void _unlock_reg() {
    SYS->REGLCTL = 0x59;
    SYS->REGLCTL = 0x16;
    SYS->REGLCTL = 0x88;
}

static void _lock_reg() {
    SYS->REGLCTL = 0;
}

static hdl_module_state_t _hdl_clock_osc_en(uint32_t osc, uint32_t stat, uint32_t timeout) {
  _unlock_reg();
  HDL_REG_SET(CLK->PWRCTL, osc);
  _lock_reg();
  uint32_t stb_timer = timeout;
  while ((!HDL_REG_CHECK(CLK->STATUS, stat)) && (stb_timer--)); /* Wait until HXTAL will be stable */
  if (!HDL_REG_CHECK(CLK->STATUS, stat))
    return HDL_MODULE_FAULT;
  return HDL_MODULE_ACTIVE;
}

hdl_module_state_t hdl_clock(void *desc, uint8_t enable) {
  hdl_clock_private_t *clk = (hdl_clock_private_t *)desc;
  clk->freq.denom = 1;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_HXTAL:
      
      clk->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HXTEN_Msk, CLK_STATUS_HXTSTB_Msk, HXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_LXTAL:
      clk->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_LXTEN_Msk, CLK_STATUS_LXTSTB_Msk, LXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC12M:
      clk->freq.num = 12000000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HIRCEN_Msk, CLK_STATUS_HIRCSTB_Msk, IRC12M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_IRC48M:
      clk->freq.num = 48000000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HIRC48MEN_Msk, CLK_STATUS_HIRC48MSTB_Msk, IRC48M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC10K:
      clk->freq.num = 10000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_LIRCEN_Msk, CLK_STATUS_LIRCSTB_Msk, IRC10K_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;


    // case HDL_CLOCK_TYPE_PLL_SEL:
    //   return _hdl_clock_selector_pll(clk, enable);

    // case HDL_CLOCK_TYPE_PLL_VCO:
    //   return _hdl_clock_pll_vco(clk, enable);

    // case HDL_CLOCK_TYPE_PLL_P:
    //   if(enable && CHECK_PLL_P_VALID(clk->config->property.div))
    //     return _hdl_clock_pll_init(clk, 0, (((clk->config->property.div >> 1) - 1U) << 16), 16, 17, HDL_CLOCK_TYPE_PLL_VCO);
    //   return HDL_MODULE_UNLOADED;

    // case HDL_CLOCK_TYPE_PLL_Q:
    //   if(enable && CHECK_PLL_Q_VALID(clk->config->property.div))
    //     return _hdl_clock_pll_init(clk, 0, (clk->config->property.div << 24), 24, 27, HDL_CLOCK_TYPE_PLL_VCO);
    //   return HDL_MODULE_UNLOADED;

    // case HDL_CLOCK_TYPE_SYS_SEL:
    //   return _hdl_clock_system(clk, enable);

    // case HDL_CLOCK_TYPE_AHB:
    //   if (enable) {
    //     return _hdl_bus_clock_cnf(clk, 4, 7, MAX_SYS_CLOCK, HDL_CLOCK_TYPE_SYS_SEL);
    //   }
    //   rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV512);
    //   return HDL_MODULE_UNLOADED;

    // case HDL_CLOCK_TYPE_APB1:
    //   if (enable) return _hdl_bus_clock_cnf(clk, 10, 12, APB1_MAX_FREQ, HDL_CLOCK_TYPE_AHB);
    //   rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV16);
    //   return HDL_MODULE_UNLOADED;

    // case HDL_CLOCK_TYPE_APB2:
    //   if (enable) return _hdl_bus_clock_cnf(clk, 13, 15, APB2_MAX_FREQ, HDL_CLOCK_TYPE_AHB);
    //   rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV16);
    //   return HDL_MODULE_UNLOADED;

    default:
      break;
  }
  return HDL_MODULE_FAULT;
}

void hdl_get_clock(hdl_clock_t *clock, hdl_clock_freq_t *freq) {
  if(freq != NULL) {
    freq->num = 0;
    freq->denom = 1;
    hdl_clock_private_t *clk = (hdl_clock_private_t *)clock;
    if((clk != NULL) && (hdl_state(&clk->module) != HDL_MODULE_FAULT)) {
      freq->num = clk->freq.num;
      freq->denom = clk->freq.denom;
    }
  }
}
