#include "hdl_iface.h"
#include "hdl_sd_def.h"

typedef enum {
  SD_READ,
  SD_WRITE,
  SD_ERASE
} hdl_sd_op_mode_t;

typedef struct {
  coroutine_t worker;
  uint32_t timer;
  struct {
    uint64_t capacity;
    uint32_t block_size;
    uint32_t rca;
    uint8_t max_bus_clk;
    uint8_t sdhc       :1,
            wr_protect :1;
  } card;
  struct {
    struct {
      uint16_t block_offset;
      uint8_t sate     : 7,
              cmd_sent : 1;
      uint8_t retry;
    } sub;
    hdl_sd_op_mode_t op_mode;
    uint8_t sd_state;
    uint32_t delay_timer;
    uint32_t delay;
  } fsm;
  
  hdl_nvm_message_t *nvm_msg;
  hdl_sdio_cmd_message_t cmd_msg;
  hdl_sdio_data_message_t data_msg;
} hdl_sd_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sd_var_t, *((hdl_sd_t *)0)->obj_var, HDL_SD_VAR_SIZE, "hdl_sd.h");

static inline uint8_t _sdio_send_cmd(hdl_sd_t *sd, uint8_t cmd, uint32_t arg) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  sd_var->cmd_msg.cmd = cmd;
  sd_var->cmd_msg.argument = arg;
  return hdl_sdio_cmd_transfer(sdio, &sd_var->cmd_msg);
}

static inline uint8_t _sdio_cmd_awaiting(hdl_sd_var_t *sd_var) {
  return !(sd_var->cmd_msg.status & HDL_SDIO_STATE_COMPLETE);
}

static inline void _delay_set(hdl_time_counter_t *tc, hdl_sd_var_t *sd_var, uint32_t ms) {
  sd_var->fsm.delay_timer = hdl_time_counter_get(tc);
  sd_var->fsm.delay = ms;
}

