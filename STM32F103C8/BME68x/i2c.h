/***********************************************************************//**
 * @file		i2c.h
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

#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdbool.h>

extern I2C_HandleTypeDef hi2c1;

/**/
#define I2C1_SCL_GPIOPIN GPIO_PIN_6
#define I2C1_SCL_GPIOX  GPIOB
#define I2C1_SDA_GPIOPIN GPIO_PIN_7
#define I2C1_SDA_GPIOX  GPIOB


typedef enum
{
  I2C_OK       = 0x00U,
  I2C_ERROR    = 0x01U
} I2C_StatusTypeDef;

I2C_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c);
I2C_StatusTypeDef I2C_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
I2C_StatusTypeDef I2C_ReadRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
I2C_StatusTypeDef I2C_WriteRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
I2C_StatusTypeDef I2C_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

#ifdef __cplusplus
}
#endif


#endif /* I2C_H_ */
