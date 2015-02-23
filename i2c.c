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
#include "i2c/i2c.h"
#include "easygpio/easygpio.h"

#define I2C_SLEEP_TIME 5 // 5==100KHz
#define i2c_read() GPIO_INPUT_GET(GPIO_ID_PIN(self->sda_pin));
#define i2c_disableOutput() GPIO_DIS_OUTPUT(GPIO_ID_PIN(self->sda_pin))


static void i2c_start(I2C_Self* self);
static void i2c_stop(I2C_Self* self);
static bool i2c_readByteCheckAck(I2C_Self* self, uint8_t *data);
static bool i2c_writeByteCheckAck(I2C_Self* self, uint8_t data);

static bool i2c_beginTransmission(I2C_Self *self, uint8_t deviceAddr, bool read);

static void ICACHE_FLASH_ATTR
i2c_delay_us(uint32_t delay) {
  uint32_t now = system_get_time();
  while(system_get_time()-now < delay) {
    ;
  }
}

/**
 * Set SDA to state.
 * This should *really* be done with the pull-up/downs but it does not work :/
 */
static void ICACHE_FLASH_ATTR
i2c_sda(I2C_Self* self, uint8_t state) {
  //Set SDA line to state
  GPIO_OUTPUT_SET(self->sda_pin, (state&0x1));  // parenthesis are required
}

/**
 * Set SCK to state
 * This should *really* be done with the pull-up/downs but it does not work :/
 */
static void ICACHE_FLASH_ATTR
i2c_sck(I2C_Self* self, uint8_t state) {
  //Set SCK line to state
  GPIO_OUTPUT_SET(self->scl_pin, (state&0x1)); // parenthesis are required
}

/**
 * I2C Start signal.
 * Data Transfer is initiated with a START bit (S) signaled by SDA being pulled low while SCL stays high.
 */
static void ICACHE_FLASH_ATTR
i2c_start(I2C_Self* self) {
  i2c_sda(self, 1);
  i2c_sck(self, 1);
  os_delay_us(I2C_SLEEP_TIME);
  i2c_sda(self, 0);
  os_delay_us(I2C_SLEEP_TIME);
  i2c_sck(self, 0);
  os_delay_us(I2C_SLEEP_TIME);
}

/**
 * I2C Stop signal.
 * A STOP bit (P) is signaled when SDA is pulled high while SCL is high.
 */
static void ICACHE_FLASH_ATTR
i2c_stop(I2C_Self* self) {
  i2c_sda(self,0); // According to http://en.wikipedia.org/wiki/I%C2%B2C this must be set low
  os_delay_us(I2C_SLEEP_TIME);
  i2c_sck(self,1);
  os_delay_us(I2C_SLEEP_TIME);
  i2c_sda(self,1);
  os_delay_us(I2C_SLEEP_TIME);
}

/**
 * deviceAddr should be unshifted
 * read : readmode=true, write=false
 */
static bool ICACHE_FLASH_ATTR
i2c_beginTransmission(I2C_Self *self, uint8_t deviceAddr, bool read) {
  i2c_start(self);
  return i2c_writeByteCheckAck(self, (deviceAddr<<1)|read);
}

/**
 * Reads a given register
 */
bool ICACHE_FLASH_ATTR
i2c_readRegister(I2C_Self *self, uint8_t deviceAddr, uint8_t regAddr, uint8_t *regValue) {
  bool rv = true;

  rv &= i2c_beginTransmission(self, deviceAddr, false);
  rv &= i2c_writeByteCheckAck(self, regAddr); // read request
  i2c_stop(self);

  rv &= i2c_beginTransmission(self, deviceAddr, true);
  rv &= i2c_readByteCheckAck(self, regValue);
  i2c_stop(self);
  return rv;
}

/**
 * Writes a given register
 */
bool ICACHE_FLASH_ATTR
i2c_writeRegister(I2C_Self *self, uint8_t deviceAddr, uint8_t regAddr, uint8_t regValue) {
  bool rv = true;

  // Write the register
  rv &= i2c_beginTransmission(self, deviceAddr, false);
  rv &= i2c_writeByteCheckAck(self, regAddr);
  rv &= i2c_writeByteCheckAck(self, regValue);
  i2c_stop(self);
  return rv;
}

