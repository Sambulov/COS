// #include "hdl_portable.h"
// #include "CodeLib.h"
// #include "Macros.h"

// // #define bUartIs9BitTransmission(port) (((port)->Instance->CR1 & (USART_CR1_M | USART_CR1_PCE)) == (USART_CR1_M))

// typedef enum {
// 	NoError      = 0,
// 	ParityError  = USART_STAT_PERR,
// 	FrameError   = USART_STAT_FERR,
// 	NoiseError   = USART_STAT_NERR,
// 	OverRunError = USART_STAT_ORERR,
// 	StateMask    = ParityError | FrameError | NoiseError | OverRunError
// } UartError_t;

// // static void _vCheckIdleTimeout(UartHandlerPrivate_t *xUart) {
// // 	if (xUart->idleTimer && (xUart->idleTimer < ullMillis())) {
// // 		_vRstUartStatus(xUart);
// // 		xUart->idleTimer = 0;
// // 	}
// // }

// // static void _vUartReceiveCallback(UartHandlerPrivate_t *xUart, uint16_t rxData) {
// // 	xUart->idleTimer = ullMillis() + 1;
// // 	//TransceiverBufferAlign_t align = bUartIs9BitTransmission(xUart) ? eAlign2Bytes : eAlignByte;
// // 	if (xUart->pxHandlers->pfRxDataCallback != NULL) {
// // 		xUart->pxHandlers->pfRxDataCallback(xUart->pxHandlers->pxProtoDescriptor, (uint8_t *)&rxData, 1);
// // 	}
// // }

// // static void _vUartTransmitCallback(UartHandlerPrivate_t *xUart) {
// // 	uint8_t data;
// // 	//TransceiverBufferAlign_t align = bUartIs9BitTransmission(xUart) ? eAlign2Bytes : eAlignByte;
// // 	if ((xUart->pxHandlers->pfTxEmptyCallback != NULL) && 
// // 		(xUart->pxHandlers->pfTxEmptyCallback(
// // 			xUart->pxHandlers->pxProtoDescriptor,
// // 		&data, 1))) {
// // 		xUart->Instance->DR = data;
// // 	}
// // 	else {
// // 		if (xUart == (&uarts[eRS485Port])) {
// // 			vRS485DriverDisable();
// // 		}
// // 	}
// // }

// // #if (UART_TX_IN_ISR)
// // static void _vTxWorkISR(void) {
// // 	int i;
// // 	for (i = 0; i < UARTS_COUNT; i++) {
// // 		if (uarts[i].isInitialized && 
// // 			!(uarts[i].Instance->CR1 & (USART_CR1_TCIE | USART_CR1_TXEIE)) &&
// // 			uarts[i].pxHandlers != NULL) {
// // 			//TransceiverBufferAlign_t align = bUartIs9BitTransmission(&uarts[i]) ? eAlign2Bytes : eAlignByte;
// // 			if (uarts[i].pxHandlers->pfTxAvailable(uarts[i].pxHandlers->pxProtoDescriptor)) {
// // 				if (i == eRS485Port) {
// // 					if (!(uarts[i].idleTimer)) {
// // 						vRS485DriverEnable();
// // 					}
// // 				}
// // 				if ((i != eRS485Port) || (bRS485IsDriverEnabled())) {
// // 					uarts[i].Instance->CR1 |= USART_CR1_TXEIE;
// // 				}
// // 			}
// // 		}
// // 	}
// // }
// // #else
// // static void _vTxWorkPending(void)
// // {
// // 	int i;
// // 	uint8_t rep = 0;
// // 	do
// // 	{
// // 		rep = 0;
// // 		for (i = 0; i < UARTS_COUNT; i++)
// // 		{
// // 			if (uarts[i].isInitialized && (uarts[i].Instance->SR & (USART_SR_TC | USART_SR_TXE)))
// // 			{
// // 				uint16_t av = lFIFOAvailableToRead(uarts[i].stream.Tx);
// // 				if ((i == eRS485Port) && !(av) && (uarts[i].Instance->SR & USART_SR_TC))
// // 				{
// // 					vRS485DriverDisable();
// // 				}
// // 				if ((av >= 2) || (!bUartIs9BitTransmission(&uarts[i]) && av))
// // 				{
// // 					if (i == eRS485Port)
// // 					{
// // 						if (!(uarts[i].idleTimer))
// // 						{
// // 							vRS485DriverEnable();
// // 						}
// // 					}
// // 					if ((i != eRS485Port) || (bRS485IsDriverEnabled()))
// // 					{
// // 						_vUartTransmitCallback(&uarts[i]);
// // 						rep = 1;
// // 					}
// // 				}
// // 			}
// // 		}
// // 	} while (rep);
// // }
// // #endif