static inline uint8_t _delay_awaiting(hdl_time_counter_t *tc, hdl_sd_var_t *sd_var) {
  uint32_t now = hdl_time_counter_get(tc);
  return !CL_TIME_ELAPSED(sd_var->fsm.delay_timer, sd_var->fsm.delay, now);
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

#define SD_INIT_STATE_PREPARE      0
#define SD_INIT_STATE_RESET        1
#define SD_INIT_STATE_HS_SEND_EXT  2
#define SD_INIT_STATE_ACMD41_PREP  3
#define SD_INIT_STATE_ACMD41       4
#define SD_INIT_STATE_CID          5
#define SD_INIT_STATE_RCA          6
#define SD_INIT_STATE_CSD          7
#define SD_INIT_STATE_SELECT       8
#define SD_INIT_STATE_STATUS       9
#define SD_INIT_STATE_ACMD6_PREP   10
#define SD_INIT_STATE_ACMD6        11
#define SD_INIT_STATE_COMPLETE     12

static int8_t init_sd(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];

  if(sd_var->fsm.sub.cmd_sent) {
    uint8_t next_state;
    switch (sd_var->fsm.sub.sate) {
      case SD_INIT_STATE_RESET:
        if(sd_var->cmd_msg.status & HDL_SDIO_ERROR) {
          _delay_set(tc, sd_var, 10);
          next_state = SD_INIT_STATE_RESET;
        }
        next_state = SD_INIT_STATE_HS_SEND_EXT;
        break;
      case SD_INIT_STATE_HS_SEND_EXT:
        if((sd_var->cmd_msg.status & HDL_SDIO_ERROR) || 
          (sd_var->cmd_msg.response[0] & SDMMC_CHECK_PATTERN) != SDMMC_CHECK_PATTERN) 
          return -1; /* Not 2.0 Spec */
        next_state = SD_INIT_STATE_ACMD41_PREP;
        break;
      case SD_INIT_STATE_ACMD41_PREP:
        if(!(sd_var->cmd_msg.status & HDL_SDIO_ERROR) && (sd_var->cmd_msg.response[0] & HDL_SD_STA_APP_CMD))
          next_state = SD_INIT_STATE_ACMD41;
        else
          next_state = SD_INIT_STATE_ACMD41_PREP;
        break;
      case SD_INIT_STATE_ACMD41:
        sd_var->cmd_msg.status &= ~HDL_SDIO_ERROR_CRC; /* there is no CRC in R3 responce */
        uint32_t exp_options = HDL_SD_ACMD41_RESP_BUSY | sd->config->voltage_window;
        if(!(sd_var->cmd_msg.status & HDL_SDIO_ERROR) && ((sd_var->cmd_msg.response[0] & exp_options) == exp_options)) {
          sd_var->card.sdhc = ((sd_var->cmd_msg.response[0] & HDL_SD_ACMD41_RESP_HIGH_CAPACITY) != 0);
          next_state = SD_INIT_STATE_CID;
        }
        else
          next_state = SD_INIT_STATE_ACMD41_PREP;
        break;
      case SD_INIT_STATE_CID:
        if(sd_var->cmd_msg.status & HDL_SDIO_ERROR)
          return -1;
        /* todo: parse CID */
        next_state = SD_INIT_STATE_RCA;
        break;
      case SD_INIT_STATE_RCA:
        if(sd_var->cmd_msg.status & HDL_SDIO_ERROR)
          return -1;
        sd_var->card.rca = sd_var->cmd_msg.response[0] & 0xffff0000; /* RCA [31:16] */
        next_state = SD_INIT_STATE_CSD;
        break;
      case SD_INIT_STATE_CSD:
        if(sd_var->cmd_msg.status & HDL_SDIO_ERROR)
          return -1;
        _sd_parse_csd(sd_var, sd_var->cmd_msg.response);
        next_state = SD_INIT_STATE_SELECT;
        break;
      case SD_INIT_STATE_SELECT:
        if(sd_var->cmd_msg.status & HDL_SDIO_ERROR)
          return -1;
        next_state = SD_INIT_STATE_STATUS;
        break;
      case SD_INIT_STATE_STATUS:
        if((sd_var->cmd_msg.status & HDL_SDIO_ERROR) || !(sd_var->cmd_msg.response[0] & HDL_SD_STA_READY_FOR_DATA))
          return -1; /* cmd err or !READY_FOR_DATA */
        next_state = SD_INIT_STATE_ACMD6_PREP;
        break;
      case SD_INIT_STATE_ACMD6_PREP:
        if(!(sd_var->cmd_msg.status & HDL_SDIO_ERROR) && (sd_var->cmd_msg.response[0] & HDL_SD_STA_APP_CMD))
          next_state = SD_INIT_STATE_ACMD6;
        else
          next_state = SD_INIT_STATE_COMPLETE;
        break;
      case SD_INIT_STATE_ACMD6:
        if(!(sd_var->cmd_msg.status & HDL_SDIO_ERROR))
          hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_4);
        next_state = SD_INIT_STATE_COMPLETE;
        break;
      default:
        return -1;
    }
    sd_var->fsm.sub.sate = next_state;
    sd_var->fsm.sub.cmd_sent = HDL_FALSE;
  }

  uint8_t cmd;
  uint32_t arg;
  switch (sd_var->fsm.sub.sate) {
    case SD_INIT_STATE_PREPARE:
      sd_var->fsm.sub.cmd_sent = HDL_FALSE;
      sd_var->fsm.sub.retry = 30;
      sd_var->fsm.sub.sate = SD_INIT_STATE_RESET;
    /* fall through */
    case SD_INIT_STATE_RESET:
      cmd = SDMMC_CMD_GO_IDLE_STATE | HDL_SDIO_CMD_RESPONSE_NONE;
      arg = 0;
      break;
    case SD_INIT_STATE_HS_SEND_EXT:
      cmd = SDMMC_CMD_HS_SEND_EXT_CSD | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = SDMMC_CHECK_PATTERN;
      _delay_set(tc, sd_var, 10);
      break;
    case SD_INIT_STATE_ACMD41_PREP:
      if(!sd_var->fsm.sub.retry) 
        return -1;
      sd_var->fsm.sub.retry--;
      cmd = SDMMC_CMD_APP_CMD | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = 0;
      _delay_set(tc, sd_var, 5);
      break;
    case SD_INIT_STATE_ACMD41:
      cmd = HDL_SD_ACMD41 | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = HDL_SD_ACMD41_ARG_HIGH_CAPACITY | sd->config->voltage_window;
      _delay_set(tc, sd_var, 45);
      break;
    case SD_INIT_STATE_CID:
      cmd = SDMMC_CMD_ALL_SEND_CID | HDL_SDIO_CMD_RESPONSE_LONG;
      arg = 0;
      break;
    case SD_INIT_STATE_RCA:
      cmd = SDMMC_CMD_SET_REL_ADDR | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = 0;
      break;
    case SD_INIT_STATE_CSD:
      cmd = SDMMC_CMD_SEND_CSD | HDL_SDIO_CMD_RESPONSE_LONG;
      arg = sd_var->card.rca;
      break;
    case SD_INIT_STATE_SELECT:
      cmd = SDMMC_CMD_SEL_DESEL_CARD | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = sd_var->card.rca;
      break;
    case SD_INIT_STATE_STATUS:
      cmd = SDMMC_CMD_SEND_STATUS | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = sd_var->card.rca;
      break;
    case SD_INIT_STATE_ACMD6_PREP:
      if(hdl_sdio_bus_supported(sdio, HDL_SDIO_BUS_WIDTH_4)) {
        cmd = SDMMC_CMD_APP_CMD | HDL_SDIO_CMD_RESPONSE_SHORT;
        arg = sd_var->card.rca;
        _delay_set(tc, sd_var, 5);
      }
      else {
        sd_var->fsm.sub.sate = SD_INIT_STATE_COMPLETE;
        return 0;
      }
      break;
    case SD_INIT_STATE_ACMD6:
      cmd = SDMMC_CMD_APP_SD_SET_BUSWIDTH | HDL_SDIO_CMD_RESPONSE_SHORT;
      arg = 2;
      break;
    case SD_INIT_STATE_COMPLETE:
      hdl_sdio_set_clock(sdio, sd_var->card.max_bus_clk * 1000000);
      return 1;
    default:
      break;
  }
  if(_sdio_send_cmd(sd, cmd, arg))
    sd_var->fsm.sub.cmd_sent = HDL_TRUE;
  return 0;
}

