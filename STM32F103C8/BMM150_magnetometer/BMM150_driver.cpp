#include "system_configuration.h"
#include "FreeRTOS_wrapper.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "sensing.h"
#include "i2c.h"
#include "bmm150.h"
#include "CAN.h"

void MX_I2C1_Init(void);

BMM150 bmm150;

unsigned mag_x, mag_y, mag_z;

static void BMM150_runnable (void *argument)
{
  CAN_packet p (0x120, 8);

  while (true) // try initialization again and again
    {
      delay( 100);

      CAN_init ();

      MX_I2C1_Init();
      I2C_Init (&hi2c1);

      bmm150.initialize();

      for( Synchronous_Timer t( 50); true; t.sync()) // 20 Hz
	{
	  bmm150.read_mag_data();
	  mag_x = bmm150.mag_data.x;
	  mag_y = bmm150.mag_data.y;
	  mag_z = bmm150.mag_data.z;
	}
    }
}

Task BMM150_test ( BMM150_runnable, "BME680", 256);
