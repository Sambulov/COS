#include "hdl_portable.h"

// typedef struct {
//   __IO uint32_t CTL0;            /*!< I2C Control register 0,                Address offset: 0x00 */
//   __IO uint32_t CTL1;            /*!< I2C Control register 1,                Address offset: 0x04 */
//   __IO uint32_t SADDR0;          /*!< I2C Slave address register 0,          Address offset: 0x08 */
//   __IO uint32_t SADDR1;          /*!< I2C Slave address register 1,          Address offset: 0x0C */
//   __IO uint32_t DATA;            /*!< I2C Transfer buffer register,          Address offset: 0x10 */
//   __IO uint32_t STAT0;           /*!< I2C Transfer status register 0,        Address offset: 0x14 */
//   __IO uint32_t STAT1;           /*!< I2C Transfer status register 1,        Address offset: 0x18 */
//   __IO uint32_t CKCFG;           /*!< I2C Clock configure register,          Address offset: 0x1C */
//   __IO uint32_t RT;              /*!< I2C Rise time register,                Address offset: 0x20*/
//   __IO uint32_t _dummy24_7C[23]; 
//   __IO uint32_t SAMCS;           /*!< I2C SAM control and status register,   Address offset: 0x80 */
//   __IO uint32_t _dummy84_8C[3];
//   __IO uint32_t FMPCFG;          /*!< I2C Fast mode plus configure register, Address offset: 0x90 */
// } i2c_periph_t;

// void hdl_hal_i2c_deinit(hdl_i2c_t *desc) {
//   if((desc == NULL || desc->hw_conf == NULL))
//     return;
//   /* reset config */
//   hdl_gpio_deinit(desc->hw_conf->scl);
//   hdl_gpio_deinit(desc->hw_conf->sda);
//   i2c_deinit(desc->hw_conf->periph);
// }

// int32_t hdl_i2c(hdl_i2c_t *desc, uint8_t enable) {
//   if((desc == NULL) || (desc->hw_conf == NULL))
//     return;
//   /* reset config */
//   hdl_hal_i2c_deinit(desc);

//   if((desc->hw_conf->mode & 0x03) == 0x00) { /* if disabled mode */
//     return;
//   }
//   /* init gpio */
//   hdl_gpio_init(desc->hw_conf->scl);
//   hdl_gpio_init(desc->hw_conf->sda);
//   hdl_gpio_write(desc->hw_conf->scl, HDL_GPIO_HIGH);
//   hdl_gpio_write(desc->hw_conf->sda, HDL_GPIO_HIGH);

//   switch (desc->hw_conf->periph) {
//     case I2C0:
//       rcu_periph_clock_enable(RCU_I2C0);
//       break;
//     case I2C1:
//       rcu_periph_clock_enable(RCU_I2C1);
//       break;
//     default:
//       return HDL_FALSE;
//   }
//   /* configure I2C clock */
//   i2c_clock_config(desc->hw_conf->periph, desc->hw_conf->speed, desc->hw_conf->dtcy);

//   i2c_slave_response_to_gcall_config(desc->hw_conf->periph, I2C_GCEN_DISABLE);
//   i2c_mode_addr_config(desc->hw_conf->periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0); /* general call address */
//   i2c_dualaddr_disable(desc->hw_conf->periph);

//   /* if server mode enabled */
//   if(desc->hw_conf->mode & 2) {
//     /* configure I2C address */
//     i2c_mode_addr_config(desc->hw_conf->periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, desc->hw_conf->addr << 1);
//     if(desc->hw_conf->general_call)
//       i2c_slave_response_to_gcall_config(desc->hw_conf->periph, I2C_GCEN_ENABLE);
//   }
//   i2c_enable(desc->hw_conf->periph);
//   if(!desc->hw_conf->mode & 1) { /* if i2c client reset bus */
//     i2c_stop_on_bus(desc->hw_conf->periph);
//   }
// }

// #define is_timeout(start_tick, timeout) ((hdl_millis() - start_tick) >= timeout)

// static uint8_t _hdl_reg_wait_condition(__IO uint32_t *reg, uint32_t flags, uint8_t is_set, uint32_t timeout) {
//   uint32_t start_tick = hdl_millis();
//   if(is_set) {
//     while (((*reg & flags) != flags) && !(is_timeout(start_tick, timeout))) ;
//   }
//   else {
//     flags = ~flags;
//     while (((*reg | flags) != flags) && !(is_timeout(start_tick, timeout))) ;
//   }
//   return is_timeout(start_tick, timeout);
// }

// void hdl_i2c_client_xfer_message(hdl_i2c_t *desc, hdl_i2c_message_t *msg) {

// }


// // void hdl_hal_i2c_transfer_message(const hdl_i2c_hw_t *hw_conf, hdl_i2c_message_t *message) {
// //   if((hw_conf == NULL) || (message == NULL))
// //     return;
// //   i2c_periph_t *desc = (i2c_periph_t *)hw_conf->periph;
// //   if(message->options & HDL_I2C_MESSAGE_SEND_START) {
// //     /* send a start condition to I2C bus */
// //     i2c_start_on_bus((uint32_t)desc);
// //     /* wait until SBSEND bit is set */
// //     _hdl_reg_wait_condition(&desc->STAT0, I2C_STAT0_SBSEND, 1, 10);
// //     /* send slave address to I2C bus */
// //     i2c_master_addressing((uint32_t)desc, message->address << 1, 
// //       (message->options & HDL_I2C_MESSAGE_READ)? I2C_RECEIVER: I2C_TRANSMITTER);
// //     /* wait until ADDSEND bit is set */
// //     _hdl_reg_wait_condition(&desc->STAT0, I2C_STAT0_ADDSEND, 1, 10);
// //     /* clear the ADDSEND bit */
// //     //i2c_flag_clear(desc, I2C_FLAG_ADDSEND); // ?????
// //   }

// //   i2c_ackpos_config((uint32_t)desc, I2C_ACKPOS_CURRENT);
// //   i2c_ack_config((uint32_t)desc, I2C_ACK_ENABLE);
  
// //   if(((message->options & HDL_I2C_MESSAGE_READ) && (desc->STAT1 & I2C_STAT1_TR)) ||
// //       (!(message->options & HDL_I2C_MESSAGE_READ) && !(desc->STAT1 & I2C_STAT1_TR))) {
// //     /* if message read operation and desc state transmitter or
// //           message write operation and desc state receiver */
// //     // TODO: reset transceiver, return bad state
// //   }

// //   /* transfer data */
// //   for (uint32_t i = 0; i < message->length; i++) {
// //     if(message->options & HDL_I2C_MESSAGE_READ) {
// //       /* wait until byte received */
// //       if(_hdl_reg_wait_condition(&desc->STAT0, I2C_STAT0_RBNE, 1, 10)) {
// //         break;
// //       }
// //       message->data[i] = i2c_data_receive((uint32_t)desc);
// //     }
// //     else {
// //       if(_hdl_reg_wait_condition(&desc->STAT0, I2C_STAT0_TBE, 1, 10)) {
// //         break;
// //       }
// //       i2c_data_transmit((uint32_t)desc, message->data[i]);
// //     }
// //   }
// // }

