// #include "hdl_iface.h"


// #define MODBUS_RX_STATE_DEFINE_PACKET_TYPE     3
// #define MODBUS_RX_STATE_SKIP_PACKET            20

// #define bPacketTypeIsValid(pt) ((pt == eModBusPacketError) || (((pt & ~eModBusPacketFull)) == 0))

// #define MODBUS_XFER_STATE_RECEIVING                  0x10
// #define MODBUS_XFER_STATE_RECEIVED                   0x20
// #define MODBUS_XFER_STATE_TRANSMITTING               0x40
// #define MODBUS_XFER_STATE_TRANSMITED                 0x80
// #define MODBUS_XFER_STATE_TX_ERR                     0x01
// #define MODBUS_XFER_STATE_RX_ERR                     0x02
// #define MODBUS_XFER_STATE_TX_TIMEOUT                 0x03
// #define MODBUS_XFER_STATE_RX_TIMEOUT                 0x04
// #define MODBUS_XFER_STATE_COMPLETE_NO_RESPONSE       0x0E
// #define MODBUS_XFER_STATE_COMPLETE                   0x0F

// typedef struct {
//     //SimpleCircularBuffer_t scb_tx;
//     //SimpleCircularBuffer_t scb_rx;

//     //ModBusData_t xMbPacketData;
// 	uint32_t timestamp;
// 	//uint8_t eRxPacketType;
//   uint8_t rx_type;

//   coroutine_t worker;
//   hdl_transceiver_t transceiver;

//   hdl_modbus_request_t *request;

//   uint8_t xfer_state;

//   int8_t frame_xref_state;
// 	hdl_modbus_frame_t frame;

// 	//uint16_t usCrc;
// 	//uint8_t ucRxCounter;
// } hdl_modbus_client_var_t;

// //_Static_assert(sizeof(_prModBusServer_t) == (MODBUS_SERVER_DESCRIPTOR_SIZE), "In Modbus.h data structure size doesn't match allocation MODBUS_SERVER_DESCRIPTOR_SIZE");
// //_Static_assert(sizeof(_prModBusClient_t) == (MODBUS_CLIENT_DESCRIPTOR_SIZE), "In Modbus.h data structure size doesn't match allocation MODBUS_CLIENT_DESCRIPTOR_SIZE");

// uint8_t hdl_modbus_func_to_packet_type(uint8_t func, uint8_t is_request) {
// 	switch (func) {
// 	case 0x01 /* Read coil */:
// 		return is_request? eModBusPacketBase: eModBusPacketVariableLen;
// 	case 0x02 /* Read input */:
// 		return is_request? eModBusPacketBase: eModBusPacketVariableLen;
// 	case 0x03 /* Read holding registers */:
// 		return is_request? eModBusPacketBase: eModBusPacketVariableLen;
// 	case 0x04 /* Read input iegisters */:
// 		return is_request? eModBusPacketBase: eModBusPacketVariableLen;
// 	case 0x05 /* Write single coil */:
// 		return eModBusPacketBase;
// 	case 0x06 /* Write single register */:
// 		return eModBusPacketBase;
// 	//	case 0x07 /* Read Exception Status */:
// 	//		return ;
// 	//	case 0x08 /* Diagnostic */:
// 	//		return ;
// 	//	case 0x0B /* Get Com Event Counter */:
// 	//		return ;
// 	//	case 0x0C /* Get Com Event Log */:
// 	//		return ;
// 	case 0x0F /* Write multiple coils */:
// 		return is_request? eModBusPacketFull: eModBusPacketBase;
// 	case 0x10 /* Write multiple registers */:
// 		return is_request? eModBusPacketFull: eModBusPacketBase;
//     //	case 0x11 /* Report Slave ID */:
//     //		return ;
//     //	case 0x14 /* Read File Record */:
//     //		return ;
//     //	case 0x15 /* Write File Record */:
//     //		return ;
//     //	case 0x16 /* Mask Write Register */:
//     //		return ;
//     //	case 0x18 /* Read FIFO Queue */:
//     //		return ;
//     //	case 0x2B /* Encapsulated Interface Transport */:
//     //		return ;
// 	default:
// 		return eModBusPacketNone;
// 	}
// }

