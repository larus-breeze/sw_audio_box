/**
 * @file    system_configuration.h
 * @brief   system tweaks, project configuration settins
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#ifndef SYSTEM_CONFIGURATION_H_
#define SYSTEM_CONFIGURATION_H_

#include "main.h"
#include "FreeRTOS_wrapper.h"

extern uint16_t unique_id_hash;

#define USE_FREE_RTOS		1

#if 0 // for Nucleo board
#define LED_PORT 		GPIOA
#define LED_PIN 		GPIO_PIN_5
#define LED_PORT_CLOCK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUTTON_ON_PA0		1 // blue pill
#define BUTTON_ON_PC13		0 // nucleo
#else // for Blue Pill board:
#define BUTTON_ON_PA0		0 // blue pill
#define BUTTON_ON_PC13		1 // nucleo
#define LED_PORT 		GPIOC
#define LED_PIN 		GPIO_PIN_13
#define LED_PORT_CLOCK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#endif

#define RUN_AUDIO_TEST		0
#define RUN_AUDIO_CONTROLLER	1
#define ACTIVATE_OAT_SENSOR	0
#define RUN_BUTTON		0

#define ACTIVATE_CAN 		1
#define CAN_OPEN_DRAIN		0
#define CAN_PB8_PB9		1
#define RUN_CAN_TRANSMITTER	0
#define RUN_CAN_RECEIVER	0

#define SUICIDE_STACKOVERFLOW 	0
#define RUN_CAN_DISTRIBUTION_TEST 0

#define ACTIVATE_BLINKER 	1
#define USE_WATCHDOG		0
#define USE_SLEEP_PA0		0

#define ACTIVATE_USART_1	0
#define ACTIVATE_USART_2	0
#define USART_DMA		1

#endif /* SYSTEM_CONFIGURATION_H_ */
