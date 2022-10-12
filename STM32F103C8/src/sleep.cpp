/**
 * @file    sleep.cpp
 * @brief   simple example for core sleep mode
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"

#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_bus.h"

#if USE_SLEEP_PA0

void sleep_enable( void *)
{
  delay(10000);
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  HAL_PWR_EnterSTANDBYMode();
  asm("bkpt 0");
  while(true);
}

Task sleepy( sleep_enable);

#endif
