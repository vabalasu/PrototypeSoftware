//Board Support (Feather 32u4 with NR51 BLE)
#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_ATParser.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BLEBattery.h>
#include <Adafruit_BLEEddystone.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLEMIDI.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>

//LedRing Controller, encapsulates a NeoPixel 12-segment LED ring on Pin 6
#include "LEDMapper.h"
#include <Adafruit_NeoPixel.h>
#define LEDRING_PIN 6
Adafruit_NeoPixel *ring;
PotLedMapper flowMapper;
BatteryLedMapper batteryMapper;

//Driver for Sensirion SDP3x Differential Pressure Sensor I2C addon
#include "SDP3x.h"
#define SD3Px_I2C_ADDRESS 0x21
SDP3x sdp3x;

//BLE Configuration
#include "BluefruitConfig.h"
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

//Potentiometer Circuit on Pin A4 Configuration
#define POT_PIN A4

//Battery Voltage on Pin A9 Configuration
#define VBAT_PIN A9

#define BATSTATUS_PIN 13

Adafruit_BLEBattery battery(ble);
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

unsigned long startMillis = 0;

void setup() {
  //Begin Serial communication (debugging only)
  Serial.begin(9600);

  //Setup LED ring for the pressure sensor (converted to flow)
  ring = new Adafruit_NeoPixel(60, LEDRING_PIN, NEO_GRB + NEO_KHZ800);
  ring->begin();
  ring->show();

  pinMode(BATSTATUS_PIN, INPUT_PULLUP);

  flowMapper.setup(ring);
  flowMapper.setInputRange(2.0, 4.0);
  flowMapper.setTargetRange(2.7, 3.3);

  batteryMapper.setup(ring);
  batteryMapper.setInputRange(0.0, 100.0);
  batteryMapper.setTargetRange(25.0, 100.0);  

  //Setup SDP3x Sensor
  sdp3x.setup(SD3Px_I2C_ADDRESS);

  //Begin I2C communication
  Wire.begin();

  //Begin BLE communication
  ble.begin();
  battery.begin(true);
  ble.setMode(BLUEFRUIT_MODE_DATA);

  startMillis = millis();
}

void loop() {
  // Perform A/D sampling of battery voltage (~0.1ms)
  float batteryVoltage = analogRead(VBAT_PIN) * 2 * (3.3 / 1024);
  int batteryPercent = batteryVoltageToPercent(batteryVoltage);
  battery.update(batteryPercent);

  // Sample pressure sensor (~45ms)
  float temperature = 0.0, pressure = 0.0;
  sdp3x.readSensor(temperature, pressure);

  // Flow conversion
  float flow = pressureToAbsFlow(pressure);

  bool showBattery = digitalRead(BATSTATUS_PIN) == LOW;
  if (showBattery)
    startMillis = millis();
    
  if (millis() - startMillis < 5000)
    batteryMapper.update(batteryPercent);
  else  
    flowMapper.update(flow);

  if (Serial)
  {
    Serial.println("Battery (V), Temperature (C), Pressure (Pa), Abs Flow (lpm): "
    + String(batteryVoltage,2)
    + "," + String(temperature,2)
    + "," + String(pressure,2)
    + "," + String(flow,2));
  }

  if (ble.isConnected())
  {
    ble.println(String(temperature,2)
    + "," + String(pressure,2)
    + "," + String(flow,2));
  }

  //Delay between cycles in ms
  delay(50);
}
