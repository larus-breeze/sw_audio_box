/**
 * @file    uart_communicator.cpp
 * @brief   USART communication tests
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"

#define BUFSIZE 20
static ROM uint8_t text[]="Hello\r\n";

#if ACTIVATE_USART_1
#include "usart_1.h"

uint8_t usart_1_buf[BUFSIZE];

static void usart_1_runnable( void *)
{
  volatile HAL_StatusTypeDef res;
  USART_1_Init(115200);
  UART_1_init_DMA();

  HAL_UART_Transmit_DMA( &USART_1_handle, (uint8_t *)text, 7);
  delay(10);

  while( true)
    {
      res=HAL_UART_Receive_DMA( &USART_1_handle, (uint8_t *)usart_1_buf, 4);
      suspend();
    }
}

Task usart_1_communicator( usart_1_runnable, "UART", 256);

#endif

#if ACTIVATE_USART_2
#include "usart_2.h"

uint8_t usart_2_buf[BUFSIZE];

static void usart_2_runnable( void *)
{
  volatile HAL_StatusTypeDef res;
  USART_2_Init(115200);
  UART_2_init_DMA();

  HAL_UART_Transmit_DMA( &USART_2_handle, (uint8_t *)text, 7);
  delay(10);

  while( true)
    {
      res=HAL_UART_Receive_DMA( &USART_2_handle, (uint8_t *)usart_2_buf, 4);
      suspend();
    }
}

Task usart_2_communicator( usart_2_runnable, "UART", 256);

#endif
