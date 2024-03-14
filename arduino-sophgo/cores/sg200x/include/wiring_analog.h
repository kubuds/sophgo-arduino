/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#include "wiring_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void analogReference(uint8_t mode) ;

extern void analogWrite(uint8_t pin, uint32_t val) ;

extern uint32_t analogRead(uint32_t ulPin) ;

extern void analogReadResolution(int res);

extern void analogWriteResolution(int res);

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_ */