#define SD_BLOCK_SIZE        512

#define SD_RW_STATE_PREPARE     0
#define SD_RW_STATE_COMMAND     1
#define SD_RW_STATE_TX_DATA     2
#define SD_RW_STATE_AWAIT       3
#define SD_RW_STATE_COMPLETE    4
#define SD_RW_STATE_ERR         5

static uint8_t sd_read_write(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  while (sd_var->nvm_msg != NULL) {
    if(sd_var->fsm.sub.sate == SD_RW_STATE_PREPARE) {
      sd_var->fsm.sub.retry = 3;
      sd_var->fsm.sub.block_offset = 0;
      sd_var->fsm.sub.sate = SD_RW_STATE_COMMAND;
    }
    if(sd_var->fsm.sub.sate == SD_RW_STATE_COMMAND) {
      uint32_t data_offset = (uint32_t)sd_var->fsm.sub.block_offset * SD_BLOCK_SIZE;
      sd_var->data_msg.data = &((uint8_t *)sd_var->nvm_msg->data)[data_offset];
      sd_var->data_msg.data_block_size = SD_BLOCK_SIZE;
      sd_var->data_msg.data_length = SD_BLOCK_SIZE;
      sd_var->data_msg.dir = ((sd_var->fsm.op_mode == SD_READ))? HDL_SDIO_DATA_FROM_DEV: HDL_SDIO_DATA_TO_DEV;
      sd_var->data_msg.mode = HDL_SDIO_DATA_BLOCK;
      sd_var->data_msg.timeout = 1000;
      if(sd_var->fsm.op_mode == SD_READ)
        hdl_sdio_data_transfer(sdio, &sd_var->data_msg);
      uint8_t cmd = ((sd_var->fsm.op_mode == SD_READ)? 
        SDMMC_CMD_READ_SINGLE_BLOCK: 
        SDMMC_CMD_WRITE_SINGLE_BLOCK) | HDL_SDIO_CMD_RESPONSE_SHORT;
      uint32_t block = (sd_var->nvm_msg->address / SD_BLOCK_SIZE) + sd_var->fsm.sub.block_offset;
      if(_sdio_send_cmd(sd, cmd, block)) {
        if(sd_var->fsm.op_mode == SD_WRITE)
          sd_var->fsm.sub.sate = SD_RW_STATE_TX_DATA;
        else 
          sd_var->fsm.sub.sate = SD_RW_STATE_AWAIT;
      }
    }
    else if(sd_var->fsm.sub.sate == SD_RW_STATE_TX_DATA) {
      sd_var->fsm.sub.sate = SD_RW_STATE_AWAIT;
      if(!(sd_var->cmd_msg.status & HDL_SDIO_ERROR)) {
        if(sd_var->cmd_msg.response[0] & HDL_SD_STA_READY_FOR_DATA)
          hdl_sdio_data_transfer(sdio, &sd_var->data_msg);
        else {
          if(sd_var->fsm.sub.retry--) {
            _delay_set(tc, sd_var, 1);
            sd_var->fsm.sub.sate = SD_RW_STATE_COMMAND;
            return HDL_TRUE;
          }
          sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
        }
      }
    }
    if(sd_var->fsm.sub.sate == SD_RW_STATE_AWAIT) {
      if((sd_var->cmd_msg.status & HDL_SDIO_ERROR) || (sd_var->data_msg.status & HDL_SDIO_ERROR))
        sd_var->fsm.sub.sate = SD_RW_STATE_ERR;
      else if(sd_var->data_msg.status & HDL_SDIO_STATE_COMPLETE) {
        sd_var->fsm.sub.sate = SD_RW_STATE_COMPLETE;
        sd_var->nvm_msg->synced_size += SD_BLOCK_SIZE;
        sd_var->fsm.sub.block_offset++;
        if(sd_var->nvm_msg->size > ((uint32_t)sd_var->fsm.sub.block_offset * SD_BLOCK_SIZE)) {
          sd_var->fsm.sub.sate = SD_RW_STATE_COMMAND;
          continue;
        }
      }
    }
    if(sd_var->fsm.sub.sate == SD_RW_STATE_ERR) {
      if((sd_var->data_msg.status | sd_var->cmd_msg.status) & HDL_SDIO_ERROR_INTERNAL)
        sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_INTERNAL_FAULT;
      else
        sd_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
      sd_var->fsm.sub.sate = SD_RW_STATE_COMPLETE;
    }
    if(sd_var->fsm.sub.sate == SD_RW_STATE_COMPLETE) {
      sd_var->nvm_msg->out_status &= ~HDL_NVM_STATE_BUSY;
      sd_var->nvm_msg->out_status |= HDL_NVM_STATE_COMPLETE;
      sd_var->nvm_msg = NULL;
      sd_var->fsm.sub.sate = SD_RW_STATE_PREPARE;
    }
    break;
  }
  return HDL_TRUE;
}

