//NeoPixel Library
#include <Adafruit_NeoPixel.h>


//Constants
#define RING_PIN 6
#define RING_BRIGHTNESS 20
#define RING_NUM_LEDS 12

#define POT_PIN 4
#define POT_MAX_VALUE 1023

Adafruit_NeoPixel rgbRing = Adafruit_NeoPixel(60, RING_PIN, NEO_GRB + NEO_KHZ800);
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
  Serial.begin(9600);
}



void loop() {
  potValue = analogRead(POT_PIN);

  //Calculated the number of pixels that need to be enabled or disabled
  ledsEnabled = (potValue / ledStep);


  //Clear all
  for(unsigned int i = 0; i < RING_NUM_LEDS; i++)
  {
    rgbRing.setPixelColor(i, 0, 0, 0);
  }

  //Set Colors
  for(unsigned int i = 0; i < ledsEnabled; i++)
  {
    rgbRing.setPixelColor(i,  255, 80, 0);
  }
  Serial.println(ledsEnabled);
  rgbRing.show();
}
