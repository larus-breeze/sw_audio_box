/**
 * @file    usart_1.h
 * @brief   settings and defs for usart 1 driver: exports
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#ifndef USART_1_H_
#define USART_1_H_

void USART_1_Init(unsigned baudrate);
void UART_1_init_DMA(void);

extern UART_HandleTypeDef USART_1_handle;

#endif /* USART_1_H_ */
