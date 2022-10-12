/***********************************************************************//**
 * @file		i2c.cpp
 * @brief		I2C driver
 * @author		Maximilian Betz
 * @copyright 		Copyright 2021 Maximilian Betz. All rights reserved.
 * @license 		This project is released under the GNU Public License GPL-3.0

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

#include "i2c.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define I2C_DEFAULT_TIMEOUT_MS  100
static QueueHandle_t I2C1_CPL_Message_Id = NULL;


static void I2C1_ResolveStuckSlave(void)
{
  /* Reset a slave which potentially blocks the bus by pulling the data line to low. Very sporadic issue.
   * Can happen if stm32 is reset during an i2c transaction*/
  HAL_I2C_DeInit(&hi2c1);

  /* Configure SDA to input to monitor success*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = I2C1_SDA_GPIOPIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(I2C1_SDA_GPIOX, &GPIO_InitStruct);

  /* Configure SCL to output to generate a pseudo clock signal */
  GPIO_InitStruct.Pin = I2C1_SCL_GPIOPIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  HAL_GPIO_Init(I2C1_SCL_GPIOX, &GPIO_InitStruct);

  /* Generate clock pulses until slave releases the data line */
  while(HAL_GPIO_ReadPin(I2C1_SDA_GPIOX, I2C1_SDA_GPIOPIN) == GPIO_PIN_RESET)
    {
      HAL_GPIO_TogglePin(I2C1_SCL_GPIOX, I2C1_SCL_GPIOPIN);
      vTaskDelay(1);
    }

  /* Initialize I2C for normal operation.*/
  HAL_I2C_Init(&hi2c1);
}

I2C_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c)
{
  I2C_StatusTypeDef status = I2C_OK;
  if (hi2c->Instance == I2C1)
    {
      if (I2C1_CPL_Message_Id == NULL)
	{
	  I2C1_CPL_Message_Id =  xQueueCreate(1, sizeof(I2C_StatusTypeDef));
	  I2C1_ResolveStuckSlave();
	  if (NULL == I2C1_CPL_Message_Id)
	    {
	      status = I2C_ERROR;
	    }
	}
      else
	{
	  I2C1_ResolveStuckSlave();
	  status = I2C_OK; //Already initialized
	}

    }
  else
    {
      status = I2C_ERROR;
      Error_Handler();
    }

  return status;
}


I2C_StatusTypeDef I2C_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;
  BaseType_t queue_status = pdFALSE;
  I2C_StatusTypeDef flag = I2C_ERROR;
  status = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
  if(HAL_OK != status)
    {
      return I2C_ERROR;  // Return false in case HAL did not accept the command.
    }
  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueReceive(I2C1_CPL_Message_Id, &flag, I2C_DEFAULT_TIMEOUT_MS);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }

  if((I2C_OK != flag) | (pdTRUE != queue_status))
    {
      return I2C_ERROR;  // Return false in case queue timeout or false flag value.
    }
  return I2C_OK;
}


I2C_StatusTypeDef I2C_ReadRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;
  BaseType_t queue_status = pdFALSE;
  I2C_StatusTypeDef flag = I2C_OK;
  status = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
  if(HAL_OK != status)
    {
      return I2C_ERROR;  // Return false in case HAL did not accept the command.
    }
  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueReceive(I2C1_CPL_Message_Id, &flag, I2C_DEFAULT_TIMEOUT_MS);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }

  if((I2C_OK != flag) | (pdTRUE != queue_status))
    {
      return I2C_ERROR;  // Return false in case queue timeout or false flag value.
    }
  return I2C_OK;
}


I2C_StatusTypeDef I2C_WriteRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;
  BaseType_t queue_status = pdFALSE;
  I2C_StatusTypeDef flag = I2C_OK;
  status = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
  if(HAL_OK != status)
    {
      return I2C_ERROR;  // Return false in case HAL did not accept the command.
    }
  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueReceive(I2C1_CPL_Message_Id, &flag, I2C_DEFAULT_TIMEOUT_MS);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }


  if((I2C_OK != flag) | (pdTRUE != queue_status))
    {
      return I2C_ERROR;  // Return false in case queue timeout or false flag value.
    }
  return I2C_OK;
}


I2C_StatusTypeDef I2C_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;
  BaseType_t queue_status = pdFALSE;
  I2C_StatusTypeDef flag = I2C_ERROR;
  status = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
  if(HAL_OK != status)
    {
      return I2C_ERROR;  // Return false in case HAL did not accept the command.
    }
  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueReceive(I2C1_CPL_Message_Id, &flag, I2C_DEFAULT_TIMEOUT_MS);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }


  if((I2C_OK != flag) | (pdTRUE != queue_status))
    {
      return I2C_ERROR;  // Return false in case queue timeout or false flag value.
    }
  return I2C_OK;
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_OK;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_OK;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}


void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_OK;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}


void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_OK;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_ERROR;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}


void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
  BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
  BaseType_t queue_status;
  I2C_StatusTypeDef flag = I2C_ERROR;

  if (hi2c->Instance == I2C1)
    {
      queue_status = xQueueSendFromISR(I2C1_CPL_Message_Id, &flag, &xHigherPriorityTaskWokenByPost);
    }
  else
    {
      Error_Handler();   //I2C2 not available
    }
  if(pdTRUE != queue_status){
      Error_Handler();
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
}
