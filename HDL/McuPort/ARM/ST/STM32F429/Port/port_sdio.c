#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_sdio_cmd_message_t *cmd_msg;
  hdl_sdio_data_message_t *data_msg;
  uint8_t cmd_state  : 4;
  uint8_t data_state : 4;
  uint32_t sdio_ck; /* sdio_ck = Pf/(div+2) */
} hdl_sdio_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sdio_mcu_var_t, *((hdl_sdio_mcu_t *)0)->obj_var, HDL_SDIO_MCU_VAR_SIZE, port_sd.h);

static uint8_t _sdio_worker(coroutine_t *this, uint8_t cancel, void *arg);

#define SDIO_R1_RESP_STATUS(sdio)   (sdio->RESP1)
#define SDIO_R1_RESP_CMD(sdio)      ((uint8_t)(sdio->RESPCMD))
#define SDIO_R3_RESP_STATUS(sdio)   (sdio->RESP1)

static void _sdio_data_flow(hdl_sdio_mcu_t *sdio) {
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  const hdl_dma_channel_t *sdio_dma = (hdl_dma_channel_t *)sdio->dependencies[1];
  if(sdio_var->data_state == 0) {
    uint32_t dtctrl = SDIO_DCTRL_DTEN | SDIO_DCTRL_DMAEN;
    if(sdio_var->data_msg->mode == HDL_SDIO_DATA_BLOCK) {
      uint32_t bs = sdio_var->data_msg->data_block_size;
      if(!bs || (bs & (bs - 1)) || (sdio_var->data_msg->data_length & (bs - 1))) {
        /* block size not power of 2 or data_length unaligned */
        sdio_var->data_msg->status |= HDL_SDIO_ERROR_BAD_ARG;
        sdio_var->data_state = 2;
        return;
      }
      bs = 31 - __CLZ(sdio_var->data_msg->data_block_size);
  		dtctrl |= ((bs << SDIO_DCTRL_DBLOCKSIZE_Pos) & SDIO_DCTRL_DBLOCKSIZE_Msk);
    }
    phy->DLEN = sdio_var->data_msg->data_length;

    if(sdio_var->data_msg->timeout) {
      hdl_clock_freq_t sdio_f;
      hdl_clock_get(sdio->dependencies[0], &sdio_f);
      uint32_t timeout = (sdio_var->sdio_ck * sdio_var->data_msg->timeout + 500) / 1000;
      phy->DTIMER = timeout;
    }

    if(sdio_var->data_msg->dir == HDL_SDIO_DATA_TO_DEV) {
      /* amout = 0, data transfer controled by SDIO */
      hdl_dma_channel_run_m2p(sdio_dma, (void *)&phy->FIFO, sdio_var->data_msg->data, 0);
      /* This is SDIO+DMA hardware bug workaround. */
      CL_REG_CLEAR(SDIO->CLKCR, SDIO_CLKCR_CLKEN); /* Disable SDIO_CK */
  		phy->DCTRL = dtctrl; /* Start transmission in DMA mode */
      while (SDIO->STA & SDIO_STA_TXFIFOHE); /* Wait FIFO half-full */
      CL_REG_SET(SDIO->CLKCR, SDIO_CLKCR_CLKEN); /* Enable SDIO_CK */
    }
    else {
      /* amout = 0, data transfer controled by SDIO */
      hdl_dma_channel_run_p2m(sdio_dma, (void *)&phy->FIFO, sdio_var->data_msg->data, 0);
      phy->DCTRL = dtctrl | SDIO_DCTRL_DTDIR;
    }
    uint32_t delay = 1000;
    while (delay--); /* delay for sdio fsm */
    sdio_var->data_state = 1;
  }
  if(sdio_var->data_state == 1) {
    uint32_t sta = phy->STA;
    if(!(sta & (SDIO_STA_RXACT | SDIO_STA_TXACT)) || (sta & (SDIO_STA_RXOVERR | SDIO_STA_TXUNDERR))) {
      hdl_dma_channel_stop(sdio_dma);
      if((sta & SDIO_FLAG_DTIMEOUT) && !((sta & SDIO_FLAG_DATAEND) && (sta && SDIO_FLAG_DBCKEND)))
        sdio_var->data_msg->status |= HDL_SDIO_ERROR_TIMEOUT;
      if(sta & SDIO_FLAG_DCRCFAIL)
        sdio_var->data_msg->status |= HDL_SDIO_ERROR_CRC;
      if(sta & (SDIO_FLAG_TXUNDERR | SDIO_FLAG_RXOVERR))
        sdio_var->data_msg->status |= HDL_SDIO_ERROR_INTERNAL;
      phy->ICR = SDIO_STATIC_DATA_FLAGS | SDIO_FLAG_STBITERR;
      CL_REG_CLEAR(phy->DCTRL, SDIO_DCTRL_DTEN);
      sdio_var->data_msg->transferred = hdl_dma_channel_get_counter(sdio_dma);
      sdio_var->data_state = 2;
    }
  }
  if(sdio_var->data_state == 2) {
    sdio_var->data_msg->status &= ~HDL_SDIO_STATE_BUSY;
    sdio_var->data_msg->status |= HDL_SDIO_STATE_COMPLETE;
    sdio_var->data_msg = NULL;
    sdio_var->data_state = 0;
  }
}

