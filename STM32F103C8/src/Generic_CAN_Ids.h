/*
 *****************************************************************************
 *  @file   Generic_CAN_Ids.h
 *  @author Horst Rupp
 *  @brief  Created on: 21.11.2020
 *  @brief  Last Change : 28.11.2020
 *  @brief  These are the valid IDs for any data transfer on the CAN Bus.
 *****************************************************************************
 */

#ifndef  __Generic_CAN_Ids_h
  #define  __Generic_CAN_Ids_h

  //
  //  **** Sensor-Bit-Masks ***************************************************
  //
  enum SENSOR_IDs
  {
      GNSS_AVAILABLE          = 1,
      D_GNSS_AVAILABLE        = 2,

      MTI_SENSOR_AVAILABLE    = 0x10,
      FXOS_SENSOR_AVAILABLE   = 0x20,
      L3GD20_SENSOR_AVAILABLE = 0x40,
      MS5611_STATIC_AVAILABLE = 0x80,

      MS5611_PITOT_AVAILABLE  = 0x100,
      PITOT_SENSOR_AVAILABLE  = 0x200,
      AIR_SENSOR_AVAILABLE    = 0x400,

      USB_OUTPUT_ACTIVE       = 0x1000,
      BLUEZ_OUTPUT_ACTIVE     = 0x2000,
      CAN_OUTPUT_ACTIVE       = 0x4000,
      USART_2_OUTPUT_ACTIVE   = 0x8000
  };

  //
  //  **** Signal IDs **********************************************************
  //
  enum CAN_SIGNAL_IDs
  {
    cNoSignal,
    cAutoChange,
    cInvAutoChange,
    cAlarm,
    cTransfer,
    cClick,
    cBeep,
    cmaxSigId,
  };

  //
  //  ******** Package IDs *****************************************************
  //
  enum CAN_PACKAGE_IDs
  {
    //
    //  CAN packages with source FSB (Klaus-Sensor-Box)
    //
    c_CID_KSB_HeartBeat        = 0x100,    //!< uint32_t  version as 0x0102002a "1.02 Build 42"
    c_CID_KSB_EulerAngles      = 0x101,    //!< int16_t roll nick yaw / 1/1000 rad
    c_CID_KSB_Airspeed         = 0x102,    //!< uint16_t TAS, IAS / km/h
    c_CID_KSB_Vario            = 0x103,    //!< int16_t , -integrator / mm/s
    c_CID_KSB_GPS_Date_Time    = 0x104,    //!< uint8_t year-2000, month, day, hour, mins, secs
    c_CID_KSB_GPS_LatLon       = 0x105,    //!< int32_t lat, lon / 10^-7 degrees
    c_CID_KSB_GPS_Alt          = 0x106,    //!< int64_t MSL altitude / mm
    c_CID_KSB_GPS_Trk_Spd      = 0x107,    //!< int16_t ground vector / 1/1000 rad, uint16_t groundspeed / km/h
    c_CID_KSB_Wind             = 0x108,    //!< int16_t 1/1000 rad, uint16_t km/h    Current Wind
                                           //!> int16_t 1/1000 rad, uint16_t km/h    Average Wind
    c_CID_KSB_Atmosphere       = 0x109,    //!< uint16_t pressure / Pa uint16_t density / g/m^3
    c_CID_KSB_GPS_Sats         = 0x10a,    //!< uint8_t No of Sats
                                           //!> uint8_t Fix-Type NO=0 2D=1 3D=2 RTK=3

    c_CID_KSB_Acceleration     = 0x10b,    //!< int16_t representing TOTAL G-force in mm/s^2
                                           //!< acceleration counted positive downward relative to plane
                                           //!< stored as float m/s^2 counted positive upward

                                           //!< int16_t representing NETTO G-force in mm/s^2
                                           //!< acceleration counted positive downward relative to plane
                                           //!< stored as float m/s^2 counted positive upward

                                           //!> int16_t representing GPS vertical speed as seen from earth
                                           //!> in mm/s, stored as float m/s

                                           //!> uint8_t, representing enum ( c_Gliding, c_Transition, c_Climbing )

    c_CID_KSB_TurnCoord        = 0x10c,    //!< float  in mm/s^2 , float in mm/s^2
    c_CID_KSB_SystemState      = 0x10d,    //!< uint32_t Bitmuster

    c_CID_KSB_Noise            = 0x10e,    //!< ??   TODO
    c_CID_KSB_Temperature      = 0x10f,    //!< int32_t  as float temp * 1000
    c_CID_KSB_Humidity         = 0x110,    //!< unit32_t as float hum * 1000
    c_CID_KSB_Pressure         = 0x111,    //!< unit32_t as float press * 1000
    c_CID_KSB_Vdd              = 0x112,    //!< unit16_t as float voltage * 10
    c_CID_KSB_TCs              = 0x113,    //!< int16_t as float sec * 10 tau for fast wind in cruise +
                                           //!< int16_t as float sec * 10 tau for slow wind in cruise +
                                           //!< int16_t as float sec * 10 tau for fast wind in climb +
                                           //!< int16_t as float sec * 10 tau for slow wind in climb
    c_CID_KSB_Sw_Hysteresis    = 0x114,    //!< int16_t as float sec * 10
    c_CID_KSB_Euler_SetUp      = 0x115,    //!< int16_t as float dec deg * 10 +  // Roll
                                           //!< int16_t as float dec deg * 10 +  // Nick
                                           //!< int16_t as float dec deg * 10    // Yaw
    c_CID_KSB_DecInclination   = 0x116,    //!< int16_t as float dec deg * 10 +  // Declination
                                           //!< int16_t as float dec deg * 10    // Inclination
    c_CID_KSB_IAS_Offset       = 0x117,    //!< int16_t as float km/h * 10

    //
    //  CAN packages with source AUD (Horst-Audio)
    //
    c_CID_AUD_HeartBeat        = 0x200,    // uint32_t  version as 0x0102002a "1.02 Build 42"
    c_CID_AUD_CMD_2_XCSOAR     = 0x201,    //!< uint8_t command for XCSoar
                                           //!> = 0 Unforce XCSoar CLIMB-CRUISE
                                           //!> = 1 Force XCSoar to CLIMB
                                           //!> = 2 Force XCSoar to CRUISE
                                           //!> = 3 Unforce XCSoar WINDUP
                                           //!> = 4 Force XCSoar to WINDUP
    c_CID_AUD_Noise            = 0x202,    //!< ??   TODO
    c_CID_AUD_Temperature      = 0x203,    //!< int32_t  as float temp * 1000
    c_CID_AUD_Humidity         = 0x204,    //!< unit32_t as float hum * 1000
    c_CID_AUD_Pressure         = 0x205,    //!< unit32_t as float press * 1000
    c_CID_AUD_Flaps_Data       = 0x206,    //!< uint16_t position [percent * 100]
                                           //!> + uint8_t switch pattern [0b0000-0b1111]

#ifdef  RUN_EMULATOR
    c_CID_AUD_TCs              = 0x207,    //!< int16_t as float sec * 10 tau for fast wind in cruise +
                                           //!< int16_t as float sec * 10 tau for slow wind in cruise +
                                           //!< int16_t as float sec * 10 tau for fast wind in climb +
                                           //!< int16_t as float sec * 10 tau for slow wind in climb
    c_CID_AUD_Sw_Hysteresis    = 0x208,    //!< int16_t as float sec * 10
    c_CID_AUD_Euler_SetUp      = 0x209,    //!< int16_t as float dec deg * 10 +  // Roll
                                           //!< int16_t as float dec deg * 10 +  // Nick
                                           //!< int16_t as float dec deg * 10    // Yaw
    c_CID_AUD_DecInclination   = 0x20a,    //!< int16_t as float dec deg * 10 +  // Declination
                                           //!< int16_t as float dec deg * 10    // Inclination
    c_CID_AUD_IAS_Offset       = 0x209,    //!< int16_t as float km/h * 10
#endif

    //
    //  CAN packages with source AD57
    //
    c_CID_A57_HeartBeat        = 0x300,    // uint32_t  version as 0x0102002a "1.02 Build 42"
    c_CID_A57_CMD_2_XCSOAR     = 0x301,    //!< uint8_t command for XCSoar
                                           //!> = 0 Unforce XCSoar CLIMB-CRUISE
                                           //!> = 1 Force XCSoar to CLIMB
                                           //!> = 2 Force XCSoar to CRUISE
                                           //!> = 3 Unforce XCSoar WINDUP
                                           //!> = 4 Force XCSoar to WINDUP
    c_CID_A57_Noise            = 0x302,    //!< ??   TODO
    c_CID_A57_Temperature      = 0x303,    //!< int32_t  as float temp * 1000
    c_CID_A57_Humidity         = 0x304,    //!< unit32_t as float hum * 1000
    c_CID_A57_Pressure         = 0x305,    //!< unit32_t as float press * 1000
    c_CID_A57_Vdd              = 0x306,    //!< unit16_t as float voltage * 10
    c_CID_A57_TCs              = 0x307,    //!< int16_t as float sec * 10 tau for fast wind in cruise +
                                           //!< int16_t as float sec * 10 tau for slow wind in cruise +
                                           //!< int16_t as float sec * 10 tau for fast wind in climb +
                                           //!< int16_t as float sec * 10 tau for slow wind in climb
    c_CID_A57_Sw_Hysteresis    = 0x308,    //!< int16_t as float sec * 10
    c_CID_A57_Euler_SetUp      = 0x309,    //!< int16_t as float dec deg * 10 +  // Roll
                                           //!< int16_t as float dec deg * 10 +  // Nick
                                           //!< int16_t as float dec deg * 10    // Yaw
    c_CID_A57_DecInclination   = 0x30a,    //!< int16_t as float dec deg * 10 +  // Declination
                                           //!< int16_t as float dec deg * 10    // Inclination
    c_CID_A57_IAS_Offset       = 0x30b,    //!< int16_t as float km/h * 10


    c_CID_A57_Signal           = 0x310,    //!< uint8_t signal_id +
                                           //!< uint8_t signal_volume

    c_CID_A57_Audio            = 0x311,    //!< int16_t  audio_frequency +
                                           //!< uint16_t interval +
                                           //!< uint8_t  audio-volume +
                                           //!< uint8_t  duty cycle
                                           //!< uint8_t  climb-mode

    c_CID_A57_Flaps_Status     = 0x312,    //!< uint8_t  0/1 on/off-switch
                                           //!< uint8_t  CurrentFlapsSetting
                                           //!< uint8_t  OptimalFlapsSetting
                                           //!< uint8_t  FlapsFlashControl
                                           //!< uint8_t  LEDDutyCycle in %

    c_CID_A57_Reboot           = 0x313,    //!< empty package, just a trigger

  };

#endif  // __Generic_CAN_Ids_h
// ****************************************************************************
//  EOF
// ****************************************************************************