// // void vUartWork(void) {
// // 	int i;
// // 	for (i = 0; i < UARTS_COUNT; i++) {
// // 		_vCheckIdleTimeout(&uarts[i]);
// // 	}
// // #if (UART_TX_IN_ISR)
// // 	_vTxWorkISR();
// // #else
// // 	_vTxWorkPending();
// // #endif

// // }

// /* size power of 2 */
// #define UART_RX_BUFFER_SIZE   32
// #define UART_TX_BUFFER_SIZE   32

// typedef struct {
//   hdl_module_t module;
//   hdl_nvic_irq_n_t iterrupt;
//   uint32_t baudrate;
//   hdl_uart_word_len_t word_len;
//   hdl_uart_parity_t parity;
//   hdl_uart_stop_bits_t stop_bits;

//   uint32_t init_timeout;

//   hdl_transceiver_t *transceiver;
//   uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
//   uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
//   uint8_t rx_tail;
//   uint8_t tx_tail;
//   uint8_t rx_head;
//   uint8_t tx_head;
//   uint32_t idleTimer;
//   __LinkedListObject__;
// } hdl_uart_private_t;

// _Static_assert(sizeof(hdl_uart_private_t) == sizeof(hdl_uart_t), "In port_uart.h data structure size of hdl_uart_t doesn't match, check UART_PRIVATE_SIZE");

// #define buf_get_count(buf_sz, tail, head)               ((head - tail) & (buf_sz - 1))
// #define buf_get_free(buf_sz, tail, head)                (buf_sz - 1 - buf_get_count(buf_sz, tail, head))
// #define buf_move_tail(buf_sz, tail, head, count)        { uint8_t cnt = MIN(buf_get_count(buf_sz, tail, head), count); 
//                                                           tail = (tail + cnt) & (buf_sz - 1); }
// #define buf_move_head(buf_sz, tail, head, count)        { uint8_t cnt = MIN(buf_get_free(buf_sz, tail, head), count); 
//                                                           head = (head + cnt) & (buf_sz - 1); }
// #define buf_get_non_segmented_data(buf_sz, tail, head)  ((head > tail)? (head - tail): (UART_RX_BUFFER_SIZE - tail))
// #define buf_get_non_segmented_buf(buf_sz, tail, head)   ((head < tail)? (tail - head - 1): (UART_RX_BUFFER_SIZE - head - (tail == 0)? 1: 0))