// static inline uint16_t _crc_modbus_ascii(uint8_t *data, uint16_t count, uint16_t *value) {
//   uint16_t val = (value != NULL)? *value: 0;
// 	while (count--) val -= *data++;
// 	return val;
// }

// uint32_t _available_to_write(hdl_transceiver_t *trx) {
// 	return 0;
// }

// uint32_t _available_to_read(hdl_transceiver_t *trx) {
// 	return 0;
// }

// void _bus_flash(hdl_transceiver_t *trx) {

// }

// int32_t _bus_write(hdl_transceiver_t *trx, uint8_t *buf, uint32_t size) {
// 	return 0;
// }

// static int32_t _bus_write_ascii(hdl_transceiver_t *trx, uint8_t *data, uint32_t size) {
//   int32_t aw = _available_to_write(trx) >> 1;
//   int32_t count = 0;
//   while (size-- && aw--) {
//     uint16_t asscii_encoded = convert_byte_to_ascii_hex(*data++, HDL_TRUE);
//     //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
//     if(_bus_write(trx, (uint8_t *)&asscii_encoded, 2) != 2) break;
//     count++;
//   }
//   return count;
// }

// static uint8_t _bus_write_block(hdl_transceiver_t *trx, uint8_t *data, uint32_t block_size) {
//   uint32_t wa = _available_to_write(trx);
//   if(wa < block_size) return HDL_FALSE;
//   return _bus_write(trx, (uint8_t *)&data, block_size) == block_size;
// }

// static uint8_t _bus_write_block_ascii(hdl_transceiver_t *trx, uint8_t *data, uint32_t block_size) {
//   if(_available_to_write(trx) < (block_size * 2)) return HDL_FALSE;
//   while (block_size--) {
//     uint16_t asscii_encoded = convert_byte_to_ascii_hex(*data++, HDL_TRUE);
//     //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
//     int32_t res = _bus_write(trx, (uint8_t *)&asscii_encoded, 2);
//     if (res <= 0) return HDL_FALSE;
//   }
//   return HDL_TRUE;
// }

// static int8_t _modbus_tx_frame(hdl_transceiver_t *trx, int8_t ptx_sm, hdl_modbus_frame_t *frame, uint8_t tx_type, uint8_t is_ascii) {
// 	if (ptx_sm < 0) ptx_sm = 0;
// 	if (tx_type == eModBusPacketNone) return -1;
//   int32_t (*data_write_f)(hdl_transceiver_t *trx, uint8_t *buf, uint32_t size) = &_bus_write;
//   if(is_ascii) data_write_f = &_bus_write_ascii;
//   uint8_t (*block_write_f)(hdl_transceiver_t *trx, uint8_t *buf, uint32_t block_size) = &_bus_write_block;
//   if(is_ascii) block_write_f = &_bus_write_block_ascii;
//   uint16_t (*crc_modbus_f)(const uint8_t *data, int32_t len, const uint16_t *crc) = &crc16_modbus_rtu;
//   if(is_ascii) crc_modbus_f = &_crc_modbus_ascii;

