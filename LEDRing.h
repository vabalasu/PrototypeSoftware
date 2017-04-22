#ifndef LEDRING_H
#define LEDRING_H

//NeoPixel Library for LED-ring addon
#include <Adafruit_NeoPixel.h>

class LedRing
{
  private:
    Adafruit_NeoPixel* ring;
    const int NumberOfLeds = 12;
    const int Brightness = 20;

    float lowerInput = 0.0;
    float upperInput = 1023.0;

    float lowerTarget = 426.0;
    float upperTarget = 682.0;

    int lastValue = 0;

  public:
    setup(int pin);
    void setInputRange(float lower, float upper);
    void setTargetRange(float lower, float upper);

    void updateRing(float value);

  private:
    int valueToIndex(float value);
};
#endif

