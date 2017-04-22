#include <Arduino.h>
#include <Wire.h>
#include "SDP3x.h"

void SDP3x::setup(int i2cAddress)
{
  I2cAddress = i2cAddress;
}

float SDP3x::convertPressure(uint16_t ticks)
{
  // Pressure scale factor for SDP31 is 60 ticks/Pascal
  const float dp_scale = 60.0;
  return ticks / dp_scale;
}

float SDP3x::convertTemperature(uint16_t ticks)
{
  // Temperature scale factor for SDP31 is 200 ticks/C
  const float t_scale = 200.0;
  return ticks / t_scale;
}

SDP3xResponse SDP3x::readSensor(float &temperature, float &pressure)
{
  const float ErrorValue = -1.0;
  
  // triggered mode with clock stretching + temperature compensation for DP (0x372D)
  const uint8_t txData[2] = {0x37, 0x2D};

  // Send command
  Wire.beginTransmission(I2cAddress);
  Wire.write(txData, sizeof(txData));
  Wire.endTransmission();

  // Read first 6 bytes of response: 2 bytes DP, 1 CRC, 2 bytes T, 1 CRC
  const int expectedResponseSize = 6;
  uint8_t readData[expectedResponseSize];
  Wire.requestFrom(I2cAddress, expectedResponseSize);
  int rxByteCount = 0;
  
  while (Wire.available() && rxByteCount < expectedResponseSize)
  {
    readData[rxByteCount] = Wire.read();
    rxByteCount++;
  }

  if (rxByteCount != expectedResponseSize)
  {
    temperature = ErrorValue;
    pressure = ErrorValue;
    return rxByteCount == 0 ? SDP_ERROR_NO_RESPONSE : SDP_ERROR_WRONG_RESPONSE_LENGTH;
  }
  
  uint16_t dp_ticks = convertBytes(&readData[0]);
  uint8_t dp_crc = readData[2];

  bool badCrc = false;
  if (checkCrc(&readData[0], 2, dp_crc))
    pressure = convertPressure(dp_ticks);
  else
  {
    pressure = ErrorValue;
    badCrc = true;
  } 

  uint16_t temperature_ticks = convertBytes(&readData[3]);
  uint8_t temperature_crc = readData[5];

  if (checkCrc(&readData[3], 2, temperature_crc))
    temperature = convertTemperature(temperature_ticks);
  else
  {
    temperature = ErrorValue;
    badCrc = true;
  }

  if (badCrc)
    return SDP_ERROR_BAD_CRC;

  return SDP_SUCCESS;
}

bool SDP3x::checkCrc(const uint8_t* data, const uint8_t size, const uint8_t checksum)
{
  uint8_t crc = 0xFF;
  
  // calculates 8-Bit checksum with given polynomial 0x31 (x^8 + x^5 + x^4 + 1)
  for (int i = 0; i < size; i++)
  {
    crc ^= (data[i]);
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80)
        crc = (crc << 1) ^ 0x31;
      else
        crc = (crc << 1);
    }
  }

  // verify checksum
  return (crc == checksum);
}

uint16_t SDP3x::convertBytes(uint8_t* data)
{
  return ((uint16_t)(data)[0]) << 8 | ((data)[1]);
}


