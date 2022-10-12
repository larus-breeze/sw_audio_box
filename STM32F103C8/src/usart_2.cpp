/**
 * @file    usart_2.cpp
 * @brief   settings and defs for usart 2 driver
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"
#include "main.h"
#include "stm32f1xx_hal.h"

#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

#if USART_DMA

#define UART_x_init_DMA UART_2_init_DMA

// handles must be static as there may be more instances
// on the other UART channels
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

/* Definition for USARTx's DMA */
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()

#define USARTx_TX_DMA_CHANNEL             DMA1_Channel7
#define USARTx_RX_DMA_CHANNEL             DMA1_Channel6

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn                DMA1_Channel7_IRQn
#define USARTx_DMA_RX_IRQn                DMA1_Channel6_IRQn
#define USARTx_DMA_TX_IRQHandler          DMA1_Channel7_IRQHandler
#define USARTx_DMA_RX_IRQHandler          DMA1_Channel6_IRQHandler

#endif

UART_HandleTypeDef USART_2_handle;
#define USART_x_handle USART_2_handle

#define USART_x_Init USART_2_Init
#define UART_x_init_DMA UART_2_init_DMA

#include "usart_generic.h"