// static void _uart_handler(LinkedListItem_t *uart_item, void *arg) {
//   hdl_uart_private_t *uart = linked_list_get_object(hdl_uart_private_t, uart_item);
//   if(uart->transceiver != NULL) {
//     if(uart->transceiver->rx_data != NULL) {
//       uint8_t rx_count = buf_get_count(UART_RX_BUFFER_SIZE, uart->rx_tail, uart->rx_head);
//       while(rx_count) {
//         uint8_t buf_len = buf_get_non_segmented_data(UART_RX_BUFFER_SIZE, uart->rx_tail, uart->rx_head);
//         uint32_t readed = uart->transceiver->rx_data(uart->transceiver->proto_context,
//           uart->rx_buffer + uart->rx_tail, buf_len);
//         readed = MIN(readed, buf_len);
//         buf_move_tail(UART_RX_BUFFER_SIZE, uart->rx_tail, uart->rx_head, readed);
//         rx_count = buf_get_count(UART_RX_BUFFER_SIZE, uart->rx_tail, uart->rx_head);
//       }
//     }
//     if(uart->transceiver->tx_empty != NULL) {
//       uint8_t tx_free = buf_get_free(UART_TX_BUFFER_SIZE, uart->tx_tail, uart->tx_head);
//       if(tx_free) {
//         uint8_t buf_len = buf_get_non_segmented_buf(UART_TX_BUFFER_SIZE, uart->tx_tail, uart->rx_head);
//         uint32_t writed = uart->transceiver->tx_empty(uart->transceiver->proto_context,
//           uart->tx_buffer + uart->tx_head, buf_len);
//         writed = MIN(writed, buf_len);
//         buf_move_head(UART_TX_BUFFER_SIZE, uart->rx_tail, uart->rx_head, writed);
//         if(USART_STAT((uint32_t)uart->module.reg) & (USART_STAT_TBE | USART_STAT_TC)) { //??? TODO: run interrupt if no shifting
//           USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
//         }
//       }
//     }
//   }
//   /* TODO: process eot*/
// }

// static uint8_t _uart_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   LinkedList_t uarts = (LinkedList_t)arg;
//   linked_list_do_foreach(uarts, &_uart_handler, NULL);
//   return cancel;
// }

// static void _rst_uart_status(hdl_uart_private_t *uart) {
// 	__IO uint32_t tmpreg;
// 	tmpreg = USART_STAT((uint32_t)uart->module.reg);
// 	tmpreg = USART_DATA((uint32_t)uart->module.reg);
// 	(void)tmpreg;
// }

// static void event_uart_isr(uint32_t event, void *sender, void *context) {
//   hdl_uart_private_t *uart = (hdl_uart_private_t *)context;
//   uint32_t state = USART_STAT((uint32_t)uart->module.reg);
//   uint32_t cr0 = USART_CTL0((uint32_t)uart->module.reg);
// 	if (state & USART_STAT_IDLEF) {
// 		USART_CTL0((uint32_t)uart->module.reg) &= ~USART_CTL0_IDLEIE;
//     //TODO: clear tx, set eot
// 		uart->idleTimer = 0;
// 	}
// 	if ((state & StateMask) == NoError) {
// 		/* UART in mode Receiver ---------------------------------------------------*/
// 		if (state & USART_STAT_RBNE) {
// 			uint16_t data = USART_DATA((uint32_t)uart->module.reg);
//       USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_IDLEIE;
//       uint8_t rx_free = buf_get_free(UART_RX_BUFFER_SIZE, uart->rx_tail, uart->rx_head);
//       uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
//       if(rx_free >= wl) {
//         while(wl--) {
//           uart->rx_buffer[uart->rx_head] = data;
//           buf_move_head(UART_TX_BUFFER_SIZE, uart->rx_tail, uart->rx_head, 1);
//           data >>= 8;
//         }
//       }
// 		}
// 		///* UART in mode Transmitter ------------------------------------------------*/
// 		if ((state & USART_STAT_TBE) && (cr0 & USART_CTL0_TBEIE)) {
//       uint8_t tx_av = buf_get_count(UART_TX_BUFFER_SIZE, uart->tx_tail, uart->tx_head);
//       uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
//       if(tx_av >= wl) {
//         uint16_t data = 0;
//         while(wl--){
//           data |= ((uint16_t)uart->tx_buffer[uart->tx_tail]) << ((1 - wl) * 8);
//           buf_move_tail(UART_TX_BUFFER_SIZE, uart->tx_tail, uart->tx_head, 1);
//         }
//         USART_DATA((uint32_t)uart->module.reg) = data;
//       }
//       USART_CTL0((uint32_t)uart->module.reg) &= ~USART_CTL0_TBEIE;
// 			USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TCIE;
// 		}
//     /* UART in mode Transmitter end --------------------------------------------*/
// 		if ((state & USART_STAT_TC) && (cr0 & USART_CTL0_TCIE)) {
//       USART_STAT((uint32_t)uart->module.reg) &= ~USART_STAT_TC;
// 			USART_CTL0((uint32_t)uart->module.reg) &= ~USART_CTL0_TCIE; 
// 			USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
// 		}
// 	}
// 	else {
// 		_rst_uart_status(uart);
// 	}
// }

