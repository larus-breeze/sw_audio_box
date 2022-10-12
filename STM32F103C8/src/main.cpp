/***********************************************************************//**
 * @file    	main.cpp
 * @brief   	System startup
 * @author	Dr. Klaus Schaefer
 * @copyright 	Copyright 2021 Dr. Klaus Schaefer. All rights reserved.
 * @mainpage
 * Basic software environment for STM32F103 I/O modules
 * Target Hardware: SMT32F103RG-nucleo and "BluePill"-hardware

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

void SystemClock_Config (void);
void check_core (void);

void
read_mpu_identifier(void);

uint16_t size; //!< memory size from MPU core
uint32_t unique_id[4]; // unique identifier of this individual MPU
uint16_t unique_id_hash;

/** @brief  The application entry point. */
int
main (void)
{
  HAL_Init ();

  SystemClock_Config ();

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

//  check_core(); // my be used to spot fake micro-controllers

  read_mpu_identifier ();
//trace_printf( "Trace: running on instance %04lx %04lx %08lx %08lx\n", unique_id[0],unique_id[1],unique_id[2],unique_id[3]);

  void MX_I2C1_Init(void);

#if ACTIVATE_OAT_SENSOR
  MX_I2C1_Init();
#endif

  asm ("b vTaskStartScheduler");
}

/** @brief sleep CPU while idling */
extern "C" void vApplicationIdleHook( void)
{
  __WFI();
}

void
HAL_Delay (uint32_t Delay)
{
  delay (Delay);
}

/** @brief System Clock Configuration -> 72 MHz */
void
SystemClock_Config (void)
{
  RCC_ClkInitTypeDef clkinitstruct =
    { 0 };
  RCC_OscInitTypeDef oscinitstruct =
    { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.HSIState = RCC_HSI_ON;
  oscinitstruct.PLL.PLLState = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;


  if (HAL_RCC_OscConfig (&oscinitstruct) != HAL_OK)
    {
      /* Start Conversation Error */
      Error_Handler ();
    }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
   clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig (&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
    {
      /* Start Conversation Error */
      Error_Handler ();
    }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }
}

volatile uint64_t SystemTicks; // global time counter (1 ms)

/**
 * @brief Tick Hook: Callback for system Tick ISR
 *
 * do SystemTicks timekeeping
 */
#if USE_FREE_RTOS
extern "C" void
vApplicationTickHook (void)
{
  ++SystemTicks;
  HAL_IncTick ();
}
#else
extern "C" void xPortSysTickHandler( void )
{
  ++SystemTicks;
  HAL_IncTick();
}
extern "C" void vApplicationTickHook( void)
{} // just a stub for the linker
#endif

/** @brief read precise system time in microsecond units */
uint64_t
getTime_usec (void)
{
  uint64_t time;
  uint64_t present_systick;
  uint64_t reload = (*(uint32_t*) 0xe000e014) + 1;

  do
    {
      time = __LDREXW ((uint32_t*) &SystemTicks);
      present_systick = (uint64_t) (*(uint32_t*) 0xe000e018);
    }
  while (__STREXW ((uint32_t) time, (uint32_t*) &SystemTicks) != 0)

  ; // milliseconds -> microseconds
  time *= 1000;

  present_systick = reload - present_systick; // because it's a down-counter
  present_systick *= 1000; // millisecs -> microsecs
  present_systick /= reload;

  return time + present_systick;
}

void read_mpu_identifier(void)
  {
    size = * (uint16_t *)0x1ffff7e0;
    unique_id[0] = * (uint16_t *)0x1ffff7e8;
    unique_id[1] = * (uint16_t *)(0x1ffff7e8+2);
    unique_id[2] = * (uint32_t *)(0x1ffff7e8+4);
    unique_id[3] = * (uint32_t *)(0x1ffff7e8+8);
    uint32_t tmp=unique_id[0] ^ unique_id[1] ^ unique_id[2] ^ unique_id[3];
    unique_id_hash=tmp ^ (tmp >> 16);
  }
/**
 * @brief check for fake micro-controller
 */
void
check_core (void)
{
  uint32_t debugmcu = *(uint32_t*) 0xe0042000;

#if 0
  if( debugmcu != 0x20036410)
    while( true)
      asm("bkpt 0");
  #else
  if (debugmcu != 0 /* 0x20036410 */)
    while (true)
      asm("bkpt 0");
#endif
}

/**
 * @brief  This function is executed in case of error occurrence.
 */
void
Error_Handler (void)
{
  asm("bkpt 0");
}

/** @brief emergency brake for software-checked stack overflow */
extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
  asm("bkpt 0");
}


#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 */
void
assert_failed (uint8_t *file, uint32_t line)
{
  asm("bkpt 0");
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
