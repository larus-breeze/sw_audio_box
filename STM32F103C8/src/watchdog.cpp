/**
 * @file    watchdog.cpp
 * @brief   MPU watchdog module
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
#include "FreeRTOS_wrapper.h"
#include "system_configuration.h"

#if USE_WATCHDOG

static WWDG_HandleTypeDef   WwdgHandle;

void watchdog_runnable( void *)
{
  __HAL_RCC_WWDG_CLK_ENABLE();

  WwdgHandle.Instance = WWDG;
  WwdgHandle.Init.Prescaler = WWDG_PRESCALER_8;
  WwdgHandle.Init.Window    = 80U;  // earliest reset after (127-80)*0.8ms = 38ms
  WwdgHandle.Init.Counter   = 127U; // latest after (127-64)*0.8ms = 50ms
  WwdgHandle.Init.EWIMode   = WWDG_EWI_ENABLE;

  if (HAL_WWDG_Init(&WwdgHandle) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriority (WWDG_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ (WWDG_IRQn);

  for( Synchronous_Timer t(55); true; )
    {
      t.sync();
      if (HAL_WWDG_Refresh(&WwdgHandle) != HAL_OK)
      {
        Error_Handler();
      }
    }
}
extern "C" void WWDG_IRQHandler( void)
{
    HAL_WWDG_IRQHandler(&WwdgHandle);
}

Task watch( watchdog_runnable, "WDOG");

#endif
