#include <Arduino.h>
#include<Adafruit_PWMServoDriver.h>
#include "GamepadController.h"
#include "ServoController.h"


constexpr uint8_t SDApin = 21;
constexpr uint8_t SCLpin = 22;

GamepadController& controller = GamepadController::instance();
ServoController pca;

void setup(){
  Serial.begin(115200);
  Wire.begin(SDApin,SCLpin);
  pca.pca_begin();
  controller.begin();
  pca.stopContinuousServos();
}

void loop(){
  pca.controll_tentacle(controller);
  delay(10);
}