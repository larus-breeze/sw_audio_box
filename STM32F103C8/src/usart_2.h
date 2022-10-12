/**
 * @file    usart_2.h
 * @brief   settings and defs for usart 2 driver: exports
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#ifndef USART_1_H_
#define USART_1_H_

void USART_2_Init(unsigned baudrate);
void UART_2_init_DMA(void);

extern UART_HandleTypeDef USART_2_handle;

#endif /* USART_1_H_ */
