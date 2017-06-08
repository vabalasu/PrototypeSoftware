#include <Arduino.h>

int batteryVoltageToPercent(float voltage)
{
  int batteryPercent;
  
  const float VMAX = 4.2;
  const float VMIN = 3.5;
  batteryPercent = (int)((voltage - VMIN) / (VMAX - VMIN) * 100 + 0.5);
  batteryPercent = max(batteryPercent, 0);
  batteryPercent = min(batteryPercent, 100);

  return batteryPercent;
}

// Based on a characterization of a specific pneumatic configuration on 11May2017
// 0 lpm = 0 Pa
// 2.7 lpm = 257.712 Pa
// 3.0 lpm = 301.2748 Pa
// 3.3 lpm = 347.98 Pa
float pressureToAbsFlow(float pressure)
{
  if (pressure < 0)
    pressure = -pressure;
    
  return pressure * pressure * -0.0000108 + pressure * 0.0133;
}