static void _sdio_cmd_flow(hdl_sdio_mcu_t *sdio) {
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(sdio_var->cmd_state == 0) {
    uint32_t tmpreg = sdio_var->cmd_msg->cmd | SDIO_CPSM_ENABLE | SDIO_WAIT_NO;
    phy->ARG = sdio_var->cmd_msg->argument;
    MODIFY_REG(phy->CMD, CMD_CLEAR_MASK, tmpreg);
    sdio_var->cmd_state = 1;
  }
  if(sdio_var->cmd_state == 1) {
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
      sdio_var->cmd_state = 0;
    }
  }
}

static uint8_t _sdio_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)arg;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  if(sdio_var->data_msg != NULL)
    _sdio_data_flow(sdio);
  if(sdio_var->cmd_msg != NULL)
    _sdio_cmd_flow(sdio);
  return cancel;
}

static uint8_t _hdl_sdio_set_clock(const void *desc, uint32_t speed) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(speed == 0) return HDL_FALSE;
  hdl_clock_freq_t sdio_f;
  hdl_clock_get(sdio->dependencies[0], &sdio_f);
  uint32_t div = (sdio_f.num / speed) / sdio_f.denom;
  hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
  if(div < 2) div = 0;
  else div = (div - 2) & 0xff;
  sdio_var->sdio_ck = (sdio_f.num / (div + 2)) / sdio_f.denom;
  CL_REG_MODIFY(phy->CLKCR, SDIO_CLKCR_CLKDIV, div << SDIO_CLKCR_CLKDIV_Pos);
  return HDL_TRUE;
}

#define CLKCR_CONFIG_MASK                    ((uint32_t)(SDIO_CLKCR_PWRSAV |\
                                             SDIO_CLKCR_BYPASS  | SDIO_CLKCR_WIDBUS |\
                                             SDIO_CLKCR_NEGEDGE | SDIO_CLKCR_HWFC_EN))

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

    _hdl_sdio_set_clock(desc, sdio->config->init_speed);
    uint32_t tmpreg = (sdio->config->clock_edge   |\
              sdio->config->clock_bypass          |\
              sdio->config->clock_power_save      |\
              SDIO_BUS_WIDE_1B                    |\
              sdio->config->hardware_flow_control);
    CL_REG_MODIFY(phy->CLKCR, CLKCR_CONFIG_MASK, tmpreg);
    uint32_t delay = 1000;
    while (delay--);
    __SDIO_DISABLE();
    CL_REG_SET(phy->POWER, SDIO_POWER_PWRCTRL);
    __SDIO_ENABLE();
    hdl_clock_freq_t sdio_f;
    hdl_clock_get(sdio->dependencies[0], &sdio_f);
    delay = (sdio_f.num / 8000) / sdio_f.denom;
    while (delay--);
    sdio_var->cmd_msg = NULL;
    sdio_var->data_msg = NULL;
    sdio_var->cmd_state = 0;
    sdio_var->data_state = 0;
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
  if((sdio_var->cmd_msg != NULL) || (message == NULL)) 
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
  sdio_var->data_msg->status = HDL_SDIO_STATE_BUSY;
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_check_bus(const void *desc, hdl_sdio_bus_width_t width) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  if(!width || (width & (width - 1)) || (width & ~HDL_SDIO_BUS_WIDTH_ALL) || !(sdio->config->support_bw & width))
    return HDL_FALSE;
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_set_bus(const void *desc, hdl_sdio_bus_width_t width) {
  if(!_hdl_sdio_check_bus(desc, width)) 
    return HDL_FALSE;
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  uint32_t tmpreg = SDIO_BUS_WIDE_1B;
  if(width == HDL_SDIO_BUS_WIDTH_4)
    tmpreg = SDIO_BUS_WIDE_4B;
  else if(width == HDL_SDIO_BUS_WIDTH_8)
    tmpreg = SDIO_BUS_WIDE_8B;
  CL_REG_MODIFY(phy->CLKCR, SDIO_BUS_WIDE_1B | SDIO_BUS_WIDE_4B | SDIO_BUS_WIDE_8B, tmpreg);
  return HDL_TRUE;
}

static uint8_t _hdl_sdio_get_cnf(const void *desc, hdl_sdio_bus_width_t *out_width, uint32_t *out_speed) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  if(out_width != NULL) {
    SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
    uint32_t tmp = (phy->CLKCR & (SDIO_BUS_WIDE_1B | SDIO_BUS_WIDE_4B | SDIO_BUS_WIDE_8B));
    if(tmp == SDIO_BUS_WIDE_1B)
      *out_width = HDL_SDIO_BUS_WIDTH_1;
    else if(tmp == SDIO_BUS_WIDE_4B)
      *out_width = HDL_SDIO_BUS_WIDTH_4;
    else if(tmp == SDIO_BUS_WIDE_8B)
      *out_width = HDL_SDIO_BUS_WIDTH_8;
  }
  if(out_speed != NULL) {
    hdl_sdio_mcu_var_t *sdio_var = (hdl_sdio_mcu_var_t *)sdio->obj_var;
    *out_speed = sdio_var->sdio_ck;
  }
  return HDL_TRUE;
}

const hdl_sdio_iface_t hdl_sdio_mcu_iface = {
  .init = &_hdl_sdio,
  .cmd = &_hdl_sdio_cmd_transfer,
  .data = &_hdl_sdio_data_transfer,
  .set_bus = &_hdl_sdio_set_bus,
  .set_clock = &_hdl_sdio_set_clock,
  .check_bus = &_hdl_sdio_check_bus,
  .get_bus_clock = &_hdl_sdio_get_cnf
};