// hdl_module_state_t hdl_uart(void *desc, uint8_t enable) {
//   static coroutine_desc_static_t uart_task_buf;
//   static LinkedList_t uarts = NULL;
//   hdl_uart_private_t *uart = (hdl_uart_private_t*)desc;
//   rcu_periph_enum rcu;
//   uint8_t is_usart = HDL_TRUE;
//   switch ((uint32_t)uart->module.reg) {
//     case USART0:
//     rcu = RCU_USART0;
//     break;
//     case USART1:
//     rcu = RCU_USART1;
//     break;
//     case USART2:
//     rcu = RCU_USART2;
//     break;
//     case UART3:
//     rcu = RCU_UART3;
//     is_usart = HDL_FALSE;
//     break;
//     case UART4:
//     rcu = RCU_UART4;
//     is_usart = HDL_FALSE;
//     break;
//     default:
//     return HDL_MODULE_FAULT;
//   }
//   usart_deinit((uint32_t)uart->module.reg);

//   if(enable) {
//     rcu_periph_clock_enable(rcu);
//     usart_baudrate_set((uint32_t)uart->module.reg, uart->baudrate);
//     usart_word_length_set((uint32_t)uart->module.reg, uart->word_len);
//     usart_parity_config((uint32_t)uart->module.reg, uart->parity);
//     usart_stop_bit_set((uint32_t)uart->module.reg, uart->stop_bits);
//     usart_transmit_config((uint32_t)uart->module.reg, USART_TRANSMIT_ENABLE);
//     usart_receive_config((uint32_t)uart->module.reg, USART_RECEIVE_ENABLE);
//     usart_enable((uint32_t)uart->module.reg);
//     linked_list_insert_last(&uarts, linked_list_item(uart));
//     coroutine_add_static(&uart_task_buf, &_uart_worker, (void *)uarts);
//     uart->transceiver = NULL;
//     uart->rx_tail = 0;
//     uart->tx_tail = 0;
//     uart->rx_head = 0;
//     uart->tx_head = 0;
//     uart->idleTimer = 0;
//     hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->module.dependencies[3];
//     if(hdl_interrupt_request(ic, uart->iterrupt, &event_uart_isr, desc))
//       return HDL_MODULE_ACTIVE;
//   }
//   linked_list_unlink(linked_list_item(uart));
//   rcu_periph_clock_disable(rcu);
//   return HDL_MODULE_UNLOADED;
// }

// void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver) {
//   if(uart != NULL) {
//     ((hdl_uart_private_t*)uart)->transceiver = transceiver;
//   }
// }













#include "hdl_portable.h"
#include "CodeLib.h"

// typedef enum {
// 	NoError      = 0,
// 	ParityError  = USART_STAT_PERR,
// 	FrameError   = USART_STAT_FERR,
// 	NoiseError   = USART_STAT_NERR,
// 	OverRunError = USART_STAT_ORERR,
// 	StateMask    = ParityError | FrameError | NoiseError | OverRunError
// } UartError_t;

// typedef struct {
//   hdl_module_t module;
//   hdl_nvic_irq_n_t iterrupt;
//   uint32_t baudrate;
//   hdl_uart_word_len_t word_len;
//   hdl_uart_parity_t parity;
//   hdl_uart_stop_bits_t stop_bits;
//   /* private */
//   hdl_delegate_t uart_isr;
//   hdl_transceiver_t *transceiver;
//   __linked_list_object__;
// } hdl_uart_private_t;

// _Static_assert(sizeof(hdl_uart_private_t) == sizeof(hdl_uart_t), "In port_uart.h data structure size of hdl_uart_t doesn't match, check UART_PRIVATE_SIZE");