//   switch (ptx_sm) {
// 	case 0:
// 	case 1 /* Tx frame start */:
// 		if ((is_ascii) && (_bus_write(trx, (uint8_t *)":", 1) <= 0)) return 1;
// 	case 2 /* Tx addr */:
//     if(data_write_f(trx, &frame->addr, 1) <= 0) return 2;
//     frame->crc = crc_modbus_f(&frame->addr, 1, NULL);
// 	case 3 /* Tx func */: 
// 		if (data_write_f(trx, &frame->func, 1) <=0 ) return 3;
//     frame->crc = crc_modbus_f(&frame->func, 1, &frame->crc);
// 	case 4 /* Tx Reg addr */:
// 		if (!(frame->func & MODBUS_RESPONSE_ERROR_FLAG_MASK) && (tx_type & eModBusPacketBase)) {
// 			uint16_t regAddr = swap_bytes(frame->reg_addr);
// 			if (!block_write_f(trx, (uint8_t *)&regAddr, 2)) return 4;
//       frame->crc = crc_modbus_f((uint8_t *)&regAddr, 2, &frame->crc);
// 		}
// 	case 5 /* Tx Reg count */:
// 		if (!(frame->func & MODBUS_RESPONSE_ERROR_FLAG_MASK) && (tx_type & eModBusPacketBase)) {
// 			uint16_t regValCount = swap_bytes(frame->reg_value_count);
// 			if (!block_write_f(trx, (uint8_t *)&regValCount, 2)) return 5;
// 			frame->crc = crc_modbus_f((uint8_t *)&regValCount, 2, &frame->crc);
// 		}
// 	case 6 /* Tx length */:
// 		if (tx_type & (eModBusPacketVariableLen | eModBusPacketError)) {
// 			if (data_write_f(trx, &frame->length_error, 1) <= 0) return 6;
// 			frame->crc = crc_modbus_f(&frame->length_error, 1, &frame->crc);
// 		}
// 	case 7 /* Tx data */:
// 		if (tx_type & eModBusPacketVariableLen) {
//       uint8_t *data = frame->data + frame->transfer_counter;
//       uint32_t left = frame->data_length - frame->transfer_counter;
// 			int32_t count = data_write_f(trx, data, left);
//       if(count < 0) return 7;
//       frame->transfer_counter += count;
// 			if (frame->transfer_counter < frame->length_error) return 7;
// 			frame->crc = crc_modbus_f(frame->data, frame->length_error, &frame->crc);
// 		}
// 	case 8 /* Tx crc */: 
// 		if (is_ascii) {
// 			uint8_t crc = frame->crc;
// 			if (!data_write_f(trx, &crc, 1)) return 8;
// 		}
// 		else if(!block_write_f(trx, (uint8_t *)&frame->crc, 2)) return 8;
// 	case 9: /* Tx frame terminator */
// 		if (is_ascii) {
//       uint32_t aw = _available_to_write(trx);
//       if((aw < 2) || (_bus_write(trx, (uint8_t *)"\r\n", 2) <= 0)) return 9;
//     }
// 	default:
// 		break;
// 	}
// 	return 0;
// }

// uint32_t _bus_read(hdl_transceiver_t *trx, uint8_t *buf, uint32_t size) {
// 	return 0;
// }

// static int32_t _bus_read_ascii(hdl_transceiver_t *trx, uint8_t *data, uint32_t size) {
//   int32_t count = 0;
//   uint16_t asscii_encoded;
//   uint32_t ar = _available_to_read(trx);
//   while (size-- && ar >= 2) {
//     //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
//     if((_bus_read(trx, (uint8_t *)&asscii_encoded, 2) != 2) ||
//        !convert_ascii_hex_to_byte(asscii_encoded, data++)) return -1;
//     ar -= 2;
//     count++;
//   }
//   count >>= 2;
// }

// static uint8_t _bus_read_block(hdl_transceiver_t *trx, uint8_t *data, uint32_t block_size) {
//   if (_available_to_read(trx) < block_size) return HDL_FALSE;
//   return _bus_read(trx, data, block_size) == block_size;
// }

// static uint8_t _bus_read_block_ascii(hdl_transceiver_t *trx, uint8_t *data, uint32_t block_size) {
//   if (_available_to_read(trx) < (block_size * 2)) return HDL_FALSE;
//   uint16_t asscii_encoded;
//   while (block_size--) {
//     //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
//     if((_bus_read(trx, (uint8_t *)&asscii_encoded, 2) != 2) ||
//        !convert_ascii_hex_to_byte(asscii_encoded, data++)) return HDL_FALSE;
//   }
//   return HDL_TRUE;
// }

