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
  uint64_t capacity;
  hdl_nvm_message_t *nvm_msg;
  hdl_sd_op_mode_t op_mode;
  uint8_t state;
  hdl_sdio_cmd_message_t cmd_msg;
  hdl_sdio_data_message_t data_msg;
} hdl_sd_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sd_var_t, *((hdl_sd_t *)0)->obj_var, HDL_SD_VAR_SIZE, "hdl_sd.h");

static uint8_t sdio_send_cmd(hdl_sd_t *sd, uint8_t *cmd, uint32_t arg, uint32_t **resp) {
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

static void delay(hdl_time_counter_t *tc, uint32_t ms) {
  uint32_t time = hdl_time_counter_get(tc);
  uint32_t now;
  do {
    yield_return;
    now = hdl_time_counter_get(tc);
  } while (!CL_TIME_ELAPSED(time, ms, now));
}


// typedef struct {
//   uint32_t CardType;
//   uint32_t BlockNbr;
//   uint32_t BlockSize;
//   uint32_t LogBlockNbr;
//   uint32_t LogBlockSize;
// } sd_card_t;

// sd_card_t SdCard;


// void SD_GetCardCSD(uint32_t *CSD, HAL_SD_CardCSDTypeDef *pCSD)
// {
//   pCSD->CSDStruct = (uint8_t)((CSD[0] & 0xC0000000U) >> 30U);

//   pCSD->SysSpecVersion = (uint8_t)((CSD[0] & 0x3C000000U) >> 26U);

//   pCSD->Reserved1 = (uint8_t)((CSD[0] & 0x03000000U) >> 24U);

//   pCSD->TAAC = (uint8_t)((CSD[0] & 0x00FF0000U) >> 16U);

//   pCSD->NSAC = (uint8_t)((CSD[0] & 0x0000FF00U) >> 8U);

//   pCSD->MaxBusClkFrec = (uint8_t)(CSD[0] & 0x000000FFU);

//   pCSD->CardComdClasses = (uint16_t)((CSD[1] & 0xFFF00000U) >> 20U);

//   pCSD->RdBlockLen = (uint8_t)((CSD[1] & 0x000F0000U) >> 16U);

//   pCSD->PartBlockRead   = (uint8_t)((CSD[1] & 0x00008000U) >> 15U);

//   pCSD->WrBlockMisalign = (uint8_t)((CSD[1] & 0x00004000U) >> 14U);

//   pCSD->RdBlockMisalign = (uint8_t)((CSD[1] & 0x00002000U) >> 13U);

//   pCSD->DSRImpl = (uint8_t)((CSD[1] & 0x00001000U) >> 12U);

//   pCSD->Reserved2 = 0U; /*!< Reserved */

//   if(SdCard.CardType == CARD_SDSC)
//   {
//     pCSD->DeviceSize = (((CSD[1] & 0x000003FFU) << 2U) | ((CSD[2] & 0xC0000000U) >> 30U));

//     pCSD->MaxRdCurrentVDDMin = (uint8_t)((CSD[2] & 0x38000000U) >> 27U);

//     pCSD->MaxRdCurrentVDDMax = (uint8_t)((CSD[2] & 0x07000000U) >> 24U);

//     pCSD->MaxWrCurrentVDDMin = (uint8_t)((CSD[2] & 0x00E00000U) >> 21U);

//     pCSD->MaxWrCurrentVDDMax = (uint8_t)((CSD[2] & 0x001C0000U) >> 18U);

//     pCSD->DeviceSizeMul = (uint8_t)((CSD[2] & 0x00038000U) >> 15U);

//     SdCard.BlockNbr  = (pCSD->DeviceSize + 1U) ;
//     SdCard.BlockNbr *= (1UL << ((pCSD->DeviceSizeMul & 0x07U) + 2U));
//     SdCard.BlockSize = (1UL << (pCSD->RdBlockLen & 0x0FU));

//     SdCard.LogBlockNbr =  (SdCard.BlockNbr) * ((SdCard.BlockSize) / 512U);
//     SdCard.LogBlockSize = 512U;
//   }
//   else if(SdCard.CardType == CARD_SDHC_SDXC)
//   {
//     /* Byte 7 */
//     pCSD->DeviceSize = (((CSD[1] & 0x0000003FU) << 16U) | ((CSD[2] & 0xFFFF0000U) >> 16U));

//     SdCard.BlockNbr = ((pCSD->DeviceSize + 1U) * 1024U);
//     SdCard.LogBlockNbr = SdCard.BlockNbr;
//     SdCard.BlockSize = 512U;
//     SdCard.LogBlockSize = SdCard.BlockSize;
//   }
//   else
//   {
//     /* Clear all the static flags */
//     //__HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
//     //ErrorCode |= HAL_SD_ERROR_UNSUPPORTED_FEATURE;
//     //State = HAL_SD_STATE_READY;
//     return;// HAL_ERROR;
//   }

//   pCSD->EraseGrSize = (uint8_t)((CSD[2] & 0x00004000U) >> 14U);

//   pCSD->EraseGrMul = (uint8_t)((CSD[2] & 0x00003F80U) >> 7U);

//   pCSD->WrProtectGrSize = (uint8_t)(CSD[2] & 0x0000007FU);

//   pCSD->WrProtectGrEnable = (uint8_t)((CSD[3] & 0x80000000U) >> 31U);

//   pCSD->ManDeflECC = (uint8_t)((CSD[3] & 0x60000000U) >> 29U);

//   pCSD->WrSpeedFact = (uint8_t)((CSD[3] & 0x1C000000U) >> 26U);

//   pCSD->MaxWrBlockLen= (uint8_t)((CSD[3] & 0x03C00000U) >> 22U);

//   pCSD->WriteBlockPaPartial = (uint8_t)((CSD[3] & 0x00200000U) >> 21U);

//   pCSD->Reserved3 = 0;

//   pCSD->ContentProtectAppli = (uint8_t)((CSD[3] & 0x00010000U) >> 16U);

//   pCSD->FileFormatGroup = (uint8_t)((CSD[3] & 0x00008000U) >> 15U);

//   pCSD->CopyFlag = (uint8_t)((CSD[3] & 0x00004000U) >> 14U);

//   pCSD->PermWrProtect = (uint8_t)((CSD[3] & 0x00002000U) >> 13U);

//   pCSD->TempWrProtect = (uint8_t)((CSD[3] & 0x00001000U) >> 12U);

//   pCSD->FileFormat = (uint8_t)((CSD[3] & 0x00000C00U) >> 10U);

//   pCSD->ECC= (uint8_t)((CSD[3] & 0x00000300U) >> 8U);

//   pCSD->CSD_CRC = (uint8_t)((CSD[3] & 0x000000FEU) >> 1U);

//   pCSD->Reserved4 = 1;

//   return;// HAL_OK;
// }


static uint8_t init_sd(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  uint32_t *responce;
  uint8_t cmd;
  uint8_t err;

  cmd = SDMMC_CMD_GO_IDLE_STATE | HDL_SDIO_CMD_RESPONSE_NONE;
  while(sdio_send_cmd(sd, &cmd, 0, &responce));
  delay(tc, 10);
  cmd = SDMMC_CMD_HS_SEND_EXT_CSD | HDL_SDIO_CMD_RESPONSE_SHORT;
  err = sdio_send_cmd(sd, &cmd, SDMMC_CHECK_PATTERN, &responce);

  if(err || (responce[0] & SDMMC_CHECK_PATTERN) != SDMMC_CHECK_PATTERN) 
    return HDL_FALSE; /* Not 2.0 Spec */
  uint32_t retry = 30;
  do {
    delay(tc, 40);
    cmd = SDMMC_CMD_APP_CMD | HDL_SDIO_CMD_RESPONSE_SHORT;
    if((err = sdio_send_cmd(sd, &cmd, 0, &responce)) == 0) {
      delay(tc, 5);
      cmd = HDL_SD_ACMD41 | HDL_SDIO_CMD_RESPONSE_SHORT;
      err = sdio_send_cmd(sd, &cmd, 0x40FF8000,
        //HDL_SD_ACMD41_ARG_HIGH_CAPACITY | HDL_SD_OCR_VW_3V2_3V3, 
        &responce);
      err = err & ~HDL_SDIO_ERROR_CRC; /* there is no CRC in R3 responce */
      uint32_t exp_options = HDL_SD_ACMD41_RESP_BUSY | HDL_SD_OCR_VW_3V2_3V3;
      if(!err && ((responce[0] & exp_options) == exp_options)) {
        //if(responce[0] & HDL_SD_OCR_HIGH_CAPACITY)
          //SdCard.CardType = CARD_SDHC_SDXC;
        //else
          //SdCard.CardType = CARD_SDSC;
        break;
      }
    }
  } while (!err && retry--);
  if(err) 
    return HDL_FALSE;


  cmd = SDMMC_CMD_ALL_SEND_CID | HDL_SDIO_CMD_RESPONSE_LONG;
  if (sdio_send_cmd(sd, &cmd, 0, &responce))
    return HDL_FALSE;

  cmd = SDMMC_CMD_SET_REL_ADDR | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (sdio_send_cmd(sd, &cmd, 0, &responce))
    return HDL_FALSE;
  uint32_t rca = responce[0] & 0xffff0000; /* RCA [31:16] */

  cmd = SDMMC_CMD_SEND_CSD | HDL_SDIO_CMD_RESPONSE_LONG;
  if (sdio_send_cmd(sd, &cmd, rca, &responce)) {
    return HDL_FALSE;
  }

  {
    uint64_t capacity = (((responce[1] & 0x0000003FU) << 16U) | ((responce[2] & 0xFFFF0000U) >> 16U));
    capacity = (uint64_t)(((((uint64_t)capacity + 1ULL)) * 1024ULL) * 512ULL);
    hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
    sd_var->capacity = capacity;
    //DEBUG_IPRINTF("SD card capacity %lld\r\n", CardCapacity);
  }

  //HAL_SD_CardCSDTypeDef ttt;
  //SD_GetCardCSD(responce, &ttt);

  cmd = SDMMC_CMD_SEL_DESEL_CARD | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (sdio_send_cmd(sd, &cmd, rca, &responce))
    return HDL_FALSE;

  cmd = SDMMC_CMD_SEND_STATUS | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (sdio_send_cmd(sd, &cmd, rca, &responce) || 
    (!(responce[0] & HDL_SD_STA_READY_FOR_DATA)))
      return HDL_FALSE; /* cmd err or !READY_FOR_DATA */

  //hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_4);
  hdl_sdio_set_clock(sdio, 24000000);
      
  return HDL_TRUE;
}

static void sd_read_write(hdl_sd_t *sd, uint8_t read) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  uint32_t *responce;
  uint8_t cmd;

  sd_var->data_msg.data = sd_var->nvm_msg->data;
  sd_var->data_msg.data_block_size = 512;
  sd_var->data_msg.data_length = 512;
  sd_var->data_msg.dir = read? HDL_SDIO_DATA_FROM_DEV: HDL_SDIO_DATA_TO_DEV;
  sd_var->data_msg.mode = HDL_SDIO_DATA_BLOCK;
  sd_var->data_msg.timeout = 1000;
  hdl_sdio_data_transfer(sdio, &sd_var->data_msg);

  cmd = (read? SDMMC_CMD_READ_SINGLE_BLOCK: SDMMC_CMD_WRITE_SINGLE_BLOCK) | HDL_SDIO_CMD_RESPONSE_SHORT;
  if (sdio_send_cmd(sd, &cmd, 0, &responce))
    return;

  while(!(sd_var->data_msg.status & HDL_SDIO_STATE_COMPLETE))
    yield_return;

  sd_var->nvm_msg->out_status = HDL_NVM_STATE_COMPLETE;
  sd_var->nvm_msg = NULL;
  __NOP();
}

