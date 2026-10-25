#include <Arduino.h>
#include<Adafruit_PWMServoDriver.h>
#include "ServoController.h"

Adafruit_PWMServoDriver pca;
ServoController servocontroller;

constexpr uint8_t SDApin = 21;
constexpr uint8_t SCLpin = 22;




void setup(){
  Serial.begin(115200);
  Wire.begin(SDApin,SCLpin);
  servocontroller.pca_begin();
  pca.writeMicroseconds(3,90);
  delay(1000);
}
void loop(){

}