// static int8_t _modbus_rx_frame(hdl_transceiver_t *trx, int8_t prx_sm, hdl_modbus_frame_t *frame, uint8_t rx_type, uint8_t is_ascii) {
//   int32_t (*data_read_f)(hdl_transceiver_t *trx, uint8_t *buf, uint32_t size) = &_bus_read;
//   if(is_ascii) data_read_f = &_bus_read_ascii;
//   int32_t (*block_read_f)(hdl_transceiver_t *trx, uint8_t *buf, uint32_t block_size) = &_bus_read_block;
//   if(is_ascii) block_read_f = &_bus_read_block_ascii;
//   uint16_t (*crc_modbus_f)(const uint8_t *data, int32_t len, const uint16_t *crc) = &crc16_modbus_rtu;
//   if(is_ascii) crc_modbus_f = &_crc_modbus_ascii;
//   if (prx_sm <= 0) prx_sm = 1;
//   switch (prx_sm) {
//     case 1 /* Rx frame start */:
//       if (is_ascii) {
//         if ((_bus_read(trx, &frame->addr, 1) <= 0) || (frame->addr != ':')) return 1;
//       }
//       else if (_available_to_read(trx) <= 0) return 1;
//     case 2 /* Rx addr & func */: {
//       uint8_t buf[2];
//       int32_t res = data_read_f(trx, buf, 2);
//       if (res <= 0) return ((res < 0)? -1: 2);
//       frame->addr = buf[0];
//       frame->func = buf[1];
//       frame->crc = crc_modbus_f(buf, 2, NULL);
//       return MODBUS_RX_STATE_DEFINE_PACKET_TYPE;
//     }
//     case MODBUS_RX_STATE_DEFINE_PACKET_TYPE:
//     case 4 /* Rx Reg addr & count */:
//       if (rx_type & eModBusPacketBase) {
//         uint8_t buf[4];
//         int32_t res = _receive_all(trx, buf, 4, is_ascii);
//         if (res <= 0) return ((res < 0)? -1: 4);
//         frame->reg_addr = (((uint16_t)buf[0]) << 8) | buf[1];
//         frame->reg_value_count = (((uint16_t)buf[2]) << 8) | buf[3];
//         frame->crc = crc_modbus_f(buf, 4, &frame->crc);
//       }
//     case 5 /* Rx length | err */:
//       if ((rx_type & eModBusPacketVariableLen) || (rx_type == eModBusPacketError)) {
//         int32_t res = _receive_all(trx, &frame->length_error, 1, is_ascii);
//         if (res <= 0) return ((res < 0)? -1: 5);
//         frame->transfer_counter = 0;
//         frame->crc = crc_modbus_f(&frame->length_error, 1, &frame->crc);
//       }
//     case 6:
//       if ((rx_type & eModBusPacketVariableLen) && (frame->length_error > frame->data_length)) {
//         frame->length_error = 0;
//         return ((is_ascii)? -1: MODBUS_RX_STATE_SKIP_PACKET);
//       }
//     case 7 /* Rx data */:
//       if (rx_type & eModBusPacketVariableLen) {
//         int32_t res = _receive_data(trx, 
//         frame->data + frame->transfer_counter, 
//         frame->length_error - frame->transfer_counter,
//         is_ascii);
//         if (res <= 0) return ((res < 0)? -1: 7);
//         frame->transfer_counter += res;
//         frame->crc = crc_modbus_f(frame->data, res, &frame->crc);			
//         if (frame->length_error > frame->transfer_counter) return 7;
//       }
//     case 8 /* Rx unknown */:
//       if (rx_type == eModBusPacketNone) {
//         if (is_ascii) return -1;
//         else {
//           frame->length_error = 0;
//           return MODBUS_RX_STATE_SKIP_PACKET;
//         }
//       }
//     case 9 /* Rx crc */: {
//       uint8_t buf[2];
//       uint8_t count = ((is_ascii)? 1: 2);
//       int32_t res = _receive_all(trx, buf, count, is_ascii);
//       if (res <= 0) return ((res < 0)? -1: 9);
//       if (is_ascii) {
//         if (((uint8_t)frame->crc) != buf[0]) return -1;
//       }
//       else return (frame->crc == ((((uint16_t)buf[1]) << 8) | buf[0]))? 0: -1;
//     }
//     case 10:
//       if (is_ascii) {
//         if (_available_to_read(trx) < 2) return 10;
//         uint8_t buf[2];
//         _bus_read(trx, buf, 2);
//         return (((buf[0] == '\r') && (buf[1] == '\n'))? 0: -1);
//       }
//     case MODBUS_RX_STATE_SKIP_PACKET:
//     default: /* Will try find end of frame. Break if crc match or too many data received, or timeout occurred */
//       while (_bus_read(trx, &frame->data[1], 1) > 0) {
//         if (frame->length_error > 0) {
//           uint16_t crc = (((uint16_t)frame->data[0]) << 8) | (frame->data[1]);
//           if ((crc == frame->crc) || (frame->length_error >= 251)) return -1;
//           frame->crc = crc16_modbus_rtu(frame->data, 1, &frame->crc);
//         }
//         frame->length_error++;
//         frame->data[0] = frame->data[1];
//       }
//       break;
//   }
//   return MODBUS_RX_STATE_SKIP_PACKET;
// }

