#include "LedRing.h"

LedRing::setup(int pin)
{
  ring = new Adafruit_NeoPixel(60, pin, NEO_GRB + NEO_KHZ800);

  ring->begin();
  ring->setBrightness(Brightness);

  lastValue = 0;
}

void LedRing::setInputRange(float lower, float upper)
{
  lowerInput = lower;
  upperInput = upper;
}

void LedRing::setTargetRange(float lower, float upper)
{
  lowerTarget = lower;
  upperTarget = upper;
}

void LedRing::updateRing(float value)
{
  // First, clip value to allowable input range
  value = max(value, lowerInput);
  value = min(value, upperInput);

  int numberToLight = valueToIndex(value);

  // Set the LED's
  const uint32_t yellow = ring->Color(128, 100,   0);
  const uint32_t green =  ring->Color(  0, 255,   0);
  const uint32_t off =    ring->Color(  0,   0,   0);

  int lowerTargetIndex = valueToIndex(lowerTarget);
  int upperTargetIndex = valueToIndex(upperTarget);

  // I can't tell if there is flicker or not. Just in case, only touch lights
  // whose state needs to be changed.
  for (int i = lastValue; i < numberToLight; i++)
  {
    uint32_t color = green;
    if (i < lowerTargetIndex || i > upperTargetIndex)
      color = yellow;

    ring->setPixelColor(i, color);
  }

  for (int i = numberToLight; i < NumberOfLeds; i++)
  {
    ring->setPixelColor(i, off);
  }

  ring->show();
}

int LedRing::valueToIndex(float value)
{
  return (int)((value - lowerInput) / (upperInput - lowerInput) * NumberOfLeds + 0.5);
}