// static void _uart_handler(linked_list_item_t *uart_item, void *arg) {
//   hdl_uart_private_t *uart = linked_list_get_object(hdl_uart_private_t, uart_item);
//   if((uart->transceiver != NULL) && !(USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_TBEIE)) {
//     uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
//     if((uart->transceiver->tx_available != NULL) && (uart->transceiver->tx_empty != NULL) && 
//        (uart->transceiver->tx_available(uart->transceiver->proto_context) >= wl)) {
//       USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
//     }
//   }
// }

// static uint8_t _uart_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   linked_list_t uarts = (linked_list_t)arg;
//   linked_list_do_foreach(uarts, &_uart_handler, NULL);
//   return cancel;
// }

// static void _rst_uart_status(hdl_uart_private_t *uart) {
//   USART_INTC((uint32_t)uart->module.reg) |= USART_INTC_PEC | USART_INTC_FEC | USART_INTC_NEC | USART_INTC_OREC;
// 	__IO uint32_t tmpreg;
// 	tmpreg = USART_RDATA((uint32_t)uart->module.reg);
// 	(void)tmpreg;
// }

// static void event_uart_isr(uint32_t event, void *sender, void *context) {
//   hdl_uart_private_t *uart = (hdl_uart_private_t *)context;
//   uint32_t state = USART_STAT((uint32_t)uart->module.reg);
//   uint32_t cr0 = USART_CTL0((uint32_t)uart->module.reg);
// 	if (state & USART_STAT_IDLEF) {
// 		USART_INTC((uint32_t)uart->module.reg) |= USART_INTC_IDLEC;
//     if((uart->transceiver != NULL) && (uart->transceiver->end_of_transmission != NULL))
//       uart->transceiver->end_of_transmission(uart->transceiver->proto_context);
// 	}
// 	if ((state & StateMask) == NoError) {
// 		/* UART in mode Receiver ---------------------------------------------------*/
// 		if (state & USART_STAT_RBNE) {
// 			uint16_t data = USART_RDATA((uint32_t)uart->module.reg);
//       USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_IDLEIE;
//       uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
//       if((uart->transceiver != NULL) && (uart->transceiver->rx_available != NULL) && (uart->transceiver->rx_data != NULL)) {
//         if(uart->transceiver->rx_available(uart->transceiver->proto_context) >= wl) {          
//           uart->transceiver->rx_data(uart->transceiver->proto_context, (uint8_t *)&data, wl);
//         }
//       }
// 		}
// 		///* UART in mode Transmitter ------------------------------------------------*/
// 		if ((state & USART_STAT_TBE) && (cr0 & USART_CTL0_TBEIE)) {
//       USART_CTL0((uint32_t)uart->module.reg) &= ~USART_CTL0_TBEIE;
//       uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
//       if((uart->transceiver != NULL) && (uart->transceiver->tx_available != NULL) && (uart->transceiver->tx_empty != NULL)) {
//         if(uart->transceiver->tx_available(uart->transceiver->proto_context) >= wl) {
//           uint16_t data;
//           uart->transceiver->tx_empty(uart->transceiver->proto_context, (uint8_t *)&data, wl);
//           USART_DATA((uint32_t)uart->module.reg) = data;
//           USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
//         }
//       }
// 		}
// 	}
// 	else {
// 		_rst_uart_status(uart);
// 	}
// }

// static void _usart_baudrate_set(hdl_uart_private_t *uart) {
//   hdl_clock_t *clock = (hdl_clock_t *)uart->module.dependencies[2];
//   uint32_t uclk = 0U, intdiv = 0U, fradiv = 0U, udiv = 0U;
//   uint32_t baudval = uart->baudrate;
//   uint32_t usart_periph = (uint32_t)uart->module.reg;
//   if(USART_CTL0(usart_periph) & USART_CTL0_OVSMOD) {
//       /* oversampling by 8, configure the value of USART_BAUD */
//       udiv = ((2U * clock->freq / clock->div) + baudval / 2U) / baudval;
//       intdiv = udiv & 0x0000fff0U;
//       fradiv = (udiv>>1U) & 0x00000007U;
//       USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
//   }
//   else {
//       /* oversampling by 16, configure the value of USART_BAUD */
//       udiv = (clock->freq / clock->div + baudval / 2U) / baudval;
//       intdiv = udiv & 0x0000fff0U;
//       fradiv = udiv & 0x0000000fU;
//       USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
//   }
// }

