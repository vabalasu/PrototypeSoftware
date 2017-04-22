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
//Instantiates external singleton "ledRing"
#include "LEDRing.h"
#define LEDRING_PIN 6
LedRing ledRing;

//Driver for Sensirion SDP3x Differential Pressure Sensor I2C addon
//Instantiates external singleton "SDP3x"
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
Adafruit_BLEBattery battery(ble);

void setup() {
  //Begin Serial communication (debugging only)
  Serial.begin(9600);

  //Setup LED ring for the potentiometer
  ledRing.setup(LEDRING_PIN);
  ledRing.setInputRange(10.0, 980.0);
  ledRing.setTargetRange(323.0, 575.0);

  //Setup SDP3x Sensor
  sdp3x.setup(SD3Px_I2C_ADDRESS);

  //Begin I2C communication
  Wire.begin();

  //Begin BLE communication
  ble.begin();
  battery.begin(true);
  ble.setMode(BLUEFRUIT_MODE_DATA);
}

void loop() {
  // Perform A/D sampling of potentiometer (~0.1ms)
  int potValue = analogRead(POT_PIN);

  // Perform A/D sampling of battery voltage (~0.1ms)
  float batteryVoltage = analogRead(VBAT_PIN) * 2 * (3.3 / 1024);
  int batteryPercent = (int)(batteryVoltage / 4.2 + 0.5);
  batteryPercent = max(batteryPercent, 0);
  batteryPercent = min(batteryPercent, 100);
  battery.update(batteryPercent);

  // Sample pressure sensor (50ms)
  // Simply print raw value, this can be viewed in the serial plotter
  float temperature, pressure;
  sdp3x.readSensor(temperature, pressure);

  ledRing.updateRing(potValue);

  String valuesString =
    String(batteryVoltage,2)
    + " " + String(potValue)
    + " " + String(pressure,2)
    + " " + String(temperature,2);
  if (Serial)
    Serial.println("Battery (V), Potentiometer, Pressure (Pa), Temperature (C): " + valuesString);

  if (ble.isConnected())
    ble.println(valuesString);

  //Delay between cycles in ms
  delay(200);
}
