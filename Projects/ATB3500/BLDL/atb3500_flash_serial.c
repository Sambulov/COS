#include "app.h"
#include "CodeLib.h"

#define ATB3500_SERIAL_ADDRESS       (((void *)&_eflash) - (ATB3500_SERIAL_SIZE))

typedef struct {
  hdl_module_t module;
  atb3500_flash_serial_proto_tx_t tx_buf;
} atb3500_flash_serial_private_t;

_Static_assert(sizeof(atb3500_flash_serial_private_t) == sizeof(atb3500_flash_serial_t), "In atb3500_flash_serial.h data structure size of atb3500_flash_serial_t doesn't match, check ATB3500_FLASH_SERIAL_PRV_SIZE");

hdl_module_state_t atb3500_flash_serial(void *desc, uint8_t enable) {
  if(enable) {
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

typedef struct
{
  __IO uint32_t WS;              /*!< FMC wait state register */
  __IO uint32_t KEY;             /*!< FMC unlock key register */
  __IO uint32_t OBKEY;           /*!< FMC option byte unlock key register */
  __IO uint32_t STAT;            /*!< FMC status register */
  __IO uint32_t CTL;             /*!< FMC control register */
  __IO uint32_t OBCTL0;          /*!< FMC option byte control register 0 */
  __IO uint32_t OBCTL1;          /*!< FMC option byte control register 1 */
  __IO uint32_t _reserved;
  __IO uint32_t PECFG;           /*!< FMC page erase configuration register */
  __IO uint32_t PEKEY;           /*!< FMC unlock page erase key register */
  __IO uint32_t __reserved[54];
  __IO uint32_t WSEN;            /*!< FMC wait state enable register */
  __IO uint32_t PID;             /*!< FMC product ID register */
} FMC_t;

uint8_t flash_unlock(void) {
  FMC_t *FLASH = (FMC_t *)FMC;
  if (!(FLASH->STAT & FMC_STAT_BUSY) && (FLASH->CTL & FMC_CTL_LK)) {
    FLASH->KEY = UNLOCK_KEY0;
    FLASH->KEY = UNLOCK_KEY1;
  }
	return (FLASH->CTL & FMC_CTL_LK) == 0;
}

uint8_t burn_flash(uint32_t addr, uint8_t *data, uint8_t len) {
  FMC_t *FLASH = (FMC_t *)FMC;
	if (flash_unlock()) {
    FLASH->CTL |= FMC_CTL_PG;
    while (len) {
      FMC_CTL &= ~FMC_CTL_PSZ;
      __IO uint32_t* f_ref = (uint32_t *)addr;
      uint32_t f_data;
      if((len >= 4) && !((uint32_t)data & 0x3) && !((uint32_t)addr & 0x3) ) {
        FLASH->CTL |= CTL_PSZ_WORD;
        f_data = *(uint32_t *)data;
        data += 4;
        addr += 4;
        len -= 4;
      } else if((len >= 2) && !((uint32_t)data & 0x1) && !((uint32_t)addr & 0x1)) {
        FLASH->CTL |= CTL_PSZ_HALF_WORD;
        f_data = *(uint16_t *)data;
        data += 2;
        addr += 2;
        len -= 2;
      }
      else {
        FLASH->CTL |= CTL_PSZ_BYTE;
        f_data = *data;
        data++;
        addr++;
        len--;
      }
      *f_ref = f_data;
			while ((FLASH->STAT & FMC_STAT_BUSY) != 0) {

			}
		}
		FLASH->CTL &= ~FMC_CTL_PG; 
		FLASH->CTL |= FMC_CTL_LK;
    return HDL_TRUE;
	}
  return HDL_FALSE;
}

uint8_t verify_flash(uint32_t addr, uint8_t *data, uint8_t len) {
  __IO uint8_t *f_ref = (uint8_t *)addr;
  while (len--) {
    if(*f_ref != *data) return HDL_FALSE;
    f_ref++;
    data++;
  }
  return HDL_TRUE;
}

uint8_t write_flash(uint32_t addr, uint8_t *data, uint8_t len) {
  return burn_flash(addr, data, len) && verify_flash(addr, data, len);
}

uint8_t erase_flash(uint32_t addr, uint32_t amount) {
  FMC_t *FLASH = (FMC_t *)FMC;
	if (flash_unlock()) {
    FLASH->PEKEY = UNLOCK_PE_KEY;
    FLASH->PECFG |= FMC_PE_EN;
    FMC_PECFG = FMC_PE_EN | ((addr >> 12) << 12); /* 4K align */
    FLASH->CTL &= ~FMC_CTL_SN;
    FLASH->CTL |= FMC_CTL_SER;
    FLASH->CTL |= FMC_CTL_START;
    while ((FLASH->STAT & FMC_STAT_BUSY) != 0) {

    }
    FLASH->PECFG &= ~FMC_PE_EN;
    FLASH->CTL &= ~FMC_CTL_SER;
		FLASH->CTL |= FMC_CTL_LK;
    return HDL_TRUE;
	}
  return HDL_FALSE;
}

uint8_t sector_erase_flash(uint32_t fmc_sector) {
  FMC_t *FLASH = (FMC_t *)FMC;
	if (flash_unlock()) {
    /* start sector erase */
    FMC_CTL &= ~FMC_CTL_SN;
    FMC_CTL |= (FMC_CTL_SER | fmc_sector);
    FMC_CTL |= FMC_CTL_START;
    while ((FLASH->STAT & FMC_STAT_BUSY) != 0) {
    }
    FMC_CTL &= (~FMC_CTL_SER);
    FMC_CTL &= ~FMC_CTL_SN;
		FLASH->CTL |= FMC_CTL_LK;
    return HDL_TRUE;
	}
  return HDL_FALSE;
}

atb3500_flash_serial_proto_tx_t *atb3500_flash_serial_update(atb3500_flash_serial_t *desc, atb3500_flash_serial_proto_rx_t *data) {
  atb3500_flash_serial_private_t *serial = (atb3500_flash_serial_private_t *)desc;
  if(serial != NULL) {
    serial->tx_buf.status = (*(uint8_t *)(ATB3500_SERIAL_ADDRESS) == 0xff)? SERIAL_EMPTY: SERIAL_VALID;
    if((data->programm_key1 == ATB3500_SERIAL_PROGRAMM_KEY1) && (data->programm_key2 == ATB3500_SERIAL_PROGRAMM_KEY2)) {
      sector_erase_flash(CTL_SECTOR_NUMBER_11);
      if(!write_flash((uint32_t)ATB3500_SERIAL_ADDRESS, data->serial, ATB3500_SERIAL_SIZE)) {
        sector_erase_flash(CTL_SECTOR_NUMBER_11);
        serial->tx_buf.status = SERIAL_FLASH_FAULT;
      }
      else serial->tx_buf.status = SERIAL_VALID;
      data->programm_key1 = 0;
      data->programm_key2 = 0;
    }
    mem_cpy(serial->tx_buf.serial, (uint8_t*)ATB3500_SERIAL_ADDRESS, ATB3500_SERIAL_SIZE);
    return &serial->tx_buf;
  }
  return NULL;
}
