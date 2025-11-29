#include "hdl_iface.h"
#include "hdl_sd_def.h"

typedef enum {
  SD_READ,
  SD_WRITE,
  SD_ERASE
} hdl_sd_op_mode_t;

typedef struct {
  hdl_coroutine_t worker;
  uint32_t wrk_ccb_buffer[1024];
  uint32_t timer;
  struct {
    uint64_t capacity;
    uint32_t block_size;
    uint32_t rca;
    uint8_t max_bus_clk;
    uint8_t sdhc       :1,
            wr_protect :1;
  } card;
  
  hdl_nvm_message_t *nvm_msg;
  hdl_sd_op_mode_t op_mode;
  uint8_t state;
  hdl_sdio_cmd_message_t cmd_msg;
  hdl_sdio_data_message_t data_msg;
} hdl_sd_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sd_var_t, *((hdl_sd_t *)0)->obj_var, HDL_SD_VAR_SIZE, "hdl_sd.h");

static uint8_t _sdio_send_cmd(hdl_sd_t *sd, uint8_t *cmd, uint32_t arg, uint32_t **resp) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  sd_var->cmd_msg.cmd = *cmd;
  sd_var->cmd_msg.argument = arg;
  hdl_sdio_cmd_transfer(sdio, &sd_var->cmd_msg);
  while(!(sd_var->cmd_msg.status & HDL_SDIO_STATE_COMPLETE))
    yield_return;
  *cmd = sd_var->cmd_msg.cmd;
  *resp = sd_var->cmd_msg.response;
  return sd_var->cmd_msg.status & HDL_SDIO_ERROR;
}

static void _delay(hdl_time_counter_t *tc, uint32_t ms) {
  uint32_t time = hdl_time_counter_get(tc);
  uint32_t now;
  do {
    yield_return;
    now = hdl_time_counter_get(tc);
  } while (!CL_TIME_ELAPSED(time, ms, now));
}

static void _sd_parse_csd(hdl_sd_var_t *sd_var, uint32_t *csd) {
  //sd_var->card.CSDStruct = (uint8_t)((csd[0] & 0xC0000000U) >> 30U);
  //sd_var->card.SysSpecVersion = (uint8_t)((csd[0] & 0x3C000000U) >> 26U);
  //sd_var->card.TAAC = (uint8_t)((csd[0] & 0x00FF0000U) >> 16U);
  //sd_var->card.NSAC = (uint8_t)((csd[0] & 0x0000FF00U) >> 8U);
  uint32_t rd_block_len = 1 << ((csd[1] & 0x000F0000U) >> 16U);
  sd_var->card.max_bus_clk = (uint8_t)(csd[0] & 0x000000FFU);
  //sd_var->card.CardComdClasses = (uint16_t)((csd[1] & 0xFFF00000U) >> 20U);
  //sd_var->card.PartBlockRead   = (uint8_t)((csd[1] & 0x00008000U) >> 15U);
  //sd_var->card.WrBlockMisalign = (uint8_t)((csd[1] & 0x00004000U) >> 14U);
  //sd_var->card.RdBlockMisalign = (uint8_t)((csd[1] & 0x00002000U) >> 13U);
  //sd_var->card.DSRImpl = (uint8_t)((csd[1] & 0x00001000U) >> 12U);
  uint32_t block_nbr = 0;
  if(sd_var->card.sdhc) {
    uint32_t device_size = (((csd[1] & 0x0000003FU) << 16U) | ((csd[2] & 0xFFFF0000U) >> 16U)) + 1U;
    block_nbr = (device_size * 1024U);
    sd_var->card.block_size = 512U;
  }
  else { /* SDSC */
    //sd_var->card.MaxRdCurrentVDDMin = (uint8_t)((csd[2] & 0x38000000U) >> 27U);
    //sd_var->card.MaxRdCurrentVDDMax = (uint8_t)((csd[2] & 0x07000000U) >> 24U);
    //sd_var->card.MaxWrCurrentVDDMin = (uint8_t)((csd[2] & 0x00E00000U) >> 21U);
    //sd_var->card.MaxWrCurrentVDDMax = (uint8_t)((csd[2] & 0x001C0000U) >> 18U);
    uint32_t device_size = (((csd[1] & 0x000003FFU) << 2U) | ((csd[2] & 0xC0000000U) >> 30U)) + 1U;
    uint32_t device_size_mul = (1UL << (((uint8_t)((csd[2] & 0x00038000U) >> 15U) & 0x07U) + 2U));
    block_nbr = device_size * device_size_mul;
    sd_var->card.block_size = rd_block_len;
  }
  sd_var->card.capacity = (uint64_t)block_nbr * (uint64_t)sd_var->card.block_size;

  //sd_var->card.EraseGrSize = (uint8_t)((csd[2] & 0x00004000U) >> 14U);
  //sd_var->card.EraseGrMul = (uint8_t)((csd[2] & 0x00003F80U) >> 7U);
  //sd_var->card.WrProtectGrSize = (uint8_t)(csd[2] & 0x0000007FU);
  //sd_var->card.WrProtectGrEnable = (uint8_t)((csd[3] & 0x80000000U) >> 31U);
  //sd_var->card.ManDeflECC = (uint8_t)((csd[3] & 0x60000000U) >> 29U);
  //sd_var->card.WrSpeedFact = (uint8_t)((csd[3] & 0x1C000000U) >> 26U);
  //uint32_t max_wr_block_len = 1 << ((csd[3] & 0x03C00000U) >> 22U);
  //sd_var->card.WriteBlockPaPartial = (uint8_t)((csd[3] & 0x00200000U) >> 21U);
  //sd_var->card.ContentProtectAppli = (uint8_t)((csd[3] & 0x00010000U) >> 16U);
  //sd_var->card.FileFormatGroup = (uint8_t)((csd[3] & 0x00008000U) >> 15U);
  //sd_var->card.CopyFlag = (uint8_t)((csd[3] & 0x00004000U) >> 14U);
  uint8_t perm_wr_protect = (uint8_t)((csd[3] & 0x00002000U) >> 13U);
  uint8_t temp_wr_protect = (uint8_t)((csd[3] & 0x00001000U) >> 12U);
  sd_var->card.wr_protect = perm_wr_protect || temp_wr_protect;
  //sd_var->card.FileFormat = (uint8_t)((csd[3] & 0x00000C00U) >> 10U);
  //sd_var->card.ECC = (uint8_t)((csd[3] & 0x00000300U) >> 8U);
  //sd_var->card.CSD_CRC = (uint8_t)((csd[3] & 0x000000FEU) >> 1U);
}

