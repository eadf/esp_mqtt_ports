#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__


/*
    I2C driver for the ESP8266 
    Copyright (C) 2014 Rudy Hardeman (zarya) 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"

typedef struct {
  uint8_t scl_pin;
  uint32_t scl_name;
  uint8_t sda_pin;
  uint32_t sda_name;
} I2C_Self;

/**
 * Reads a given register
 */
bool i2c_readRegister(I2C_Self *self, uint8_t deviceAddr, uint8_t regAddr, uint8_t *regValue);

/**
 * Writes a given register
 */
bool i2c_writeRegister(I2C_Self *self, uint8_t deviceAddr, uint8_t regAddr, uint8_t regValue);

/**
 * initiates the GPIO pins and fills the I2C_Self struct
 */
bool i2c_init(I2C_Self* self, uint8_t scl_pin, uint8_t sda_pin);

#endif
