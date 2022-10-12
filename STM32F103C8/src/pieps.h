/*
 * pieps.h
 *
 *  Created on: Apr 7, 2022
 *      Author: schaefer
 */

#ifndef PIEPS_H_
#define PIEPS_H_

void init_pieps(void); 		//!< initialize sound hardware
void sound_on( bool yes); 	//!< switch sound on | off
void set_frequency( uint16_t frequency_Hz); 	//!< set sound frequency / Hz
void set_volume( uint16_t volume);		//!< set sound volume 8 log steps, max=65536

#endif /* PIEPS_H_ */
