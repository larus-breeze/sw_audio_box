/**
 * @file    usart_common.cpp
 * @brief   usart interrupt callbacks, just templates
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"
#include "main.h"
#include "stm32f1xx_hal.h"

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *USART_x_handle)
{
//  asm("bkpt 0");
}

extern "C" void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
//  asm("bkpt 0");
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *USART_x_handle)
{
//  asm("bkpt 0");
}

extern "C" void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
//  asm("bkpt 0");
}

extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef *USART_x_handle)
{
    Error_Handler();
}

extern "C" void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
  asm("bkpt 0");
}

extern "C" void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
  asm("bkpt 0");
}

extern "C" void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
  asm("bkpt 0");
}




