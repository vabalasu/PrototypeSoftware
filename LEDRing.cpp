#include "LedRing.h"

LedRing::setup()
{
  ring = new Adafruit_NeoPixel(60, RingPin, NEO_GRB + NEO_KHZ800);

  ring->begin();
  ring->setBrightness(Brightness);
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
  if (value < lowerInput)
    value = lowerInput;
  else if (value > upperInput)
    value = upperInput;

  int numberToLight = valueToIndex(value);

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


LedRing ledRing;
