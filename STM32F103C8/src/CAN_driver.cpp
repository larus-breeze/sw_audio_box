/***********************************************************************//**
 * @file    	CAN_driver.cpp
 * @brief   	Low-level CAN bus driver
 * @author	Dr. Klaus Schaefer
 * @copyright 	Copyright 2021 Dr. Klaus Schaefer. All rights reserved.
 * @license 	This project is released under the GNU Public License GPL-3.0

    <Larus Flight Sensor Firmware>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 **************************************************************************/

#include "system_configuration.h"
#include "CAN.h"

#if ACTIVATE_CAN

#define CANx                           CAN1
#define CANx_CLK_ENABLE()              __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()

#define CANx_FORCE_RESET()             __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()           __HAL_RCC_CAN1_RELEASE_RESET()

#ifdef CAN_PB8_PB9
#define CANx_TX_PIN                    GPIO_PIN_9
#define CANx_TX_GPIO_PORT              GPIOB
#define CANx_RX_PIN                    GPIO_PIN_8
#define CANx_RX_GPIO_PORT              GPIOB
#define CANx_AFIO_REMAP_RX_TX_PIN()    __HAL_AFIO_REMAP_CAN1_2()
#else
/* Definition for CANx Pins */
#define CANx_TX_PIN                    GPIO_PIN_12
#define CANx_TX_GPIO_PORT              GPIOA
#define CANx_RX_PIN                    GPIO_PIN_11
#define CANx_RX_GPIO_PORT              GPIOA
#define CANx_AFIO_REMAP_RX_TX_PIN()    __HAL_AFIO_REMAP_CAN1_1()
#endif

/* Definition for CANx AFIO Remap */
#define CANx_AFIO_REMAP_CLK_ENABLE()   __HAL_RCC_AFIO_CLK_ENABLE()

CAN_HandleTypeDef CanHandle;

Queue < CAN_packet > CAN_RX_queue(10,"CAN_RX");

void CAN_init (void);

/** @brief Global CAN send function */
bool CAN_send( const CAN_packet &p)
{
  CAN_TxHeaderTypeDef TxHeader;
  uint32_t TxMailbox;

  TxHeader.StdId = p.id;
//TxHeader.ExtId = 0x00;
  TxHeader.RTR = p.is_remote ? CAN_RTR_REMOTE : CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = p.dlc;
  TxHeader.TransmitGlobalTime = DISABLE;

  return HAL_CAN_AddTxMessage (&CanHandle, &TxHeader, (uint8_t*) p.data_b, &TxMailbox) == HAL_OK;
}

unsigned CAN_init_done( false);

/** @brief CAN driver initialization
 *
 * To be called at least once on system start */