static void _sd_reset(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_gpio_pin_t *pw_pin = (hdl_gpio_pin_t *)sd->dependencies[3];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  sd_var->capacity = 0;
  hdl_gpio_set_inactive(pw_pin);
  hdl_sdio_set_clock(sdio, 200000);
  hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_1);
  if(sd_var->nvm_msg != NULL)
    sd_var->nvm_msg->out_status = HDL_NVM_STATE_CANCELED;
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
  }
  else if (sd_var->state == SD_STATE_READY) {
    if(sd_var->nvm_msg != NULL) {
      switch (sd_var->op_mode) {
        case SD_READ: sd_read_write(sd, HDL_TRUE); break;
        case SD_WRITE: sd_read_write(sd, HDL_FALSE); break;
        case SD_ERASE: /* todo */
        default: break;
      }
    }
  }
  //else if (sd_var->state == SD_STATE_UNUSABLE)
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
    out_info->sector_size = 512;
    out_info->mtu_size = 512;
    out_info->volume = sd_var->capacity;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_sd_rw(const void *desc, hdl_nvm_message_t *message, uint8_t read) {
  hdl_sd_t *sd = (hdl_sd_t *)desc;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  if((message != NULL) && (sd_var->state == SD_STATE_READY) && (sd_var->nvm_msg == NULL)) {
    sd_var->op_mode = read? SD_READ: SD_WRITE;
    sd_var->nvm_msg = message;
    message->out_status = HDL_NVM_STATE_BUSY;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_sd_read(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_sd_rw(desc, message, HDL_TRUE);
}

static uint8_t _hdl_sd_write(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_sd_rw(desc, message, HDL_FALSE);
}

const hdl_nvm_iface_t hdl_sd_mcu_iface = {
  .init = &_hdl_sd,
  .info = &_hdl_sd_info_get,
  .read = &_hdl_sd_read,
  .write = &_hdl_sd_write
};
