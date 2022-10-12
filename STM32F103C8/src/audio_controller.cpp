/***********************************************************************//**
 * @file     	task_Audio_Controller.c
 * @brief    	Audio controller for BluePill + audio board
 * @author	Dr. Klaus Schaefer
 * @copyright 	Copyright 2021 Dr. Klaus Schaefer. All rights reserved.
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

#include "main.h"
#include "system_configuration.h"
#include "math.h"
#include "Generic_CAN_Ids.h"
#include "CAN_distributor.h"
#include "pieps.h"

#if RUN_AUDIO_CONTROLLER

#define MINIMUM_FREQUENCY 300
#define FREQUENCY_SHIFT 12

class chirp_controller_t
{
public:
  enum mode_type
  {
    STOPPED, DOWN, UP
  };

  chirp_controller_t (void)
  {
    instant_frequency = 0;
    mode = STOPPED;
  }

  void
  set_mode (mode_type mode, uint16_t speed_error = 0)
  {
    if (this->mode != mode)
      {
	switch (mode)
	  {
	  case UP:
	    instant_frequency = START_F;
	    increment = speed_error;
	    break;
	  case DOWN:
	    instant_frequency = STOP_F;
	    increment = speed_error;
	    break;
	  case STOPPED:
	    increment = 0;
	    instant_frequency = 0;
	    break;
	  }
      }
    this->mode = mode;
  }

  uint16_t step (void)
  {
    switch (mode)
      {
      case UP:
	ASSERT( increment < 0);
	instant_frequency -= increment;
	if (instant_frequency > STOP_F)
	  {
	    instant_frequency = 0;
	    mode = STOPPED;
	  }
	break;
      case DOWN:
	ASSERT( increment > 0);
	instant_frequency -= increment >> 1;
	if (instant_frequency < START_F)
	  {
	    instant_frequency = 0;
	    mode = STOPPED;
	  }
	break;
      case STOPPED:
	instant_frequency = 0;
	break;
      }
    return instant_frequency;
  }
private:
  mode_type mode;
  enum
  {
    START_F = 500, STOP_F = 3000, INCREMENT = 30
  };
  int16_t instant_frequency;
  int16_t increment;
};

void
Start_Audio (uint16_t vol, uint16_t freq);

enum
{
  c_Cruising,         // 0
  c_Transition,       // 1
  c_Climbing,         // 2
};

typedef struct
{
  uint16_t Signal_Id;
  uint16_t Signal_Volume;

} SignalQItem_t;

void Audio_Controller (void *)
{
  uint8_t climbmode = 0;
  uint16_t Interval = 0;
  bool audio_muted = true;
  uint16_t Audio_Volume = 0;
  int Frequency;
  int32_t NormedFrequency = 0.0;

  int8_t speed_error = 0;
  int16_t speed_error_integrator = 0;
  chirp_controller_t chirp_controller;
  unsigned interval_counter = 0;

  Queue<CAN_packet> rx_q (3);

    {
      CAN_distributor_entry cde =
	{ 0xffff, c_CID_A57_Audio, &rx_q };
      bool result = subscribe_CAN_messages (cde);
      ASSERT(result);
      cde.ID_value = c_CID_A57_Signal;
      result = subscribe_CAN_messages (cde);
      ASSERT(result);
    }

  init_pieps ();
  sound_on (false);

  int CAN_RX_active = 0;

  // task main loop ************************************************
  for (Synchronous_Timer t (10); true; t.sync ())
    {
      CAN_packet p;
      while (rx_q.receive (p, NO_WAIT)) // receive CAN packet if available
	{
	  if ((p.id == c_CID_A57_Audio) && (p.dlc == 8))
	    {
	      CAN_RX_active = 100;
	      NormedFrequency = p.data_sh[0] + 10000;
	      if (NormedFrequency < 0)
		NormedFrequency = 0;
	      Interval = p.data_h[1];
	      Audio_Volume = p.data_b[4] * 5; // just use range 0..20
	      if( Audio_Volume > 100)
		Audio_Volume = 100;
	      climbmode = p.data_b[6];
	      speed_error = -(int8_t) (p.data_sb[7]);
	    }
	}

      if (CAN_RX_active) // kind of a watchdog
	--CAN_RX_active;

      if (CAN_RX_active == 0)
	{
	  Audio_Volume = 0;
	  set_volume (Audio_Volume);
	  sound_on (false);
	  delay (100);
	  continue; // continue polling CAN reception
	}

      if (Audio_Volume > 0)
	{
	  if (climbmode != c_Cruising) // ** VARIO ** sound
	    {
	      Frequency = MINIMUM_FREQUENCY + NormedFrequency / FREQUENCY_SHIFT;

	      if (Interval > 10)
		{
		  ++interval_counter;
		  if (interval_counter > 20)
		    interval_counter = 0;
		  if (interval_counter > 10)
		    Frequency = 0;
		}
	    }

	  else   // ** SPEED COMMANDER **  sound
	    {
	      speed_error_integrator += speed_error;
	      if (speed_error_integrator > 1000)
		{
		  chirp_controller.set_mode (chirp_controller_t::DOWN,
					     speed_error);
		  speed_error_integrator = 0;
		}
	      else if (speed_error_integrator < -1000)
		{
		  chirp_controller.set_mode (chirp_controller_t::UP,
					     speed_error);
		  speed_error_integrator = 0;
		}

	      Frequency = chirp_controller.step ();

	      if (speed_error > 0)
		{
		  ++interval_counter;
		  if (interval_counter > 15)
		    interval_counter = 0;
		  if (interval_counter > 5)
		    Frequency = 0;
		}

	    }
	} // volume > 0
      else
	{
	    Frequency = 0;
	    Audio_Volume = 0;
	}

      if ((Audio_Volume == 0) || (Frequency == 0))
	{
//	  set_frequency (0);
	  set_volume (0);
	  sound_on (false);
	}
      else
	{
	  set_frequency (Frequency);
	  set_volume (Audio_Volume * 65536l / 100l);
	  sound_on (true);
	}
    } // task loop
} // task

Task audio (Audio_Controller, "AUDIO", 256+128);

//
// *******************************************************************************
// The End
// *******************************************************************************

#endif