// static void _modbus_packet_copy_header(hdl_modbus_frame_t *dst, hdl_modbus_frame_t *src) {
//   dst->reg_addr = src->reg_addr;
//   dst->reg_value_count = src->reg_value_count;
//   dst->length_error = src->length_error;
//   dst->reg_value_count = src->reg_value_count;
// }

// static uint8_t _modbus_client_handler(coroutine_t *this, uint8_t cancel, void *arg) {
//   (void)this; 
//   hdl_modbus_client_t *mbc = (hdl_modbus_client_t *)arg;
//   hdl_modbus_client_var_t *var = (hdl_modbus_client_var_t *)mbc->obj_var;
//   hdl_time_counter_t *timer = (hdl_time_counter_t *)mbc->dependencies[0];
//   if (var->request != NULL) {
//     hdl_modbus_request_t *req = var->request;
//     if (var->xfer_state == MODBUS_XFER_STATE_TRANSMITTING) {
//       if(var->frame_xref_state <= 0) 
//         var->timestamp = hdl_time_counter_get(timer);
//       var->frame_xref_state = _modbus_tx_frame(&var->transceiver, var->frame_xref_state, 
//         &req->frame, req->tx_type, mbc->config->is_ascii);
//       if(var->frame_xref_state == 0) {
//         var->xfer_state = MODBUS_XFER_STATE_RECEIVING;
//         var->rx_type = req->rx_type;
//         _modbus_packet_copy_header(&(var->frame), &(req->frame));
//         var->frame.transfer_counter = 0;
//         var->timestamp = hdl_time_counter_get(timer);
//       }
//       if(var->frame_xref_state < 0)
//         var->xfer_state = MODBUS_XFER_STATE_TX_ERR;
//       if(TIME_ELAPSED(var->timestamp, mbc->config->tx_timeout, hdl_time_counter_get(timer)))
//         var->xfer_state = MODBUS_XFER_STATE_TX_TIMEOUT;
//     }
//     if (var->xfer_state == MODBUS_XFER_STATE_RECEIVING) {
//       if(var->rx_type != eModBusPacketNone) {
//         var->frame_xref_state = _modbus_rx_frame(&var->transceiver, var->frame_xref_state, 
//         &var->frame, var->rx_type, mbc->config->is_ascii);
//         if (var->frame_xref_state == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) {
//           if (bModBusIsErrorResponse(var->frame))
//             var->rx_type = eModBusPacketError;
//           else if((var->frame.addr != req->frame.addr) || (var->frame.func != req->frame.func))
//             var->rx_type = hdl_modbus_func_to_packet_type(var->frame.func, 0);
//         }
//         else if (var->frame_xref_state <= 0) {
//           var->xfer_state = MODBUS_XFER_STATE_RX_ERR;
//           if (var->frame_xref_state == 0) {
//             if((var->frame.addr == req->frame.addr) &&
//               ((var->frame.func & ~MODBUS_RESPONSE_ERROR_FLAG_MASK) == req->frame.func)) 
//               var->xfer_state = MODBUS_XFER_STATE_COMPLETE;
//             else {
//               var->xfer_state = MODBUS_XFER_STATE_RECEIVING;
//               var->rx_type = req->rx_type;
//               var->timestamp = hdl_time_counter_get(timer);
//             }
//           }
//         }
//         else if (TIME_ELAPSED(var->timestamp, mbc->config->rx_timeout, hdl_time_counter_get(timer)))
//           var->xfer_state = MODBUS_XFER_STATE_RX_TIMEOUT;
//       }
//       else
//         var->xfer_state = MODBUS_XFER_STATE_COMPLETE_NO_RESPONSE;
//     }
//     if(var->xfer_state & MODBUS_XFER_STATE_COMPLETE) {
//       var->request = NULL;
//       if (req->complete_cb != NULL) {
//         uint8_t *buf = var->frame.data;
//         uint8_t bufSz = var->frame.data_length;
//         ModBusResult_t err = 0;
//         if((var->xfer_state == MODBUS_XFER_STATE_COMPLETE_NO_RESPONSE) ||
//           (var->xfer_state == MODBUS_XFER_STATE_TX_TIMEOUT) ||
//           (var->xfer_state == MODBUS_XFER_STATE_RX_TIMEOUT))
//           err = ModBusGatewayTargetDeviceFailedtoRespond;
//         if((var->xfer_state == MODBUS_XFER_STATE_TX_ERR) ||
//           (var->xfer_state == MODBUS_XFER_STATE_RX_ERR))
//           err = ModBusGatewayPathUnavailable;
//         if(err) {
//           _modbus_packet_copy_header(&(var->frame), &(req->frame));
//           var->frame.func = (req->frame.func | 0x80);
//           var->frame.length_error = err;
//         }
//         req->complete_cb(req, &var->frame);
//         var->frame.data = buf;
//         var->frame.data_length = bufSz;
//         var->frame_xref_state = -1;
//       }
//       var->request = NULL;
//     }
//   }
//   else _bus_flash(&var->transceiver);
//   return cancel;
// }

