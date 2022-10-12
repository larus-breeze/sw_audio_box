/**
 * @file    pieps.cpp
 * @brief   Semi-sine-wave output (Quadrature 2channel Signal)
 *
 * 8 loudness steps via simple R-divider on PA2 .. PA7
 *
 * Resistor assignment:
 * PA0, PA1: 120kOhm
 * PA2 60kOhm
 * PA3 30kOhm
 * PA4 15kOhm
 * PA5 7.5kOhm
 * PA6 3.6kOhm
 * PA7 1.8kOhm
 *
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de

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

 */
#include "main.h"
#include "system_configuration.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "pieps.h"

#define SIGNAL_PERIOD_BASE_VALUE 12000000

TIM_HandleTypeDef htim2;

uint16_t sound_period_length = 1200;
uint8_t amplitude = 0;

ROM uint8_t LOUDNESS_BITS[]={
    0x00, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd
};

//!< set volume using log multiplying DAC
void set_volume( uint16_t volume)
{
  uint32_t odr = GPIOA->ODR; // read and conserve bits > bit 7
  odr &= ~0xff;
  odr |= LOUDNESS_BITS[volume / 8192];
  GPIOA->ODR = odr;
}

//!< activate or deactivate sound timer
void sound_on( bool activated)
{
  if( activated)
    {
      if ( HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1)  != HAL_OK)
        Error_Handler();
      if ( HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2)  != HAL_OK)
        Error_Handler();
    }
  else
    {
      if ( HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_1)  != HAL_OK)
          Error_Handler();
      if ( HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_2)  != HAL_OK)
          Error_Handler();
    }
}

//!< set frequency using timer TIM2 hardware
void set_frequency( uint16_t frequency_Hz)
{
  unsigned count = SIGNAL_PERIOD_BASE_VALUE / frequency_Hz;
  // count = 12000 -> 1kHz
  TIM2->ARR = count;
  TIM2->CCR1 = count/2;
}

//!< initialize the TIM2 sound output module
void init_pieps( void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = 0xff - (GPIO_PIN_0 | GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 2;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = sound_period_length;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    Error_Handler();

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
      Error_Handler();
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
      Error_Handler();

  if ( HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1)  != HAL_OK)
    Error_Handler();
  if ( HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2)  != HAL_OK)
    Error_Handler();
}

#if RUN_AUDIO_TEST

void pieps( void *)
{
  init_pieps();

  uint16_t amplitude = 0;
  uint8_t vibrato_count = 0;
  uint16_t frequency = 250; // Hz

  for( Synchronous_Timer t( 10); true; t.sync())
    {
#if 1 // vibrato
      set_frequency( frequency);
      set_volume( amplitude);
      sound_on( vibrato_count & 4);
#else
      set_frequency( frequency + ((vibrato_count & 2) ? 20 : 0));
      set_volume( amplitude);
#endif
      ++vibrato_count;
      frequency += 10;

      if( frequency > 3000)
	{
	frequency = 250;
	amplitude += 8192;
	if( amplitude >= 32678)
		amplitude = 0;
	}
    }
}

Task pieps_task( pieps, "PIEPS");

#endif
