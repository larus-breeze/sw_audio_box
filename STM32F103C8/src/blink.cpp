/**
 * @file    blink.cpp
 * @brief   Simple LED blinker
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "main.h"
#include "system_configuration.h"
#include "CAN.h"

#if ACTIVATE_BLINKER

void init_LED( void)
{
  LED_PORT_CLOCK_ENABLE();

  GPIO_InitTypeDef  gpioinitstruct={0};

  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin    = LED_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init( LED_PORT, &gpioinitstruct);

  /* Reset PIN to switch off the LED */
  HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_RESET);
}


extern bool blink_slowly;

void blink_code( void *)
{
	  init_LED();
	  CAN_packet dummy;
	  while (1)
	  {
		  HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_RESET);
#if CAN_RUN_TESTCODE
		  CAN_RX_queue.receive( dummy);
		  if( dummy.id == 100)
		    {
		      __disable_irq();
		      while( true)
			;
		    }
		  ++dummy.data_l;
		  ++dummy.id;
		  CAN_send(dummy);
#else
		  delay( blink_slowly ? 300 : 100);
#endif
		  HAL_GPIO_WritePin( LED_PORT,LED_PIN, GPIO_PIN_SET);
#if CAN_RUN_TESTCODE
		  CAN_RX_queue.receive( dummy);
		  ++dummy.data_l;
		  ++dummy.id;
		  CAN_send(dummy);
#else
		  delay( blink_slowly ? 300 : 100);
#endif
	  }
}

Task blink( blink_code, "BLINK");

#endif