// static hdl_module_state_t _hdl_modbus_client_init(const void *desc, uint8_t enable) {
//   hdl_modbus_client_t *mbc = (hdl_modbus_client_t *)desc;
//   hdl_modbus_client_var_t *var = (hdl_modbus_client_var_t *)mbc->obj_var;
//   if(enable) {
//     // vScbInit(&context->scb_tx, 0, 0);
//     // vScbInit(&context->scb_rx, 0, 0);
//     // context->trx.proto_context = context;
//     // context->pxRequest = NULL;
//     // context->xMbPacketData.data = NULL; // TODO
//     // context->xMbPacketData.ucDataBufferSize = 0; // TODO
//     // context->frame_tx_state = 0;
//     coroutine_add(&var->worker, &_modbus_client_handler, (void *)mbc);
//     return HDL_MODULE_ACTIVE;
//   }
//   coroutine_cancel(&var->worker);
//   return HDL_MODULE_UNLOADED;
// }

// static uint8_t _hdl_modbus_client_transfer(const void *desc, hdl_modbus_request_t *request) {
//   hdl_modbus_client_t *mbc = (hdl_modbus_client_t *)desc;
//   hdl_modbus_client_var_t *var = (hdl_modbus_client_var_t *)mbc->obj_var;
//   if (bPacketTypeIsValid(request->rx_type) &&
//     (request->tx_type != eModBusPacketNone) &&
//     ((request->tx_type | eModBusPacketFull) == eModBusPacketFull) &&
//     (!(request->tx_type & eModBusPacketVariableLen) || 
//         ((request->frame.data_length > 0) && (request->frame.data != NULL)))) {
//     var->xfer_state = MODBUS_XFER_STATE_TRANSMITTING;
//     var->request = request;
//     request->frame.transfer_counter = 0;
//     return HDL_TRUE;
//   }
//   return HDL_FALSE;
// }

// static hdl_transceiver_t *_hdl_modbus_client_get_transceiver(const void *desc) {
//   hdl_modbus_client_t *mbc = (hdl_modbus_client_t *)desc;
//   hdl_modbus_client_var_t *var = (hdl_modbus_client_var_t *)mbc->obj_var;
//   return &var->transceiver;
// }

// const hdl_modbus_client_iface_t hdl_modbus_client_iface = {
//   .init = &_hdl_modbus_client_init,
//   .transfer = &_hdl_modbus_client_transfer,
//   .get_transceiver = &_hdl_modbus_client_get_transceiver
// };





















// // typedef struct {
// // 	ModBusHandler_t **handlers;
// // 	uint8_t ucHandlersCount;
// // 	uint8_t ucAddress;
// // 	uint8_t bBroadCastEnable;
// //     __LinkedListObject__;
// // } _ModBusServerEndpoint_t;

