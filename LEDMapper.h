#ifndef LEDMAPPER_H
#define LEDMAPPER_H

#include <Adafruit_NeoPixel.h>

class LedMapper
{
  protected:
    Adafruit_NeoPixel* ring;
    float lowerInput = 0.0;
    float upperInput = 1023.0;

    float lowerTarget = 1023.0 / 3.0;
    float upperTarget = 1023.0 * 2.0 / 3.0;

    int numberOfLeds = 12;
    int brightness = 20;

  public:
    virtual void setInputRange(float lower, float upper) { lowerInput = lower; upperInput = upper; }
    virtual void setTargetRange(float lower, float upper) { lowerTarget = lower; upperTarget = upper; }
    virtual void setup(Adafruit_NeoPixel *newRing) { ring = newRing; }

    virtual void update(float value);

  protected:
    virtual int valueToIndex(float value);
};

class PotLedMapper : public LedMapper
{
  public:
    virtual void update(float value);
};

class BatteryLedMapper : public LedMapper
{
  public:
    virtual void update(float value);
};

#endif