static inline void _sd_fsm_reset(hdl_sd_t *sd) {
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  sd_var->fsm.sub.sate = 0;
  sd_var->fsm.sub.cmd_sent = HDL_FALSE;
  sd_var->fsm.delay = 0;
}

static void _sd_reset(hdl_sd_t *sd) {
  hdl_sdio_t *sdio = (hdl_sdio_t *)sd->dependencies[0];
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  _sd_fsm_reset(sd);
  sd_var->card.capacity = 0;
  sd_var->card.rca = 0;
  sd_var->cmd_msg.status = HDL_SDIO_STATE_COMPLETE;
  hdl_sdio_set_clock(sdio, 300000);
  hdl_sdio_set_bus(sdio, HDL_SDIO_BUS_WIDTH_1);
  if(sd_var->nvm_msg != NULL)
    sd_var->nvm_msg->out_status = HDL_NVM_STATE_CANCELED | HDL_NVM_STATE_COMPLETE;
  sd_var->nvm_msg = NULL;
}

#define SD_STATE_CARD_DETECT     0
#define SD_STATE_RESET           1
#define SD_STATE_INIT            2
#define SD_STATE_READY           3

static uint8_t _sd_worker(coroutine_t *this, uint8_t cancel, void *context) {
  (void)this;
  hdl_sd_t *sd = (hdl_sd_t *)context;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)sd->dependencies[1];
  hdl_gpio_pin_t *cd_pin = (hdl_gpio_pin_t *)sd->dependencies[2];
  hdl_gpio_pin_t *pw_pin = (hdl_gpio_pin_t *)sd->dependencies[3];

  if(_delay_awaiting(tc, sd_var)) return cancel;
  if(_sdio_cmd_awaiting(sd_var)) return cancel;

  if(sd_var->fsm.sd_state == SD_STATE_CARD_DETECT) {
    if(hdl_is_null_module(cd_pin) || hdl_gpio_is_active(cd_pin)) {
      _delay_set(tc, sd_var, sd->config->power_up_delay);
      hdl_gpio_set_active(pw_pin);
      sd_var->fsm.sd_state = SD_STATE_INIT;
      _sd_fsm_reset(sd);
    }
  }
  else if(sd_var->fsm.sd_state == SD_STATE_INIT) {
    int8_t res = init_sd(sd);
    if(res > 0) {
      sd_var->fsm.sd_state = SD_STATE_READY;
      _sd_fsm_reset(sd);
    }
    if(res < 0)
      sd_var->fsm.sd_state = SD_STATE_RESET;
  }
  else if (sd_var->fsm.sd_state == SD_STATE_READY) {
    if(!sd_read_write(sd)) {
      _sd_reset(sd);
      sd_var->fsm.sd_state = SD_STATE_INIT;
    }
  }
  if (hdl_gpio_is_inactive(cd_pin)) {
    hdl_gpio_set_inactive(pw_pin);
    _sd_reset(sd);
    sd_var->fsm.sd_state = SD_STATE_CARD_DETECT;
  }
  else if(sd_var->fsm.sd_state == SD_STATE_RESET) {
    _sd_reset(sd);
    _delay_set(tc, sd_var, sd->config->init_retry_delay);
    sd_var->fsm.sd_state = SD_STATE_INIT;
  }
  return cancel;
}

