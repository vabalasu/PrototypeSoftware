//NeoPixel Library
#include <Adafruit_NeoPixel.h>
#include "BluefruitConfig.h"
#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
//Constants
#define RING_PIN 6
#define RING_BRIGHTNESS 20
#define RING_NUM_LEDS 12

#define POT_PIN 4
#define POT_MAX_VALUE 1023

Adafruit_NeoPixel rgbRing = Adafruit_NeoPixel(60, RING_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
int potValue = 0;
unsigned int ledStep = 0;
unsigned int ledsEnabled = 0;
void setup() {
  //Setup LED ring
  rgbRing.begin();
  rgbRing.show();
  rgbRing.setBrightness(RING_BRIGHTNESS);

  //Calculate LED step
  ledStep = POT_MAX_VALUE / RING_NUM_LEDS;

  ble.begin();
  Serial.begin(9600);
}

unsigned ledsNeedEnable;
unsigned bled = 0;
void loop() {
  potValue = analogRead(POT_PIN);

  //Calculated the number of pixels that need to be enabled or disabled
  ledsNeedEnable = (potValue / ledStep);
  int delta = ledsNeedEnable - ledsEnabled;
  if(delta > 0)
  {
    for(unsigned int i = ledsEnabled; i < ledsNeedEnable; i++)
    {
      rgbRing.setPixelColor(i, 255, 80, 0);
    }
    rgbRing.show();
  }
  else if(delta < 0)
  {
    for(unsigned int i = ledsEnabled; i > ledsNeedEnabled; i--)
    {
      rgbRing.setPixelColor(i, 0, 0, 0 );
    }
    rgbRing.show();
  }
  ledsEnabled = ledsNeedEnable;
  bled++;
  if (bled > 100) {
    ble.print("AT+BLEUARTTX=");
    String snd = String(potValue);
    snd += "    ";
    ble.println(snd);
    bled = 0;
  }


}