/**
 * Receive byte from the I2C bus
 * returns the byte
 * returns the value of ACK
 */
static bool ICACHE_FLASH_ATTR
i2c_readByteCheckAck(I2C_Self* self, uint8_t *data) {
  *data = 0;
  uint8_t data_bit;
  uint8_t i;

  for (i = 0; i < 8; i++) {
    os_delay_us(I2C_SLEEP_TIME);
    if (i==0) i2c_disableOutput();
    i2c_sck(self,0);
    os_delay_us(I2C_SLEEP_TIME);

    i2c_sck(self,1);
    os_delay_us(I2C_SLEEP_TIME);

    data_bit = !i2c_read();
    os_delay_us(I2C_SLEEP_TIME);

    data_bit <<= (7 - i);
    *data |= data_bit;
  }
  // allow slave to ACK or NACK
  i2c_disableOutput();
  os_delay_us(I2C_SLEEP_TIME);

  i2c_sck(self,1);
  os_delay_us(I2C_SLEEP_TIME);
  bool rv = i2c_read();

  i2c_sck(self,0);
  os_delay_us(I2C_SLEEP_TIME);
  return rv;
}

/**
 * scl should be low when calling this method.
 * Write byte to I2C bus
 * uint8_t data: to byte to be written
 * returns the value of ACK
 */
static bool ICACHE_FLASH_ATTR
i2c_writeByteCheckAck(I2C_Self* self, uint8_t data) {
  uint8_t sdap = self->sda_pin;
  uint8_t sclp = self->scl_pin;

  int8_t i;
  bool rv = 0;
  for (i = 7; i >= 0; i--) {
    GPIO_OUTPUT_SET(sdap, (data >> i)&0x1);
    i2c_delay_us(I2C_SLEEP_TIME);
    GPIO_OUTPUT_SET(sclp, 1);
    i2c_delay_us(I2C_SLEEP_TIME);
    GPIO_OUTPUT_SET(sclp, 0);
  }
  // allow slave to ACK or NACK
  GPIO_DIS_OUTPUT(GPIO_ID_PIN(sdap));
  i2c_delay_us(I2C_SLEEP_TIME);

  GPIO_OUTPUT_SET(sclp, 1);
  i2c_delay_us(I2C_SLEEP_TIME);
  rv = !i2c_read();

  GPIO_OUTPUT_SET(sclp, 0);
  i2c_delay_us(I2C_SLEEP_TIME);
  return rv;
}

/**
 * I2C init function
 * This sets up the GPIO pins
 * Returns false if something fails
 */
bool ICACHE_FLASH_ATTR
i2c_init(I2C_Self* self, uint8_t scl_pin, uint8_t sda_pin) {
  uint8_t gpio_func;

  if (self == NULL){
    os_printf("i2c_init: Error: self can't be null\n");
    return false;
  }

  self->sda_pin = sda_pin;
  self->scl_pin = scl_pin;

  if (self->sda_pin == self->scl_pin) {
    os_printf("i2c_init: Error: You must specify two unique pins for this to work\n");
    return false;
  }

  if (!(easygpio_getGPIONameFunc(self->sda_pin, &(self->sda_name), &gpio_func) &&
        easygpio_getGPIONameFunc(self->scl_pin, &(self->scl_name), &gpio_func) )) {
    return false;
  }
  //Disable interrupts
  ETS_GPIO_INTR_DISABLE();

  if (!(easygpio_pinMode(self->sda_pin, EASYGPIO_PULLUP, EASYGPIO_INPUT) &&
        easygpio_pinMode(self->scl_pin, EASYGPIO_PULLUP, EASYGPIO_INPUT) )) {
    return false;
  }

  //Turn interrupt back on
  ETS_GPIO_INTR_ENABLE();

  i2c_sda(self, 1);
  i2c_sck(self, 1);
  return true;
}