static uint8_t init_sd(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  uint32_t *responce;
  uint8_t cmd;
  uint8_t err;

  cmd = SDMMC_CMD_GO_IDLE_STATE | HDL_SDIO_CMD_RESPONSE_NONE;
  while(_sdio_send_cmd(sd, &cmd, 0, &responce));
  _delay(tc, 10);
  cmd = SDMMC_CMD_HS_SEND_EXT_CSD | HDL_SDIO_CMD_RESPONSE_SHORT;
  err = _sdio_send_cmd(sd, &cmd, SDMMC_CHECK_PATTERN, &responce);

  if(err || (responce[0] & SDMMC_CHECK_PATTERN) != SDMMC_CHECK_PATTERN) 
    return HDL_FALSE; /* Not 2.0 Spec */
  uint32_t retry = 30;
  do {
    _delay(tc, 40);
    cmd = SDMMC_CMD_APP_CMD | HDL_SDIO_CMD_RESPONSE_SHORT;
    if((!_sdio_send_cmd(sd, &cmd, 0, &responce)) && (responce[0] & HDL_SD_STA_APP_CMD)) {
      _delay(tc, 5);
      cmd = HDL_SD_ACMD41 | HDL_SDIO_CMD_RESPONSE_SHORT;
      err = _sdio_send_cmd(sd, &cmd, 0x40FF8000,
        //HDL_SD_ACMD41_ARG_HIGH_CAPACITY | HDL_SD_OCR_VW_3V2_3V3, 
        &responce);
      err = err & ~HDL_SDIO_ERROR_CRC; /* there is no CRC in R3 responce */
      uint32_t exp_options = HDL_SD_ACMD41_RESP_BUSY | HDL_SD_OCR_VW_3V2_3V3;
      if(!err && ((responce[0] & exp_options) == exp_options)) {
        sd_var->card.sdhc = ((responce[0] & HDL_SD_ACMD41_RESP_HIGH_CAPACITY) != 0);
        break;
      }
    }
  } while (!err && retry--);
  if(err) 
    return HDL_FALSE;


  cmd = SDMMC_CMD_ALL_SEND_CID | HDL_SDIO_CMD_RESPONSE_LONG;
  if (_sdio_send_cmd(sd, &cmd, 0, &responce))
    return HDL_FALSE;

  cmd = SDMMC_CMD_SET_REL_ADDR | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (_sdio_send_cmd(sd, &cmd, 0, &responce))
    return HDL_FALSE;
  sd_var->card.rca = responce[0] & 0xffff0000; /* RCA [31:16] */

  cmd = SDMMC_CMD_SEND_CSD | HDL_SDIO_CMD_RESPONSE_LONG;
  if (_sdio_send_cmd(sd, &cmd, sd_var->card.rca, &responce))
    return HDL_FALSE;

  _sd_parse_csd(sd_var, responce);

  cmd = SDMMC_CMD_SEL_DESEL_CARD | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (_sdio_send_cmd(sd, &cmd, sd_var->card.rca, &responce))
    return HDL_FALSE;

  cmd = SDMMC_CMD_SEND_STATUS | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (_sdio_send_cmd(sd, &cmd, sd_var->card.rca, &responce) || 
    (!(responce[0] & HDL_SD_STA_READY_FOR_DATA)))
      return HDL_FALSE; /* cmd err or !READY_FOR_DATA */


  cmd = SDMMC_CMD_APP_CMD | HDL_SDIO_CMD_RESPONSE_SHORT;
  if((!_sdio_send_cmd(sd, &cmd, sd_var->card.rca, &responce)) && (responce[0] & HDL_SD_STA_APP_CMD)) {
    _delay(tc, 5);
    cmd = SDMMC_CMD_APP_SD_SET_BUSWIDTH | HDL_SDIO_CMD_RESPONSE_SHORT;
    if(!_sdio_send_cmd(sd, &cmd, 2, &responce)) /* '00' = 1 bit or '10' = 4 bits bus */
      hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_4);
  }
  hdl_sdio_set_clock(sdio, sd_var->card.max_bus_clk * 1000000);      
  return HDL_TRUE;
}