// hdl_module_state_t hdl_uart(void *desc, uint8_t enable) {
//   static coroutine_desc_static_t uart_task_buf;
//   static linked_list_t uarts = NULL;
//   hdl_uart_private_t *uart = (hdl_uart_private_t*)desc;
//   rcu_periph_enum rcu;
//   switch ((uint32_t)uart->module.reg) {
//     case USART0:
//     rcu = RCU_USART0;
//     break;
//     case USART1:
//     rcu = RCU_USART1;
//     break;
//     default:
//     return HDL_MODULE_FAULT;
//   }
//   usart_deinit((uint32_t)uart->module.reg);

//   if(enable) {
//     rcu_periph_clock_enable(rcu);
//     _usart_baudrate_set(uart);
//     usart_word_length_set((uint32_t)uart->module.reg, uart->word_len);
//     usart_parity_config((uint32_t)uart->module.reg, uart->parity);
//     usart_stop_bit_set((uint32_t)uart->module.reg, uart->stop_bits);
//     usart_transmit_config((uint32_t)uart->module.reg, USART_TRANSMIT_ENABLE);
//     usart_receive_config((uint32_t)uart->module.reg, USART_RECEIVE_ENABLE);
//     USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_RBNEIE | USART_CTL0_PERRIE;
//     USART_CTL2((uint32_t)uart->module.reg) |= USART_CTL2_ERRIE;
//     usart_enable((uint32_t)uart->module.reg);
//     linked_list_insert_last(&uarts, linked_list_item(uart));
//     coroutine_add_static(&uart_task_buf, &_uart_worker, (void *)uarts);
//     uart->transceiver = NULL;
//     hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->module.dependencies[3];
//     uart->uart_isr.context = desc;
//     uart->uart_isr.handler = &event_uart_isr;
//     if(hdl_interrupt_request(ic, uart->iterrupt, &uart->uart_isr))
//       return HDL_MODULE_ACTIVE;
//   }
//   linked_list_unlink(linked_list_item(uart));
//   rcu_periph_clock_disable(rcu);
//   return HDL_MODULE_UNLOADED;
// }

// void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver) {
//   if(uart != NULL) {
//     ((hdl_uart_private_t*)uart)->transceiver = transceiver;
//   }
// }

#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t uart_isr;
  const hdl_transceiver_t *transceiver;
  coroutine_t worker;
} hdl_uart_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_uart_var_t, *((hdl_uart_mcu_t *)0)->obj_var, HDL_UART_VAR_SIZE, port_uart.h);

__STATIC_INLINE uint8_t _hdl_uart_word_len(hdl_uart_mcu_t *uart) {
  return ((uart->config->word_len == USART_WL_9BIT) && (uart->config->parity==USART_PM_NONE))? 2: 1;
}

static uint8_t _uart_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *) arg;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  if((uart_var->transceiver != NULL) && !(USART_CTL0((uint32_t)uart->config->phy) & USART_CTL0_TBEIE)) {
    if((uart_var->transceiver->tx_available != NULL) && (uart_var->transceiver->tx_empty != NULL) && 
       (uart_var->transceiver->tx_available(uart_var->transceiver->transmitter_context) >= _hdl_uart_word_len(uart))) {
      usart_interrupt_enable((uint32_t)uart->config->phy, USART_INT_TBE);
    }
  }
  return cancel;
}

static void _rst_uart_status(hdl_uart_mcu_t *uart) {
  __IO uint32_t tmpreg;
  tmpreg = USART_STAT(uart->config->phy);
  tmpreg = USART_DATA(uart->config->phy);
  (void)tmpreg;
}