// // typedef struct {
// //   hdl_transceiver_t trx;
// // ModBusHandler_t xHandler;
// // ModBusData_t xMbPacketData;
// // uint8_t *pucPayloadBuffer;
// // uint32_t ulTimer;
// // uint8_t ucPayloadBufferSize;
// // int8_t upacket_tx_state;
// // LinkedList_t pxModBusEndpoints;

// // uint8_t ucXferState;
// // } _prModBusServer_t;

// // static ModBusResult_t _eModBusDummyHandler(ModBusData_t *pxInOutData, void *pxContext) {
// //   (void)pxInOutData; (void)pxContext;
// // return ModBusIllegalFunction;
// // }

// // static uint8_t _bModBusServerHandlerMatch(LinkedListItem_t *pxHandler, void *pxArg) {
// // 	_ModBusServerEndpoint_t *ep =  LinkedListGetObject(_ModBusServerEndpoint_t, pxHandler);
// // 	uint8_t *addr = (uint8_t *)pxArg;
// // 	return (ep->ucAddress == *addr) || ((*addr == 0) && (ep->bBroadCastEnable));
// // }

// // static void _vModBusServerSetHandler(ModBusHandler_t *pxHandler, _ModBusServerEndpoint_t *pxEndpoint, uint8_t ucFunc) {
// // 	pxHandler->pfReceiveCallback = &_eModBusDummyHandler;
// // 	if (ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) {
// // 		pxHandler->eRxPacketType = eModBusPacketError;
// // 		pxHandler->eTxPacketType = eModBusPacketNone;
// // 	}
// // 	else {
// // 		if (pxEndpoint == NULL) {
// // 			pxHandler->eTxPacketType = eModBusPacketNone;
// // 			pxHandler->eRxPacketType = _eModBusFuncToPacketType(ucFunc, 1);
// // 		}
// // 		else {
// // 			ModBusHandler_t *hdesc = NULL;
// // 			for (int32_t i = 0; i < pxEndpoint->ucHandlersCount; i++) {
// // 				if (pxEndpoint->handlers[i]->ucFunctionNo == ucFunc) {
// // 					hdesc = pxEndpoint->handlers[i];
// // 					break;
// // 				}
// // 			}
// // 			if (hdesc == NULL) {
// // 				pxHandler->eTxPacketType = eModBusPacketError;
// // 				pxHandler->eRxPacketType = _eModBusFuncToPacketType(ucFunc, 1);
// // 			}
// // 			else {
// // 				pxHandler->pfReceiveCallback =  hdesc->pfReceiveCallback;
// // 				pxHandler->eRxPacketType = hdesc->eRxPacketType;
// // 				pxHandler->eTxPacketType = hdesc->eTxPacketType;
// // 			}
// // 		}
// // 	}	
// // }


