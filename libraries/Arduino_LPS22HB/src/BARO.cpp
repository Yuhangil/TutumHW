/*
  This file is part of the Arduino_LPS22HB library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>

#include "BARO.h"

#define LPS22HB_ADDRESS  0x5C

#define LPS22HB_WHO_AM_I_REG        0x0f
#define LPS22HB_CTRL2_REG           0x11
#define LPS22HB_STATUS_REG          0x27
#define LPS22HB_PRESS_OUT_XL_REG    0x28
#define LPS22HB_PRESS_OUT_L_REG     0x29
#define LPS22HB_PRESS_OUT_H_REG     0x2a

LPS22HBClass::LPS22HBClass(TwoWire& wire) :
  _wire(&wire)
{
}

int LPS22HBClass::begin(int ODR_HZ, int FIFO_MODE)
{
  _wire->begin();
  _wire->setClock(400000UL);
  if (i2cRead(LPS22HB_WHO_AM_I_REG) != 0xb1) {
    end();
    return 0;
  }
  SetODR(ODR_HZ);
  SetFIFO(FIFO_MODE);

  return 1;
}

void LPS22HBClass::end()
{
  _wire->end();
}

void LPS22HBClass::SetODR(int ODR_HZ)
{
    int temp = 0;
    if (ODR_HZ < 1)
        temp = 0;
    if (ODR_HZ < 10)
        temp = 1;
    if (ODR_HZ < 25)
        temp = 2;
    if (ODR_HZ < 50)
        temp = 3;
    if (ODR_HZ < 75)
        temp = 4;
    if (ODR_HZ >= 75)
        temp = 5;
    temp = temp << 4;
    BARO.i2cWrite(0x10, temp); //CTRL1 -> ODR Hz
}

void LPS22HBClass::SetFIFO(int FIFO_MODE)
{
    BARO.i2cWrite(0x14, B00000000); //Write FIFO_CTRL _ F_MODE Bypass to Clear FIFO
    BARO.i2cWrite(0x14, FIFO_MODE << 5); //Write FIFO_CTRL _ F_MODE Mode Select
    BARO.i2cWrite(0x11, B01010000); //CTRL2 -> FIFO Enable
    // 0 : By pass , 1 : FIFO mode, 2 : Stream mode, 3 : Stream to FIFO, 4 : Bypass to Stream, 5: Reserved, 6 : Dynamic-stream mode, 7 : Bypass to FIFO mode
}

float LPS22HBClass::readPressure(int units)
{
    int XL = i2cRead(LPS22HB_PRESS_OUT_XL_REG);
    int L = i2cRead(LPS22HB_PRESS_OUT_L_REG);
    int H = i2cRead(LPS22HB_PRESS_OUT_H_REG);
  float reading = ((XL | (L << 8) | (H << 16)) / 4096.0);
    if (XL == -1)
    {
        Serial.print("XL MINUS : ");
        Serial.println(XL);
    }
    if (L == -1)
    {
        Serial.print("L MINUS : ");
        Serial.println(L);
    }
    if (H == -1)
    {
        Serial.print("H MINUS : ");
        Serial.println(H);
    }

  if (units == MILLIBAR) { // 1 hPa = 1 MILLIBAR
    return reading;
  } else if (units == PSI) {  // 1 hPa = 0.0145038 PSI
    return reading * 0.0145038;
  } else {
    return reading;
  }
}

int LPS22HBClass::i2cRead(uint8_t reg)
{
  _wire->beginTransmission(LPS22HB_ADDRESS);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) {
      Serial.println("false!!!");
    return -1;
  }

  if (_wire->requestFrom(LPS22HB_ADDRESS, 1) != 1) {
    Serial.println("NOT ACK!!!");
    return -1;
  }
  
  return _wire->read();
}

int LPS22HBClass::i2cWrite(uint8_t reg, uint8_t val)
{
  _wire->beginTransmission(LPS22HB_ADDRESS);
  _wire->write(reg);
  _wire->write(val);
  if (_wire->endTransmission() != 0) {
    return 0;
  }

  return 1;
}

#ifdef ARDUINO_ARDUINO_NANO33BLE
LPS22HBClass BARO(Wire1);
#else
LPS22HBClass BARO(Wire);
#endif