static void event_uart_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *)context;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  uint32_t periph = (uint32_t)uart->config->phy;
	if (usart_interrupt_flag_get(periph, USART_INT_FLAG_IDLE)) {
    usart_interrupt_flag_clear(periph, USART_INT_FLAG_IDLE);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->end_of_transmission != NULL))
      uart_var->transceiver->end_of_transmission(uart_var->transceiver->receiver_context);
	}
	if (usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_NERR) ||
      usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_FERR) ||
      usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_ORERR)) {
      _rst_uart_status(uart);
      return;
  }
  /* UART in mode Receiver ---------------------------------------------------*/
  if (usart_interrupt_flag_get(periph, USART_INT_FLAG_RBNE)) {
    uint16_t data = usart_data_receive(periph);
    usart_interrupt_enable(periph, USART_INT_IDLE);
    uint8_t wl = _hdl_uart_word_len(uart);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->rx_available != NULL) && (uart_var->transceiver->rx_data != NULL)) {
      if(uart_var->transceiver->rx_available(uart_var->transceiver->receiver_context) >= wl) {          
        uart_var->transceiver->rx_data(uart_var->transceiver->receiver_context, (uint8_t *)&data, wl);
      }
    }
  }
  ///* UART in mode Transmitter ------------------------------------------------*/
  if (usart_interrupt_flag_get(periph, USART_INT_FLAG_TBE)) {
    usart_interrupt_disable(periph, USART_INT_TBE);
    uint8_t wl = _hdl_uart_word_len(uart);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->tx_available != NULL) && (uart_var->transceiver->tx_empty != NULL)) {
      if(uart_var->transceiver->tx_available(uart_var->transceiver->transmitter_context) >= wl) {
        uint16_t data;
        uart_var->transceiver->tx_empty(uart_var->transceiver->transmitter_context, (uint8_t *)&data, wl);
        usart_data_transmit(periph, data);
        USART_CTL0(periph) |= USART_CTL0_TBEIE;
      }
    }
  }
}

static hdl_module_state_t _hdl_uart(const void *desc, uint8_t enable) {
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t*)desc;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  uint32_t periph = (uint32_t)uart->config->phy;
  usart_deinit(periph);
  if(enable) {
    rcu_periph_clock_enable(uart->config->rcu);
    usart_baudrate_set(periph, uart->config->baudrate);
    usart_word_length_set(periph, uart->config->word_len);
    usart_parity_config(periph, uart->config->parity);
    usart_stop_bit_set(periph, uart->config->stop_bits);
    usart_transmit_config(periph, USART_TRANSMIT_ENABLE);
    usart_receive_config(periph, USART_RECEIVE_ENABLE);
    coroutine_add(&uart_var->worker, &_uart_worker, uart);
    uart_var->transceiver = NULL;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->dependencies[3];
    uart_var->uart_isr.context = uart;
    uart_var->uart_isr.handler = &event_uart_isr;
    hdl_event_subscribe(&uart->config->interrupt->event, &uart_var->uart_isr);
    hdl_interrupt_request(ic, uart->config->interrupt);
    usart_interrupt_enable(periph, USART_INT_PERR);
    usart_interrupt_enable(periph, USART_INT_RBNE);
    usart_interrupt_enable(periph, USART_INT_ERR);
    // USART_CTL0(periph) |= USART_CTL0_RBNEIE | USART_CTL0_PERRIE;
    // USART_CTL2(periph) |= USART_CTL2_ERRIE;
    usart_enable(periph);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&uart_var->worker);
  rcu_periph_clock_disable(uart->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_uart_set_transceiver(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  (void)channel_id;
  if(desc != NULL) {
    hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *) desc;
    hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;  
    uart_var->transceiver = transceiver;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_uart_iface_t hdl_uart_iface = {
  .init = &_hdl_uart,
  .transceiver_set = &_hdl_uart_set_transceiver
};
