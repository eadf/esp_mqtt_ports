/*
* bitutils.c
*
* Copyright (c) 2015, eadf (https://github.com/eadf)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "bitutils/bitutils.h"
#include "osapi.h"

/**
 * sets the 'bitNumber' bit to 'bitValue'
 */
void ICACHE_FLASH_ATTR
bitutils_setBit(uint8_t *data, uint16_t bitNumber, bool bitValue) {
  if (bitValue) {
    data[bitNumber/8] |= 1<<(bitNumber%8);
  } else {
    data[bitNumber/8] &= ~(1<<(bitNumber%8));
  }
}

/**
 * returns the 'bitNumber' bit
 */
bool ICACHE_FLASH_ATTR
bitutils_getBit(uint8_t *data, uint16_t bitNumber) {
  return (data[bitNumber/8] >> (bitNumber%8)) & 0x1;
}

/**
 * print the bits with os_printf(). Range is inclusive.
 */
void ICACHE_FLASH_ATTR
bitutils_printBool(uint8_t *data, uint16_t from, uint16_t to) {
  uint16_t i = 0;
  if (from < to) {
    for (i=from; i<=to; i++) {
      os_printf(bitutils_getBit(data,i)?"1":"0");
      if(i>0 && i%8==0) {
        os_printf(" ");
      }
    }
  } else {
    for (i=from; i>=to; i--) {
      os_printf(bitutils_getBit(data,i)?"1":"0");
      if(i>0 && i%8==0) {
        os_printf(" ");
      }
    }
  }
}
