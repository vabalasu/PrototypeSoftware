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

#include "util.h"
#include "Music.h"

//LedRing Controller, encapsulates a NeoPixel 12-segment LED ring on Pin 6
#include "LEDMapper.h"
#include <Adafruit_NeoPixel.h>
#define LEDRING_PIN 6
Adafruit_NeoPixel *ring;
FlowLedMapper flowMapper;
BatteryLedMapper batteryMapper;
ReadyLedMapper readyMapper;
BreathResultMapper breathResultMapper;

//Driver for Sensirion SDP3x Differential Pressure Sensor I2C addon
#include "SDP3x.h"
#define SD3Px_I2C_ADDRESS 0x21
SDP3x sdp3x;

//BLE Configuration
#include "BluefruitConfig.h"
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEBattery battery(ble);

//Battery Voltage on Pin A9 Configuration
#define VBAT_PIN A9
#define BATSTATUS_PIN 13

//Passive buzzer on Pin 11
#define BUZZER_PIN 11

// Finite State Machine
#include <FiniteStateMachine.h>
// I hate this so much, but the FSM code has no way to pass data into the update functions!
int batteryPercent = 0;
float flow = 0.0;
bool breathManeuverSuccess = false;

#define RESULT_DURATION_MILLIS 1000

void displayBatteryUpdate() {
  batteryMapper.update(batteryPercent);
}

void breathDetectEnter() {
  readyMapper.setStartTime(millis());
}

void breathDetectUpdate() {
  readyMapper.update(millis());
}

void breathManeuverUpdate() {
  flowMapper.update(flow);
}

void breathManeuverExit() {
  breathManeuverSuccess = true;
}

void breathResultEnter() {
  breathResultMapper.setTimes(millis(), RESULT_DURATION_MILLIS);
  breathResultMapper.setSuccess(breathManeuverSuccess);
}

void breathResultUpdate() {
  breathResultMapper.update(millis());
}

State displayBattery = State(displayBatteryUpdate);
State breathDetect = State(breathDetectEnter, breathDetectUpdate, 0);
State breathManeuver = State(0, breathManeuverUpdate, breathManeuverExit);
State breathManeuverResult = State(breathResultEnter, breathResultUpdate, 0);

FSM stateMachine(displayBattery);

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
  batteryMapper.setTargetRange(25.0, 50.0);

  readyMapper.setup(ring);

  breathResultMapper.setup(ring);

  //Begin I2C communication
  Wire.begin();

  //Setup SDP3x Sensor
  sdp3x.setup(SD3Px_I2C_ADDRESS);

  //Begin BLE communication
  ble.begin();
  battery.begin(true);
  ble.setMode(BLUEFRUIT_MODE_DATA);
  Serial.println("Setup complete");

  //Buzzer test
  pinMode(BUZZER_PIN, OUTPUT);
  playIntro(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  static unsigned long lastCommTime = 0;
  // Perform A/D sampling of battery voltage (~0.1ms)
  float batteryVoltage = analogRead(VBAT_PIN) * 2 * (3.3 / 1024);
  batteryPercent = batteryVoltageToPercent(batteryVoltage);

  // Sample pressure sensor
  float temperature = 0.0, pressure = 0.0;
  int err = sdp3x.readSensor(temperature, pressure);
  if (err != SDP_SUCCESS)
    Serial.println("I2C Error: " + String(err));
  flow = pressureToAbsFlow(pressure);

  if (stateMachine.isInState(displayBattery)) {
    if (stateMachine.timeInCurrentState() >= 5000)
      stateMachine.transitionTo(breathDetect);
  } else if (stateMachine.isInState(breathDetect)) {
    if (flow > 0.2)
      stateMachine.transitionTo(breathManeuver);
    else if (digitalRead(BATSTATUS_PIN) == LOW)
      stateMachine.transitionTo(displayBattery);
  } else if (stateMachine.isInState(breathManeuver)) {
    if (stateMachine.timeInCurrentState() >= 10000)
      stateMachine.transitionTo(breathManeuverResult);
  } else if (stateMachine.isInState(breathManeuverResult)) {
    if (stateMachine.timeInCurrentState() >= RESULT_DURATION_MILLIS)
      stateMachine.transitionTo(breathDetect);
  }

  stateMachine.update();

  if (millis() - lastCommTime >= 100)
  {
    lastCommTime = millis();
    battery.update(batteryPercent);
    logToSerial(batteryVoltage, temperature, pressure, flow);
    logToBle(temperature, pressure, flow);
  }

  delay(100);
}

void logToSerial(float vbatt, float temp, float pres, float flow) {
  if (Serial) {
    Serial.println("Battery (V), Temperature (C), Pressure (Pa), Abs Flow (lpm): "
    + String(vbatt,2)
    + "," + String(temp,2)
    + "," + String(pres,2)
    + "," + String(flow,2));
  }  
}

void logToBle(float temp, float pres, float flow) {
  if (ble.isConnected())
  {
    ble.println(String(temp,2)
    + "," + String(pres,2)
    + "," + String(flow,2));
  }
}

