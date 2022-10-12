/***********************************************************************//**
 * @file		sensing.cpp
 * @brief		Sensor driver for Bosch BME68x air sensor
 * @author		Dr. Klaus Schaefer
 * @copyright 		Copyright 2021 Dr. Klaus Schaefer. All rights reserved.
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

#include "system_configuration.h"
#include "FreeRTOS_wrapper.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "sensing.h"
#include "i2c.h"
#include "bme68x.h"
#include "CAN.h"

#if ACTIVATE_OAT_SENSOR

I2C_HandleTypeDef hi2c1;
static uint8_t dev_addr;

void
MX_I2C1_Init (void);

/*! I2C read function map */
BME68X_INTF_RET_TYPE
bme68x_i2c_read (uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
		 void *intf_ptr)
{
  uint8_t dev_addr = *(uint8_t*) intf_ptr;

  if (I2C_OK == I2C_ReadRegister (&hi2c1, dev_addr, reg_addr, 1, reg_data, len))
    {
      return BME68X_INTF_RET_SUCCESS;
    }
  else
    {
      return -1;
    }
}

/*! I2C write function map */
BME68X_INTF_RET_TYPE
bme68x_i2c_write (uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
		  void *intf_ptr)
{
  uint8_t dev_addr = *(uint8_t*) intf_ptr;

  if (I2C_OK
      == I2C_WriteRegister (&hi2c1, dev_addr, reg_addr, 1, (uint8_t*) reg_data,
			    len))
    {
      return BME68X_INTF_RET_SUCCESS;
    }
  else
    {
      return -1;
    }
}

/*! Delay function map to FreeRTOS. Delay below 1ms will be rounded up to 1ms. */
void
bme68x_delay_us (uint32_t period, void *intf_ptr)
{
  if (period < 1000)
    {
      vTaskDelay (1);
    }
  else
    {
      vTaskDelay (period / 1000);
    }
}

int8_t
bme68x_interface_init (struct bme68x_dev *bme, uint8_t intf)
{
  int8_t rslt = BME68X_OK;

  if (bme != NULL)
    {
      /* Bus configuration : I2C */
      if (intf == BME68X_I2C_INTF)
	{
	  I2C_Init (&hi2c1);
	  dev_addr = BME68X_I2C_ADDR_HIGH << 1;
	  bme->read = bme68x_i2c_read;
	  bme->write = bme68x_i2c_write;
	  bme->intf = BME68X_I2C_INTF;
	}
      /* Bus configuration : SPI */
      else if (intf == BME68X_SPI_INTF)
	{
	  Error_Handler ();
	}

      bme->delay_us = bme68x_delay_us;
      bme->intf_ptr = &dev_addr;
      bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
    }
  else
    {
      rslt = BME68X_E_NULL_PTR;
    }

  return rslt;
}

template <class type> class IIR_filter
{
public:
  IIR_filter( type _feedback)
    : feedback( _feedback),
      feedforward( 1.0 - feedback),
      old_input( 0.0)
  {};
  type step( type input)
  {
    output = old_input * feedback + input * feedforward;
    old_input = input;
    return output;
  }
  type get_value( void) const
  {
    return output;
  }
private:
  type feedback;
  type feedforward;
  type old_input;
  type output;
};

void StartSensingTask (void *argument)
{
  struct bme68x_dev bme;
  struct bme68x_conf conf;
  struct bme68x_heatr_conf heatr_conf;
  struct bme68x_data data;
  uint8_t n_fields;

  IIR_filter <float> humidity_filter( 0.9f);
  IIR_filter <float> temperature_filter( 0.9f);

  CAN_packet p (0x120, 8);
  while (true) // try initialization again and again
    {
      delay( 100);

      CAN_init ();
      MX_I2C1_Init ();

      if( BME68X_OK != bme68x_interface_init (&bme, BME68X_I2C_INTF))
	continue;
      if( BME68X_OK != bme68x_init (&bme))
	continue;

      conf.filter = BME68X_FILTER_OFF;
      conf.odr = BME68X_ODR_NONE;
      conf.os_hum = BME68X_OS_16X;
      conf.os_pres = BME68X_OS_16X;
      conf.os_temp = BME68X_OS_16X;

      if( BME68X_OK != bme68x_set_conf (&conf, &bme))
	continue;

      heatr_conf.enable = BME68X_DISABLE; /* Enabling this causes to high temperature values for quick consecutive readings*/
      heatr_conf.heatr_temp = 300;
      heatr_conf.heatr_dur = 100;

      if( BME68X_OK != bme68x_set_heatr_conf (BME68X_FORCED_MODE, &heatr_conf, &bme))
	continue;

      if( BME68X_OK != bme68x_set_op_mode (BME68X_FORCED_MODE, &bme))
	continue;

      delay(200);

      unsigned decimation_counter = 0;

      for( Synchronous_Timer t(200); true; t.sync()) /* Enter cyclic measurement mode @ 5 Hz */
	{
	  if( BME68X_OK != bme68x_set_op_mode (BME68X_FORCED_MODE, &bme))
	    break; // re-initialize

	  /* Check if rslt == BME68X_OK, report or handle if otherwise */
	  if (BME68X_OK == bme68x_get_data (BME68X_FORCED_MODE, &data, &n_fields, &bme))
	    {
	      p.data_f[0] = temperature_filter.step( data.temperature);
	      p.data_f[1] = humidity_filter.step( data.humidity * 0.01f); // percent -> float number

	      // send new filtered data @ 1 Hz
	      ++ decimation_counter;
	      if( decimation_counter >= 5)
		{
		  decimation_counter=0;
		  CAN_send (p);
		}
	    }
	  else
	    break; // re-initialize
	}
    }
}

Task BME_test (StartSensingTask, "BME680", 256);

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
void
MX_I2C1_Init (void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init (&hi2c1) != HAL_OK)
    {
      Error_Handler ();
    }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2C MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hi2c: I2C handle pointer
 * @retval None
 */
void
HAL_I2C_MspInit (I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct =
    { 0 };
  if (hi2c->Instance == I2C1)
    {
      /* USER CODE BEGIN I2C1_MspInit 0 */
      __HAL_AFIO_REMAP_I2C1_DISABLE();
      /* USER CODE END I2C1_MspInit 0 */

      __HAL_RCC_GPIOB_CLK_ENABLE();
      /**I2C1 GPIO Configuration
       PB6     ------> I2C1_SCL
       PB7     ------> I2C1_SDA
       */
      GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init (GPIOB, &GPIO_InitStruct);

      /* Peripheral clock enable */
      __HAL_RCC_I2C1_CLK_ENABLE();
      /* I2C1 interrupt Init */
      HAL_NVIC_SetPriority (I2C1_EV_IRQn, 15, 0);
      HAL_NVIC_EnableIRQ (I2C1_EV_IRQn);
      HAL_NVIC_SetPriority (I2C1_ER_IRQn, 15, 0);
      HAL_NVIC_EnableIRQ (I2C1_ER_IRQn);
      /* USER CODE BEGIN I2C1_MspInit 1 */

      /* USER CODE END I2C1_MspInit 1 */
    }

}

/**
 * @brief This function handles I2C1 event interrupt.
 */
extern "C" void
I2C1_EV_IRQHandler (void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler (&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
 * @brief This function handles I2C1 error interrupt.
 */
extern "C" void
I2C1_ER_IRQHandler (void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler (&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}

#endif
