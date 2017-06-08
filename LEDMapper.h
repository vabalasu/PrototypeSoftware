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
    int brightness = 255;

  public:
    virtual void setInputRange(float lower, float upper) { lowerInput = lower; upperInput = upper; }
    virtual void setTargetRange(float lower, float upper) { lowerTarget = lower; upperTarget = upper; }
    virtual void setup(Adafruit_NeoPixel *newRing);

    virtual void update(float value) = 0;

  protected:
    virtual int valueToIndex(float value);
};

class FlowLedMapper : public LedMapper
{
  public:
    virtual void update(float value);
};

class BatteryLedMapper : public LedMapper
{
  public:
    virtual void update(float value);
};

class ReadyLedMapper : public LedMapper
{
  protected:
    unsigned long startTime;
    
  public:
    virtual void update(float value);
    void setStartTime(unsigned long st);
};

class BreathResultMapper : public ReadyLedMapper
{
  protected:
    unsigned long startTime;
    unsigned long duration;
    bool wasSuccessful;
    
  public:
    virtual void update(float value);
    void setTimes(unsigned long st, unsigned long duration);
    void setSuccess(bool success);
};


#endif
