# Setup
This project is an Arduino Sketch designed using the [Arduino IDE for Mac](https://www.arduino.cc/en/guide/macOSX), version 1.8.2.

The hardware target is [AdaFruit's Feather 32u4 Bluefruit LE](https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/overview). The linked instructions for adding board support and installing the BLE library must be followed prior to compiling this project. Note that since these instructions were written, the IDE has added support for installing libraries from a ZIP file. This will install the library in your IDE's default folder, and make it available for all sketches.

Several addons to the board are expected to exist.
1. A [NeoPixel Ring](https://www.adafruit.com/product/1643) with 12 RGB LED's and single-wire interface. The wire is expected to be connected to Pin 6. The NeoPixel library "Adafruit_NeoPixel.h" should also be installed.
1. A potentiometer connected across 3V and Ground, with middle pin connected to Pin A4.
1. A Sensirion I2C Differential Pressure Sensor [SDP3x](https://www.sensirion.com/environmental-sensors/humidity-sensors/worlds-smallest-differential-pressure-sensor/), connected to 3V, GND, SCA and SCL.
1. A Lithium Ion Batter. The original design uses the [AdaFruit 1578](https://www.adafruit.com/product/1578), 3.7V 500mAh battery.

# Functionality
The Arduino Sketch polls battery voltage, potentiometer voltage, differential pressure and ambient temperature (from the pressure sensor). SDP31 acquisition time is 45ms, and is configured in Trigger Mode with clock stretching. Onboard A/D acquisition time is ~1ms per channel.

Data is sent to a PC via USB, and to a connected BLE device via the UART TX characteristic.
