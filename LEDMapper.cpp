#include "LEDMapper.h"

int LedMapper::valueToIndex(float value)
{
  // First, clip value to allowable input range
  value = max(value, lowerInput);
  value = min(value, upperInput);

  return (int)((value - lowerInput) / (upperInput - lowerInput) * numberOfLeds + 0.5);
}

void LedMapper::update(float value)
{
  const uint32_t color =  ring->Color(128, 128, 128);
  const uint32_t off =    ring->Color(  0,   0,   0);

  int numberToLight = valueToIndex(value);
  for (int i = 0; i < numberToLight; i++)
    ring->setPixelColor(i, color);
  for (int i = numberToLight; i < numberOfLeds; i++)
    ring->setPixelColor(i, off);
}

void PotLedMapper::update(float value)
{
  int numberToLight = valueToIndex(value);
  ring->setBrightness(brightness);

  // Set the LED's
  const uint32_t yellow = ring->Color(128, 100,   0);
  const uint32_t green =  ring->Color(  0, 255,   0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  int lowerTargetIndex = valueToIndex(lowerTarget);
  int upperTargetIndex = valueToIndex(upperTarget);

  for (int i = 0; i < numberToLight; i++)
  {
    uint32_t color = green;
    if (i < lowerTargetIndex || i > upperTargetIndex)
      color = yellow;

    ring->setPixelColor(i, color);
  }

  for (int i = numberToLight; i < numberOfLeds; i++)
  {
    ring->setPixelColor(i, off);
  }

  ring->show();
}

void BatteryLedMapper::update(float value)
{
  int numberToLight = valueToIndex(value);
  ring->setBrightness(brightness);

  // Set the LED's
  const uint32_t blue =   ring->Color(  0, 0,   255);
  const uint32_t red =    ring->Color(255, 0,     0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  int lowerTargetIndex = valueToIndex(lowerTarget);
  int upperTargetIndex = valueToIndex(upperTarget);

  // I can't tell if there is flicker or not. Just in case, only touch lights
  // whose state needs to be changed.
  uint32_t color = numberToLight >= lowerTargetIndex ? blue : red;
  for (int i = 0; i < numberToLight; i++)
    ring->setPixelColor(i, color);

  for (int i = numberToLight; i < numberOfLeds; i++)
    ring->setPixelColor(i, off);

  ring->show();
}