#define SD_BLOCK_SIZE        512

static uint8_t sd_read_write(hdl_sd_t *sd, uint8_t read) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  uint32_t *responce;
  uint8_t cmd;
  uint32_t offset = 0;
  uint8_t retry = 10;
  uint32_t addr = sd_var->nvm_msg->address / SD_BLOCK_SIZE;
  do {
    sd_var->data_msg.data = &((uint8_t *)sd_var->nvm_msg->data)[offset * SD_BLOCK_SIZE];
    sd_var->data_msg.data_block_size = SD_BLOCK_SIZE;
    sd_var->data_msg.data_length = SD_BLOCK_SIZE;
    sd_var->data_msg.dir = read? HDL_SDIO_DATA_FROM_DEV: HDL_SDIO_DATA_TO_DEV;
    sd_var->data_msg.mode = HDL_SDIO_DATA_BLOCK;
    sd_var->data_msg.timeout = 1000;
    if(read)
      hdl_sdio_data_transfer(sdio, &sd_var->data_msg);

    cmd = (read? SDMMC_CMD_READ_SINGLE_BLOCK: SDMMC_CMD_WRITE_SINGLE_BLOCK) | HDL_SDIO_CMD_RESPONSE_SHORT;
    if (_sdio_send_cmd(sd, &cmd, addr + offset, &responce)) {
      _delay(tc, 5);
      if(retry--) continue;
      sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
      break;
    }

    if(!read) {
      if(responce[0] & HDL_SD_STA_READY_FOR_DATA)
        hdl_sdio_data_transfer(sdio, &sd_var->data_msg);
      else {
        _delay(tc, 5);
        if(retry--) continue;
        sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
        break;
      }
    }

    while(!(sd_var->data_msg.status & HDL_SDIO_STATE_COMPLETE))
      yield_return;
    
    if(sd_var->data_msg.status & HDL_SDIO_ERROR) {
      if(sd_var->data_msg.status & HDL_SDIO_ERROR_INTERNAL)
        sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_INTERNAL_FAULT;
      else
        sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
      break;
    }
    //cmd = SDMMC_CMD_SEND_STATUS | HDL_SDIO_CMD_RESPONSE_SHORT;
    //uint8_t err = _sdio_send_cmd(sd, &cmd, sd_var->card.rca, &responce);
    sd_var->nvm_msg->synced_size += SD_BLOCK_SIZE;
    offset++;
  } while (sd_var->nvm_msg->size > (offset * SD_BLOCK_SIZE));
  sd_var->nvm_msg->out_status &= ~HDL_NVM_STATE_BUSY;
  sd_var->nvm_msg->out_status |= HDL_NVM_STATE_COMPLETE;
  uint32_t xfer_size = sd_var->nvm_msg->size;
  sd_var->nvm_msg = NULL;
  return !(sd_var->nvm_msg->out_status & HDL_NVM_ERROR);
}

static void _sd_reset(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_gpio_pin_t *pw_pin = (hdl_gpio_pin_t *)sd->dependencies[3];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  sd_var->card.capacity = 0;
  sd_var->card.rca = 0;
  hdl_gpio_set_inactive(pw_pin);
  hdl_sdio_set_clock(sdio, 200000);
  hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_1);
  if(sd_var->nvm_msg != NULL)
    sd_var->nvm_msg->out_status = HDL_NVM_STATE_CANCELED | HDL_NVM_STATE_COMPLETE;
  sd_var->nvm_msg = NULL;
}

