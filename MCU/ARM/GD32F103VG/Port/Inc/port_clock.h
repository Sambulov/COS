#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

hdl_module_state_t hdl_clock_hxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_lxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc8m(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc40k(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_hxtal_prescaler(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_pll(void *desc, uint8_t enable);

#ifdef GD32F10X_CL
hdl_module_state_t hdl_clock_pll1(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_pll2(void *desc, uint8_t enable);
#endif /* GD32F10X_CL */

hdl_module_state_t hdl_clock_system(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_ahb(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb1(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb2(void *desc, uint8_t enable);

#endif /* PORT_CLOCK_H_ */
