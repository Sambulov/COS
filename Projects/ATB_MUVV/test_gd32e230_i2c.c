//#include "app.h"
//#include "CodeLib.h"

// void testx() {
//   //hdl_enable(&mod_timer_ms.module);
//   //hdl_enable(&mod_i2c0_client.module);

//   while (!hdl_init_complete()) {
//     cooperative_scheduler(false);
//   }

//   // HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
//   // HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
//   // HDL_I2C_MESSAGE_MRSW           = 0x04, /* Receiver mode if option set, else trasmitter mode */
//   // HDL_I2C_MESSAGE_ADDR_10        = 0x02, /* Send addr */
//   // HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
//   // HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */


//   uint8_t reg_addr[2] = {0x00, 0x00};

//   hdl_i2c_transaction_t tr;
//   hdl_i2c_message_t msg1 = {
//     .address = 10,
//     .buffer = reg_addr,
//     .buffer_size = 2,
//     .options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR
//   };
//   uint8_t data[32];
//   hdl_i2c_message_t msg2 = {
//     .address = 10,
//     .buffer = data,
//     .buffer_size = 32,
//     .options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP
//   };
//   //hdl_i2c_client_transaction_add_message(&tr, &msg1);
//   //hdl_i2c_client_transaction_add_message(&tr, &msg2);
//   //hdl_i2c_client_enqueue_transaction(&mod_i2c0_client, &tr);
//   while (1) {
//     cooperative_scheduler(false);
//   }
// }