#define SD_STATE_NO_CARD         0
#define SD_STATE_POWER_UP_DELAY  1
#define SD_STATE_INIT            2
#define SD_STATE_READY           3
#define SD_STATE_UNUSABLE        4

static uint8_t _sd_worker(coroutine_t *this, uint8_t cancel, void *context) {
  (void)this;
  hdl_sd_t *sd = (hdl_sd_t *)context;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  hdl_gpio_pin_t *cd_pin = (hdl_gpio_pin_t *)sd->dependencies[2];
  hdl_gpio_pin_t *pw_pin = (hdl_gpio_pin_t *)sd->dependencies[3];

  uint32_t now = hdl_time_counter_get(tc);
  if(sd_var->state == SD_STATE_NO_CARD) {
    if(hdl_gpio_is_active(cd_pin)) {
      hdl_gpio_set_active(pw_pin);
      sd_var->timer = now;
      sd_var->state = SD_STATE_POWER_UP_DELAY;
    }
  }
  else if(sd_var->state == SD_STATE_POWER_UP_DELAY) {
    if(CL_TIME_ELAPSED(sd_var->timer, sd->config->power_up_delay, now))
      sd_var->state = SD_STATE_INIT;
  }
  else if(sd_var->state == SD_STATE_INIT) {
    if(init_sd(sd))
      sd_var->state = SD_STATE_READY;
    else 
      sd_var->state = SD_STATE_UNUSABLE;
    sd_var->timer = now;
  }
  else if (sd_var->state == SD_STATE_READY) {
    if(sd_var->nvm_msg != NULL) {
      if(!sd_read_write(sd, (sd_var->op_mode == SD_READ)))
        sd_var->state = SD_STATE_UNUSABLE;
    }
  }
  else if (sd_var->state == SD_STATE_UNUSABLE) {
    if(CL_TIME_ELAPSED(sd_var->timer, 1000, now)) {
      _sd_reset(sd);
      sd_var->state = SD_STATE_NO_CARD;
    }
  }
  if(hdl_gpio_is_inactive(cd_pin)) {
    _sd_reset(sd);
    sd_var->state = SD_STATE_NO_CARD;
  }
  return cancel;
}

static hdl_module_state_t _hdl_sd(const void *desc, uint8_t enable) {
  hdl_sd_t *sd = (hdl_sd_t *)desc;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  if(enable) {
    _sd_reset(sd);
    sd_var->state = 0;
    hdl_coroutine_add_yielding(&sd_var->worker, sd_var->wrk_ccb_buffer, sizeof(sd_var->wrk_ccb_buffer), _sd_worker, sd);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&sd_var->worker.coroutine);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_sd_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_sd_t *sd = (hdl_sd_t *)desc;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  if(out_info != NULL) {
    out_info->sector_size = sd_var->card.block_size;
    out_info->mtu_size = sd_var->card.block_size;
    out_info->volume = sd_var->card.capacity;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_sd_rw(const void *desc, hdl_nvm_message_t *message, hdl_sd_op_mode_t op_mode) {
  hdl_sd_t *sd = (hdl_sd_t *)desc;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  if((message != NULL) && (sd_var->state == SD_STATE_READY) && (sd_var->nvm_msg == NULL)) {
    if(message->address & (sd_var->card.block_size - 1))
      message->out_status = HDL_NVM_ERROR_SECTOR_UNALIGNED | HDL_NVM_STATE_COMPLETE;
    else if((message->address + message->size) >= sd_var->card.capacity)
      message->out_status = HDL_NVM_ERROR_OUT_OF_RANGE | HDL_NVM_STATE_COMPLETE;
    else if(op_mode == SD_ERASE) 
      message->out_status = HDL_NVM_STATE_COMPLETE; /* we don't need it for SD */
    else if((op_mode == SD_WRITE) && (sd_var->card.wr_protect))
      message->out_status = HDL_NVM_ERROR_LOCKED | HDL_NVM_STATE_COMPLETE;
    else {
      sd_var->op_mode = op_mode;
      sd_var->nvm_msg = message;
      message->out_status = HDL_NVM_STATE_BUSY;
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_sd_read(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_sd_rw(desc, message, SD_READ);
}

static uint8_t _hdl_sd_write(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_sd_rw(desc, message, SD_WRITE);
}

static uint8_t _hdl_sd_erase(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_sd_rw(desc, message, SD_ERASE);
}

static uint8_t _hdl_sd_cancel(const void *desc) {
  (void)desc;
  return HDL_FALSE;
}

const hdl_nvm_iface_t hdl_sd_mcu_iface = {
  .init = &_hdl_sd,
  .info = &_hdl_sd_info_get,
  .read = &_hdl_sd_read,
  .write = &_hdl_sd_write,
  .erase = &_hdl_sd_erase,
  .cancel = &_hdl_sd_cancel
};
