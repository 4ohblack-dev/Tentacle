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
}

void loop(){
  static unsigned long lastPrint = 0;
  const auto& input = controller.input();
  if (controller.update()) {
    if (millis() - lastPrint >= 100) {
      lastPrint = millis();
      Serial.printf("LX=%d LY=%d RX=%d RY=%d L2=%d R2=%d L1=%d R1=%d dpad=0x%02x buttons=0x%04x\n",
                    input.leftX, input.leftY, input.rightX, input.rightY,
                    input.brake, input.throttle, input.leftbutton, input.rightbutton,
                    input.dpad, input.buttons);
    }
  } else if (!input.connected && millis() - lastPrint >= 2000) {
    lastPrint = millis();
    Serial.println("[BT] Waiting for gamepad input...");
  }
  delay(10);
}