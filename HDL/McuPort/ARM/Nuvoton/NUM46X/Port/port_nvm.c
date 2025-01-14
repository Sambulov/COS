#include "hdl_portable.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_t module;
  const hdl_nvm_config_t *config;
  struct {
    coroutine_t flash_worker;
    hdl_nvm_message_t *message;
    uint8_t state;
  } private;
} hdl_nvm_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_nvm_private_t, hdl_nvm_t, HDL_NVM_PRV_SIZE, hdl_nvm.h);

#define FLASH_STATE_INITIAL        0
#define FLASH_STATE_AWAITE_WRITE   1
#define FLASH_STATE_AWAITE_ERASE   2
#define FLASH_STATE_WRITE          3
#define FLASH_STATE_ERASE          4
#define FLASH_STATE_READ           5
#define FLASH_STATE_COMPETE        6

static uint8_t _nvm_flash_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_nvm_private_t *nvm = (hdl_nvm_private_t *)arg;
  hdl_nvm_message_t *message = nvm->private.message;
  if(message != NULL) {
    uint32_t address = nvm->config->address_start + message->address + message->transferred;

    if(!(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {
      if (nvm->private.state == FLASH_STATE_AWAITE_WRITE) nvm->private.state == FLASH_STATE_WRITE;
      if (nvm->private.state == FLASH_STATE_AWAITE_ERASE) nvm->private.state == FLASH_STATE_ERASE;
    }

    if(nvm->private.state == FLASH_STATE_INITIAL) {
      uint32_t flash_end = nvm->config->address_start + nvm->config->size;      
      nvm->private.state = FLASH_STATE_COMPETE;
      
      if ((address + message->size) >= flash_end) {
        message->state |= HDL_NVM_ERROR_OUT_OF_RANGE;
      } 
      else if(message->options & HDL_NVM_OPTION_ERASE) {
        if((address & (FMC_FLASH_PAGE_SIZE - 1)) || (message->size & (FMC_FLASH_PAGE_SIZE - 1))) {
          message->state |= HDL_NVM_ERROR_PAGE_UNALIGNED;
        }
        else
          nvm->private.state = FLASH_STATE_ERASE;
      }
      else if((message->options & HDL_NVM_OPTION_READ) && (message->buffer != NULL)) {
        nvm->private.state = FLASH_STATE_READ;
      }
      else if((message->options & HDL_NVM_OPTION_WRITE) && (message->buffer != NULL)) { 
        nvm->private.state = FLASH_STATE_WRITE;
      }
      if((nvm->private.state == FLASH_STATE_WRITE) || (nvm->private.state == FLASH_STATE_ERASE)) {
        SYS_UnlockReg();
        FMC_ENABLE_ISP();
        FMC_ENABLE_AP_UPDATE();
        FMC_CLR_FAIL_FLAG();
      }
    }

    if(nvm->private.state == FLASH_STATE_WRITE) {
      uint32_t transfer = 4;
      uint32_t byte_no = address & 0x03UL;
      if(byte_no) transfer = 4 - byte_no;
      if((message->transferred + transfer) > message->size)
        transfer = (message->size - message->transferred);
      if(!transfer) nvm->private.state = FLASH_STATE_COMPETE;
      else {
        uint32_t fd = 0xffffffff;
        if(transfer != 4) {
          for (int32_t i = byte_no; i < (byte_no + transfer); i++) {
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
        nvm->private.state == FLASH_STATE_AWAITE_WRITE;
      }
    }

    if(nvm->private.state == FLASH_STATE_ERASE) {
      if (FMC->ISPCTL & FMC_ISPCTL_ISPFF_Msk) {
        message->state |= HDL_NVM_ERROR_INTERNAL_FAULT;
      }
      if(message->transferred >= message->size) nvm->private.state = FLASH_STATE_COMPETE;
      else {
        FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
        FMC->ISPADDR = address;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        message->transferred += FMC_FLASH_PAGE_SIZE;
      }
    }

    if(nvm->private.state == FLASH_STATE_READ) {
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
      nvm->private.state = FLASH_STATE_COMPETE;
    }

    if(nvm->private.state == FLASH_STATE_COMPETE) {
      message->state |= HDL_NVM_STATE_COMPLETE;
      message->state &= ~HDL_NVM_STATE_BUSY;
      nvm->private.message = NULL;
      FMC_DISABLE_AP_UPDATE();
      FMC_DISABLE_ISP();
      SYS_LockReg();
    }
  }
  return cancel;
}

hdl_module_state_t hdl_nvm(void *desc, uint8_t enable) {
  hdl_nvm_private_t *nvm = (hdl_nvm_private_t *)desc;
  if(enable) {
    nvm->private.message = NULL;
    coroutine_add(&nvm->private.flash_worker, &_nvm_flash_worker, (void*)nvm);
    return HDL_MODULE_ACTIVE;
  }
  if(nvm->private.message != NULL) {
    nvm->private.message->state |= HDL_NVM_ERROR_INTERNAL_FAULT;
  }
  coroutine_cancel(&nvm->private.flash_worker);
  return HDL_MODULE_UNLOADED;
}

uint8_t hdl_nvm_transfer(hdl_nvm_t *desc, hdl_nvm_message_t *message) {
  hdl_nvm_private_t *nvm = (hdl_nvm_private_t *)desc;
  if((nvm != NULL) && 
     (hdl_state((hdl_module_t *)nvm) != HDL_MODULE_UNLOADED) && 
     (message != NULL) &&
     (nvm->private.message == NULL) &&
     ((nvm->private.message->size == 0) || (nvm->private.message->buffer != NULL)) &&
     (nvm->private.message->options != 0)) {
    nvm->private.state = FLASH_STATE_INITIAL;
    message->state = HDL_NVM_STATE_BUSY;
    message->transferred = 0;
    nvm->private.message = message;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

uint8_t hdl_nvm_info(hdl_nvm_t *desc, hdl_nvm_info_t *out_info) {
  hdl_nvm_private_t *nvm = (hdl_nvm_private_t *)desc;
  if((nvm != NULL) && (hdl_state((hdl_module_t *)nvm) != HDL_MODULE_UNLOADED) && (out_info != NULL)) {
    out_info->block_size = nvm->config->min_erase_unit;
    out_info->volume = nvm->config->size;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
