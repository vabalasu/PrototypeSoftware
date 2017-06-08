#include <Arduino.h>
#include "LEDMapper.h"

int LedMapper::valueToIndex(float value)
{
  // First, clip value to allowable input range
  value = constrain(value, lowerInput, upperInput);

  return (int)((value - lowerInput) / (upperInput - lowerInput) * numberOfLeds + 0.5);
}

void LedMapper::setup(Adafruit_NeoPixel *newRing)
{
  ring = newRing;
}

void FlowLedMapper::update(float value)
{
  ring->setBrightness(brightness);

  // Set the LED's
  const uint32_t yellow = ring->Color(128, 100,   0);
  const uint32_t green =  ring->Color(  0, 255,   0);
  const uint32_t red =    ring->Color(255,   0,   0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  uint32_t leftColor = off;
  uint32_t topColor = off;
  uint32_t rightColor = off;

  if (value < 2.3)
    leftColor = red;
  else if (value < 2.7)
    leftColor = yellow;
  else if (value <= 3.3)
    topColor = green;
  else if (value < 3.7)
    rightColor = yellow;
  else
    rightColor = red;

  for (int i = 0; i < numberOfLeds; i++)
    ring->setPixelColor(i, off);

  ring->setPixelColor(4, leftColor);
  ring->setPixelColor(6, topColor);
  ring->setPixelColor(8, rightColor);

  ring->show();
}

void BatteryLedMapper::update(float value)
{
  ring->setBrightness(brightness);

  // Set the LED's
  const uint32_t yellow = ring->Color(255, 200,   0);
  const uint32_t green =  ring->Color(  0, 255,   0);
  const uint32_t red =    ring->Color(255,   0,   0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  uint32_t color;
  if (value < lowerTarget)
    color = red;
  else if (value < upperTarget)
    color = yellow;
  else
    color = green;

  for (int i = 0; i < numberOfLeds; i++)
    ring->setPixelColor(i, color);

  ring->show();
}

void ReadyLedMapper::setStartTime(unsigned long st)
{
  startTime = st;
}

// For this one, value should be in milliseconds
void ReadyLedMapper::update(float value)
{
  float brightLevel = (brightness * (1 - cos((value - startTime) / 12000 * 2 * 3.14159265359)) / 2);
  ring->setBrightness(max(brightLevel + 0.5,10));

  const uint32_t readyColor = ring->Color(254,  50,   0);

  uint32_t color = readyColor;

  for (int i = 0; i < numberOfLeds; i++)
    ring->setPixelColor(i, color);

  ring->show(); 
}

void BreathResultMapper::setTimes(unsigned long st, unsigned long dur)
{
  startTime = st;
  duration = dur;
}

void BreathResultMapper::setSuccess(bool success)
{
  wasSuccessful = success;
}

// For this one, value should be in milliseconds
void BreathResultMapper::update(float value)
{
  const uint32_t green =  ring->Color(  0, 255,   0);
  const uint32_t red =    ring->Color(255,   0,   0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  float progress = min((value - startTime) / (float)duration, 1.0);

  int indexToLight = (int)(progress * numberOfLeds * 2 + 0.5) - 1;
  if (indexToLight > numberOfLeds)
    indexToLight = 2*numberOfLeds - 1 - indexToLight;

  for (int i = 0; i < numberOfLeds; i++)
    ring->setPixelColor(i, off);

  uint32_t color = red;
  if (wasSuccessful)
    color = green;

  ring->setPixelColor(indexToLight, color);
  ring->show();
}