// // static void _vModBusServerWork(_prModBusServer_t *pxMbServer, uint8_t is_ascii) {
// // 	if (pxMbServer->ucXferState & MODBUS_XFER_STATE_RECEIVING) {
// // 		if(pxMbServer->upacket_tx_state <= 1) {
// // 			pxMbServer->usTimer = hdl_time_counter_get(timer);
// // 		}
// // 		if (_bIsTimerExpired(pxMbServer->usTimer, MODBUS_SERVER_RX_TIMEOUT)) /* timeout */ {
// // 			pxMbServer->upacket_tx_state = 0;
// // 		}
// // 		pxMbServer->upacket_tx_state = _cModBusReceivePacket(pxMbServer->pxDataRx, 
// // 			pxMbServer->upacket_tx_state,
// // 			&pxMbServer->xMbPacketData,
// // 			pxMbServer->xHandler.eRxPacketType,
// // 			is_ascii);
// // 		if (pxMbServer->upacket_tx_state == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) {				
// // 			_ModBusServerEndpoint_t *currentEndpoint = LinkedListGetObject(_ModBusServerEndpoint_t, pxLinkedListFindFirst(
// // 				pxMbServer->pxModBusEndpoints,
// // 				&_bModBusServerHandlerMatch,
// // 				&pxMbServer->xMbPacketData.ucAddr));
// // 			_vModBusServerSetHandler(&pxMbServer->xHandler, currentEndpoint, pxMbServer->xMbPacketData.ucFunc);
// // 			if (pxMbServer->xMbPacketData.ucAddr == 0) {
// // 				pxMbServer->xHandler.eTxPacketType = eModBusPacketNone;
// // 			}
// // 		}
// // 		if (pxMbServer->upacket_tx_state <= 0) {
// // 			if (pxMbServer->upacket_tx_state == 0) {
// // 				pxMbServer->xMbPacketData._ucXferState = MODBUS_XFER_STATE_RECEIVED;
// // 			}
// // 		}
// // 	}
// // 	if (pxMbServer->xMbPacketData._ucXferState & MODBUS_XFER_STATE_RECEIVED) {
// // 		ModBusResult_t result = pxMbServer->xHandler.pfReceiveCallback(&pxMbServer->xMbPacketData, pxMbServer->xHandler.pxContext);
// // 		if (pxMbServer->xHandler.eTxPacketType != eModBusPacketNone) {
// // 			if (result != ModBusOk) {
// // 				/* tx error */
// // 				pxMbServer->xMbPacketData.ucFunc |= MODBUS_RESPONSE_ERROR_FLAG_MASK;
// // 				pxMbServer->xMbPacketData.ucLengthError = result;
// // 				pxMbServer->xHandler.eTxPacketType = eModBusPacketError;
// // 			}
// // 			pxMbServer->xMbPacketData._ucXferState = MODBUS_XFER_STATE_TRANSMITTING;
// // 		}
// // 		else {
// // 			pxMbServer->xMbPacketData._ucXferState = MODBUS_XFER_STATE_RECEIVING;
// // 		}
// // 	}
// // 	if (pxMbServer->xMbPacketData._ucXferState & MODBUS_XFER_STATE_TRANSMITTING) {
// // 		pxMbServer->upacket_tx_state = _modbus_tx_frame(pxMbServer->pxDataTx, 
// // 			pxMbServer->upacket_tx_state, 
// // 			&pxMbServer->xMbPacketData, 
// // 			pxMbServer->xHandler.eTxPacketType, 
// // 			is_ascii);
// // 		if (pxMbServer->upacket_tx_state <= 0) {
// // 			pxMbServer->xMbPacketData._ucXferState = MODBUS_XFER_STATE_RECEIVING;
// // 		}
// // 		//TODO: check tx timeout
// // 	}	
// // }


// // void vModBusServerLinkEndpoint(ModBusServer_t *pxMbServer, ModBusServerEndpoint_t *pxEp) {
// //   _ModBusServerEndpoint_t *pxMbEp = (_ModBusServerEndpoint_t *)pxEp;
// //   _prModBusServer_t *context = (_prModBusServer_t*)pxMbServer;
// //   if(pxMbEp->ucAddress == 0) return;
// //   uint8_t check = 1;
// //   for (uint8_t i = 0; i < pxMbEp->ucHandlersCount; i++) {
// //       check = check && 
// //           bPacketTypeIsValid(pxMbEp->handlers[i]->eRxPacketType) &&
// //           (pxMbEp->handlers[i]->eRxPacketType != eModBusPacketNone) &&
// //           bPacketTypeIsValid(pxMbEp->handlers[i]->eTxPacketType) &&
// //           (pxMbEp->handlers[i]->pfReceiveCallback != NULL);
// //   }
// //   if (check) {
// //       vLinkedListInsert(&context->pxModBusEndpoints, LinkedListItem(pxMbEp), NULL);
// //   }
// // }


// // void vModBusServerInit(ModBusServer_t *pxMbServer) {
// //   _prModBusServer_t *context = (_prModBusServer_t*)pxMbServer;
// //   context->pucPayloadBuffer = NULL; // TODO
// //   context->ucPayloadBufferSize = 0; // TODO
// //   context->ucXferState = MODBUS_XFER_STATE_RECEIVING;
// //   context->upacket_tx_state = 0;
// //   context->xHandler.pfReceiveCallback = _eModBusDummyHandler;
// //   context->xHandler.eRxPacketType = eModBusPacketNone;
// //   context->xHandler.eTxPacketType = eModBusPacketError;
// //   context->xMbPacketData.data = context->pucPayloadBuffer;
// //   context->xMbPacketData.ucDataBufferSize = context->ucPayloadBufferSize;
// //   context->pxModBusEndpoints = NULL;
// // }
