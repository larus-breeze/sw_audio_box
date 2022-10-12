/***********************************************************************//**
 * @file    	CAN.h
 * @brief   	Low-level CAN bus driver
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

#ifndef CAN_H_
#define CAN_H_

//! basic CAN packet type
class CAN_packet
{
public:
	CAN_packet( uint16_t _id=0, uint8_t _dlc=0, uint64_t data=0, uint8_t _is_remote=0)
	: id(_id),
	  dlc(_dlc),
	  is_remote(_is_remote),
	  data_l( data)
	{}
// attributes
	uint16_t id; 		//!< identifier
	uint8_t dlc; 		//!< data length code
	uint8_t is_remote; 	//!< true for remote request
	union
	{
	    uint8_t  data_b[8];   //!< data seen as 8 times uint8_t
	    int8_t   data_sb[8];  //!< data seen as 8 times int8_t
	    uint16_t data_h[4];   //!< data seen as 4 times uint16_t
	    int16_t  data_sh[4];  //!< data seen as 4 times int16_t
	    uint32_t data_w[2];   //!< data seen as 2 times uint32_t
	    int32_t  data_sw[2];  //!< data seen as 2 times int32_t
	    float    data_f[2];   //!< data seen as 2 times 32-bit floats
	    uint64_t data_l;      //!< data seen as 64-bit integer
	};
	inline bool operator < (const CAN_packet & right) const
	{
		return id < right.id; // id smaller means priority higher
	 }
	inline bool operator > (const CAN_packet & right) const
	{
		return id > right.id; // id smaller means priority higher
	 }

};

//! CAN module initialization
void CAN_init(void);

//! CAN receive mechanism
extern Queue < CAN_packet > CAN_RX_queue;

//! CAN send mechanism
bool CAN_send( const CAN_packet &p);

#endif /* CAN_H_ */