void CAN_init (void)
{
  if( __sync_fetch_and_or( &CAN_init_done, true))
    return; // call me only once

  GPIO_InitTypeDef GPIO_InitStruct;

  CANx_CLK_ENABLE ();
  CANx_GPIO_CLK_ENABLE ();
  CANx_AFIO_REMAP_CLK_ENABLE ();
  CANx_AFIO_REMAP_RX_TX_PIN ();

  GPIO_InitStruct.Pin = CANx_TX_PIN;
#if CAN_OPEN_DRAIN
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
#else
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
#endif
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init (CANx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* CAN1 RX GPIO pin configuration */
  GPIO_InitStruct.Pin = CANx_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init (CANx_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC #################################################*/
  /* NVIC configuration for CAN1 Reception complete interrupt */
  HAL_NVIC_SetPriority (USB_HP_CAN1_TX_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ (USB_HP_CAN1_TX_IRQn);
  HAL_NVIC_SetPriority (USB_LP_CAN1_RX0_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ (USB_LP_CAN1_RX0_IRQn);
  HAL_NVIC_SetPriority (CAN1_RX1_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ (CAN1_RX1_IRQn);
  HAL_NVIC_SetPriority (CAN1_SCE_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ (CAN1_SCE_IRQn);

  CAN_FilterTypeDef sFilterConfig;

  /* Configure the CAN peripheral */
  CanHandle.Instance = CAN1;

  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CanHandle.Init.TimeSeg1 = CAN_BS1_6TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ;
  CanHandle.Init.Prescaler = 4;

  if (HAL_CAN_Init (&CanHandle) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler ();
    }

  /* Configure the CAN Filter */
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter (&CanHandle, &sFilterConfig) != HAL_OK)
    {
      /* Filter configuration Error */
      Error_Handler ();
    }

  /* Start the CAN peripheral */
  if (HAL_CAN_Start (&CanHandle) != HAL_OK)
    {
      /* Start Error */
      Error_Handler ();
    }

  /* Activate CAN RX notification */
  if (HAL_CAN_ActivateNotification (&CanHandle,
	CAN_IT_RX_FIFO0_MSG_PENDING |
	CAN_IT_RX_FIFO0_FULL        |
	CAN_IT_RX_FIFO0_OVERRUN     |
	CAN_IT_RX_FIFO1_MSG_PENDING |
	CAN_IT_RX_FIFO1_FULL        |
	CAN_IT_RX_FIFO1_OVERRUN     |
	CAN_IT_ERROR_WARNING        |
	CAN_IT_ERROR_PASSIVE        |
	CAN_IT_BUSOFF     	    |
	CAN_IT_LAST_ERROR_CODE	    |
	CAN_IT_ERROR		    )
      != HAL_OK)
    {
      /* Notification Error */
      Error_Handler ();
    }
}


extern "C" void USB_HP_CAN1_TX_IRQHandler( void)
{
  HAL_CAN_IRQHandler (&CanHandle);
}
extern "C" void USB_LP_CAN1_RX0_IRQHandler( void)
{
  HAL_CAN_IRQHandler (&CanHandle);
}
extern "C" void CAN1_RX1_IRQHandler( void)
{
  HAL_CAN_IRQHandler (&CanHandle);
}
extern "C" void CAN1_SCE_IRQHandler( void)
{
  HAL_CAN_IRQHandler (&CanHandle);
}


void
HAL_CAN_ErrorCallback (CAN_HandleTypeDef *hcan)
{
//  asm("bkpt 0"); path ignored
}

void
HAL_CAN_TxMailbox0CompleteCallback (CAN_HandleTypeDef *hcan)
{
  asm("bkpt 0");
}
void
HAL_CAN_TxMailbox1CompleteCallback (CAN_HandleTypeDef *hcan)
{
  asm("bkpt 0");
}
void
HAL_CAN_TxMailbox2CompleteCallback (CAN_HandleTypeDef *hcan)
{
  asm("bkpt 0");
}


void
HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef *hcan)
{
  CAN_packet p;
  CAN_RxHeaderTypeDef header;
  HAL_CAN_GetRxMessage( hcan, 0, &header, &(p.data_b[0]));
  p.id=header.StdId;
  p.dlc=header.DLC;
  p.is_remote=header.RTR != 0 ? 1 : 0;
  bool result = CAN_RX_queue.send_from_ISR(p);
  ASSERT( result);
}

void
HAL_CAN_RxFifo0FullCallback (CAN_HandleTypeDef *hcan)
{
//  asm("bkpt 0"); // patch
}

void
HAL_CAN_RxFifo1MsgPendingCallback (CAN_HandleTypeDef *hcan)
{
  CAN_packet p;
  CAN_RxHeaderTypeDef header;
  HAL_CAN_GetRxMessage( hcan, 0, &header, &(p.data_b[0]));
  p.id=header.StdId;
  p.dlc=header.DLC;
  p.is_remote=header.RTR != 0 ? 1 : 0;
  bool result = CAN_RX_queue.send_from_ISR(p);
  ASSERT( result);
}

#endif
