/**
 * @file    CAN_test.cpp
 * @brief   BluePill CAN test project
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */

#include "system_configuration.h"
#include "CAN.h"

void init_LED( void);

#if RUN_CAN_TRANSMITTER

void CAN_TX_task_code (void*)
{
  CAN_init();
  init_LED();
  uint16_t my_id = unique_id_hash ^ (unique_id_hash>>8);
  my_id &= 0x3ff;

  CAN_packet p( my_id, 8, 0x1234567890abcdef, 0);

  while (true)
    {
      // send a bunch of three CAN packets
      if( ! CAN_send( p) )
	Error_Handler ();
      ++ p.data_l;
#if 0
      if( ! CAN_send( p) )
	Error_Handler ();
      ++ p.data_l;
      (void) CAN_send( p); // may sporadically return false
      ++ p.data_l;
#endif

#if 0
      HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_SET);
      delay(50);
      HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_RESET);

      delay (950);
#endif;
      delay (1000);
    }
}

Task CAN_TX_task (CAN_TX_task_code);

#endif
#if RUN_CAN_RECEIVER

void CAN_RX_task_code (void*)
{
  init_LED();

  CAN_init();

  CAN_packet dummy;

  while (1)
    {
	  CAN_RX_queue.receive( dummy);
	  HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_SET);
	  delay(50);
	  HAL_GPIO_WritePin( LED_PORT, LED_PIN, GPIO_PIN_RESET);
	  delay(200);
    }
}

Task CAN_RX_task (CAN_RX_task_code);

#endif


