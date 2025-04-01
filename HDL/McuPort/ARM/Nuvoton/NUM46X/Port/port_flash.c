#include "hdl_iface.h"

typedef struct {
  coroutine_t flash_worker;
  hdl_nvm_message_t *message;
  uint8_t state;
} hdl_flash_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_flash_mcu_var_t, *((hdl_flash_mcu_t *)0)->obj_var, HDL_MCU_FLASH_VAR_SIZE, port_flash.h);

#define FLASH_STATE_INITIAL        0
#define FLASH_STATE_AWAITE_WRITE   1
#define FLASH_STATE_AWAITE_ERASE   2
#define FLASH_STATE_WRITE          3
#define FLASH_STATE_ERASE          4
#define FLASH_STATE_READ           5
#define FLASH_STATE_COMPETE        6

static uint8_t _nvm_flash_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_flash_mcu_t *flash = (hdl_flash_mcu_t *)arg;
  hdl_flash_mcu_var_t *flash_var = (hdl_flash_mcu_var_t *)flash->obj_var;
  hdl_nvm_message_t *message = flash_var->message;
  if(message != NULL) {
    uint32_t address = flash->config->address_start + message->address + message->transferred;

    if(!(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {
      if (flash_var->state == FLASH_STATE_AWAITE_WRITE) flash_var->state = FLASH_STATE_WRITE;
      if (flash_var->state == FLASH_STATE_AWAITE_ERASE) flash_var->state = FLASH_STATE_ERASE;
    }

    if(flash_var->state == FLASH_STATE_INITIAL) {
      uint32_t flash_end = flash->config->address_start + flash->config->size;      
      flash_var->state = FLASH_STATE_COMPETE;
      
      if ((address + message->size) >= flash_end) {
        message->status |= HDL_NVM_ERROR_OUT_OF_RANGE;
      } 
      else if(message->options & HDL_NVM_OPTION_ERASE) {
        if((address & (FMC_FLASH_PAGE_SIZE - 1)) || (message->size & (FMC_FLASH_PAGE_SIZE - 1))) {
          message->status |= HDL_NVM_ERROR_PAGE_UNALIGNED;
        }
        else
          flash_var->state = FLASH_STATE_ERASE;
      }
      else if((message->options & HDL_NVM_OPTION_READ) && (message->buffer != NULL)) {
        flash_var->state = FLASH_STATE_READ;
      }
      else if((message->options & HDL_NVM_OPTION_WRITE) && (message->buffer != NULL)) { 
        flash_var->state = FLASH_STATE_WRITE;
      }
      if((flash_var->state == FLASH_STATE_WRITE) || (flash_var->state == FLASH_STATE_ERASE)) {
        SYS_UnlockReg();
        FMC_ENABLE_ISP();
        FMC_ENABLE_AP_UPDATE();
        FMC_CLR_FAIL_FLAG();
      }
    }

    if(flash_var->state == FLASH_STATE_WRITE) {
      uint32_t transfer = 4;
      uint32_t byte_no = address & 0x03UL;
      if(byte_no) transfer = 4 - byte_no;
      if((message->transferred + transfer) > message->size)
        transfer = (message->size - message->transferred);
      if(!transfer) flash_var->state = FLASH_STATE_COMPETE;
      else {
        uint32_t fd = 0xffffffff;
        if(transfer != 4) {
          for (uint32_t i = byte_no; i < (byte_no + transfer); i++) {
            uint8_t invb = ~message->buffer[message->transferred++];
            fd ^= (invb << (8 * i));
          }
        }
        else {
          fd = *((uint32_t *)(&message->buffer[message->transferred]));
          message->transferred += 4;
        }
        FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
        FMC->ISPADDR = address;
        FMC->ISPDAT = fd;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        flash_var->state = FLASH_STATE_AWAITE_WRITE;
      }
    }

    if(flash_var->state == FLASH_STATE_ERASE) {
      if (FMC->ISPCTL & FMC_ISPCTL_ISPFF_Msk) {
        message->status |= HDL_NVM_ERROR_INTERNAL_FAULT;
      }
      if(message->transferred >= message->size) flash_var->state = FLASH_STATE_COMPETE;
      else {
        FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
        FMC->ISPADDR = address;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        message->transferred += FMC_FLASH_PAGE_SIZE;
      }
    }

    if(flash_var->state == FLASH_STATE_READ) {
      uint32_t byte_no = address & 0x03UL;
      uint32_t *ptr = (uint32_t *)(address & ~0x03UL);
      for (uint32_t i = 0; i < message->size; i++) {
        message->buffer[i] = ((*ptr) >> (8 * byte_no));
        if(++byte_no >= 4) {
          byte_no = 0;
          ptr++;
        }
      }
      message->transferred = message->size;
      flash_var->state = FLASH_STATE_COMPETE;
    }

    if(flash_var->state == FLASH_STATE_COMPETE) {
      message->status |= HDL_NVM_STATE_COMPLETE;
      message->status &= ~HDL_NVM_STATE_BUSY;
      flash_var->message = NULL;
      FMC_DISABLE_AP_UPDATE();
      FMC_DISABLE_ISP();
      SYS_LockReg();
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_nvm(const void *desc, uint8_t enable) {
  hdl_flash_mcu_t *flash = (hdl_flash_mcu_t *)desc;
  hdl_flash_mcu_var_t *flash_var = (hdl_flash_mcu_var_t *)flash->obj_var;
  if(enable) {
    flash_var->message = NULL;
    coroutine_add(&flash_var->flash_worker, &_nvm_flash_worker, (void*)flash);
    return HDL_MODULE_ACTIVE;
  }
  if(flash_var->message != NULL) {
    flash_var->message->status |= HDL_NVM_ERROR_INTERNAL_FAULT;
  }
  coroutine_cancel(&flash_var->flash_worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_nvm_transfer(const void *desc, hdl_nvm_message_t *message) {
  hdl_flash_mcu_t *flash = (hdl_flash_mcu_t *)desc;
  hdl_flash_mcu_var_t *flash_var = (hdl_flash_mcu_var_t *)flash->obj_var;
  if((message != NULL) &&
     (flash_var->message == NULL) &&
     ((flash_var->message->size == 0) || (flash_var->message->buffer != NULL)) &&
     (flash_var->message->options != 0)) {
    flash_var->state = FLASH_STATE_INITIAL;
    message->status = HDL_NVM_STATE_BUSY;
    message->transferred = 0;
    flash_var->message = message;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_nvm_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_flash_mcu_t *flash = (hdl_flash_mcu_t *)desc;
  if(out_info != NULL) {
    out_info->page_size = flash->config->min_erase_unit;
    out_info->volume = flash->config->size;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_nvm_iface_t hdl_flash_mcu_iface = {
  .init = &_hdl_nvm,
  .info = &_hdl_nvm_info_get,
  .transfer = &_hdl_nvm_transfer
};
