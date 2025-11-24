#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_sdio_cmd_message_t *cmd_msg;
  hdl_sdio_data_message_t *data_msg;
  uint8_t proc_state;
} hdl_sdio_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sdio_mcu_var_t, *((hdl_sdio_mcu_t *)0)->obj_var, HDL_SDIO_MCU_VAR_SIZE, port_sd.h);

static uint8_t _sdio_worker(coroutine_t *this, uint8_t cancel, void *arg);

#define SDIO_R1_RESP_STATUS(sdio)   (sdio->RESP1)
#define SDIO_R1_RESP_CMD(sdio)      ((uint8_t)(sdio->RESPCMD))
#define SDIO_R3_RESP_STATUS(sdio)   (sdio->RESP1)

static uint8_t _sdio_process_data(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)arg;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  (void)sdio_var; (void)phy;
  return cancel;
}

static uint8_t _sdio_process_cmd(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)arg;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(sdio_var->proc_state == 0) {
    uint32_t tmpreg = sdio_var->cmd_msg->cmd | SDIO_CPSM_ENABLE | SDIO_WAIT_NO;
    // uint32_t tmpreg = (sdio_var->cmd_msg->cmd & 0x3F) | SDIO_CPSM_ENABLE | SDIO_WAIT_NO;
    // switch (sdio_var->cmd_msg->cmd & HDL_SDIO_CMD_RESPONSE_MASK) {
    //   case HDL_SDIO_CMD_RESPONSE_LONG: tmpreg |= SDIO_RESPONSE_SHORT; break;
    //   case HDL_SDIO_CMD_RESPONSE_SHORT: tmpreg |= SDIO_RESPONSE_LONG; break;
    //   default: tmpreg |= SDIO_RESPONSE_NO; break;
    // }
    phy->ARG = sdio_var->cmd_msg->argument;
    MODIFY_REG(phy->CMD, CMD_CLEAR_MASK, tmpreg);
    sdio_var->proc_state = 1;
  }
  if(sdio_var->proc_state == 1) {
    uint32_t sta = phy->STA;
    if((sta & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) || /* <- ????*/
       (!(sta & SDIO_FLAG_CMDACT))) {
      if(sta & SDIO_FLAG_CTIMEOUT)
        sdio_var->cmd_msg->status |= HDL_SDIO_ERROR_TIMEOUT;
      if(sta & SDIO_FLAG_CCRCFAIL)
        sdio_var->cmd_msg->status |= HDL_SDIO_ERROR_CRC;
      phy->ICR = SDIO_STATIC_CMD_FLAGS;
      CL_REG_MODIFY(sdio_var->cmd_msg->cmd, HDL_SDIO_CMD_MASK, phy->RESPCMD);
      switch (sdio_var->cmd_msg->cmd & HDL_SDIO_CMD_RESPONSE_MASK) {
        case HDL_SDIO_CMD_RESPONSE_SHORT: 
          sdio_var->cmd_msg->response[0] = phy->RESP1; 
          break;
        case HDL_SDIO_CMD_RESPONSE_LONG:
          sdio_var->cmd_msg->response[0] = phy->RESP1;
          sdio_var->cmd_msg->response[1] = phy->RESP2;
          sdio_var->cmd_msg->response[2] = phy->RESP3;
          sdio_var->cmd_msg->response[3] = phy->RESP4;
        /* fall throw */
        default: break;
      }
      sdio_var->cmd_msg->status &= ~HDL_SDIO_STATE_BUSY;
      sdio_var->cmd_msg->status |= HDL_SDIO_STATE_COMPLETE;
      sdio_var->cmd_msg = NULL;
      coroutine_set_handler(this, &_sdio_worker);
    }
  }
  return cancel;
}

static uint8_t _sdio_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)arg;
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)arg;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  sdio_var->proc_state = 0;
  if(sdio_var->cmd_msg != NULL)
    coroutine_set_handler(this, &_sdio_process_cmd);
  else if(sdio_var->data_msg != NULL)
    coroutine_set_handler(this, &_sdio_process_data);
  return cancel;
}