static hdl_module_state_t _hdl_sd(const void *desc, uint8_t enable) {
  hdl_sd_t *sd = (hdl_sd_t *)desc;
  hdl_sd_var_t *sd_var = (hdl_sd_var_t *)sd->obj_var;
  if(enable) {
    _sd_reset(sd);
    sd_var->fsm.sd_state = 0;
    coroutine_add(&sd_var->worker, _sd_worker, sd);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&sd_var->worker);
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
  if((message != NULL) && (sd_var->fsm.sd_state == SD_STATE_READY) && (sd_var->nvm_msg == NULL)) {
    if(message->address & (sd_var->card.block_size - 1))
      message->out_status = HDL_NVM_ERROR_SECTOR_UNALIGNED | HDL_NVM_STATE_COMPLETE;
    else if((message->address + message->size) >= sd_var->card.capacity)
      message->out_status = HDL_NVM_ERROR_OUT_OF_RANGE | HDL_NVM_STATE_COMPLETE;
    else if(op_mode == SD_ERASE) 
      message->out_status = HDL_NVM_STATE_COMPLETE; /* we don't need it for SD */
    else if((op_mode == SD_WRITE) && (sd_var->card.wr_protect))
      message->out_status = HDL_NVM_ERROR_LOCKED | HDL_NVM_STATE_COMPLETE;
    else {
      sd_var->fsm.op_mode = op_mode;
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