static hdl_module_state_t _hdl_sdio(const void *desc, uint8_t enable) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(enable) {
    CL_REG_SET(RCC->APB2ENR, sdio->config->rcu);
    /* Check the parameters */
    if(!(IS_SDIO_ALL_INSTANCE(phy) &&
        IS_SDIO_CLOCK_EDGE(sdio->config->clock_edge) &&
        IS_SDIO_CLOCK_BYPASS(sdio->config->clock_bypass) &&
        IS_SDIO_CLOCK_POWER_SAVE(sdio->config->clock_power_save) &&
        IS_SDIO_HARDWARE_FLOW_CONTROL(sdio->config->hardware_flow_control)))
      return HDL_MODULE_FAULT;
    hdl_clock_freq_t sdio_f;
    hdl_clock_get(sdio->dependencies[0], &sdio_f);
    uint32_t tmp = (sdio_f.num / 400000) / sdio_f.denom;
    if(tmp < 2) tmp = 0;
    else tmp = (tmp - 2) & 0xff;
    uint32_t tmpreg = (sdio->config->clock_edge   |\
              sdio->config->clock_bypass          |\
              sdio->config->clock_power_save      |\
              SDIO_BUS_WIDE_1B                    |\
              sdio->config->hardware_flow_control |\
              (tmp << SDIO_CLKCR_CLKDIV_Pos));
    CL_REG_MODIFY(phy->CLKCR, CLKCR_CLEAR_MASK, tmpreg);
    tmp = 1000;
    while (tmp--);
    __SDIO_DISABLE();
    CL_REG_SET(phy->POWER, SDIO_POWER_PWRCTRL);
    __SDIO_ENABLE();
    tmp = (sdio_f.num / 8000) / sdio_f.denom;
    while (tmp--);
    sdio_var->cmd_msg = NULL;
    sdio_var->data_msg = NULL;
    coroutine_add(&sdio_var->worker, &_sdio_worker, sdio);
    return  HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&sdio_var->worker);
  CL_REG_SET(phy->POWER, (uint32_t)0x00000000);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_sdio_cmd_transfer(const void *desc, hdl_sdio_cmd_message_t *message) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  if((sdio_var->data_msg != NULL) || (message == NULL)) 
    return HDL_FALSE;
  sdio_var->cmd_msg = message;
  sdio_var->cmd_msg->status = HDL_SDIO_STATE_BUSY;
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_data_transfer(const void *desc, hdl_sdio_data_message_t *message) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  if((sdio_var->data_msg != NULL) || (message == NULL)) 
    return HDL_FALSE;
  sdio_var->data_msg = message;
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_set_bus(const void *desc, hdl_sdio_bus_width_t width) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  uint32_t tmpreg = SDIO_BUS_WIDE_1B;
  if(width == HDL_SDIO_BUS_WIDTH_4) tmpreg = SDIO_BUS_WIDE_4B;
  if(width == HDL_SDIO_BUS_WIDTH_8) tmpreg = SDIO_BUS_WIDE_8B;
  CL_REG_MODIFY(phy->CLKCR, SDIO_BUS_WIDE_1B | SDIO_BUS_WIDE_4B | SDIO_BUS_WIDE_8B, tmpreg);
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_set_clock(const void *desc, uint32_t speed) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(speed == 0) return HDL_FALSE;
  hdl_clock_freq_t sdio_f;
  hdl_clock_get(sdio->dependencies[0], &sdio_f);
  uint32_t div = (sdio_f.num / speed) / sdio_f.denom;
  if(div < 2) div = 0;
  else div = (div - 2) & 0xff;
  CL_REG_MODIFY(phy->CLKCR, SDIO_CLKCR_CLKDIV, div << SDIO_CLKCR_CLKDIV_Pos);
  return HDL_TRUE;
}

const hdl_sdio_iface_t hdl_sdio_mcu_iface = {
  .init = &_hdl_sdio,
  .cmd = &_hdl_sdio_cmd_transfer,
  .data = &_hdl_sdio_data_transfer,
  .set_bus = &_hdl_sdio_set_bus,
  .set_clock = &_hdl_sdio_set_clock
};
