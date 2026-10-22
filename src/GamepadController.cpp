#include <Arduino.h>
#include "GamepadController.h"

GamepadController& GamepadController::instance() {
  static GamepadController controller;
  return controller;
}

void GamepadController::begin() {
  Serial.print("[BT] Bluepad32 firmware: ");
  Serial.println(BP32.firmwareVersion());

  BP32.setup(onConnected, onDisconnected);
  BP32.enableNewBluetoothConnections(true);

  Serial.println("[BT] Waiting for a gamepad...");
}

bool GamepadController::update() {
  return BP32.update() && read();
}

void GamepadController::onConnected(ControllerPtr ctl) {
  auto& self = instance();
  // 接続時点では入力データの種類が未確定の場合があるため、受信後に判定する。
  for (auto& controller : self.controllers_) {
    if (controller == nullptr) {
      controller = ctl;
      break;
    }
  }
  Serial.print("[BT] Controller connected: ");
  Serial.println(ctl->getModelName());
}

void GamepadController::onDisconnected(ControllerPtr ctl) {
  auto& self = instance();
  for (auto& controller : self.controllers_) {
    if (controller == ctl) controller = nullptr;
  }
  if (ctl != self.gamepad_) {
    return;
  }

  self.gamepad_ = nullptr;
  self.input_ = ControllerInput{};  // connectedを含めて初期値に戻す

  Serial.println("[BT] Gamepad disconnected");
}

int GamepadController::adjustAxis(float value){
  value = constrain(value,-508,512);
  if(value < 4){
    return map(value,-508,4,-512,0);
  } else {
    return map(value,4,512,0,512);
  }
}

bool GamepadController::read(){
  if (!gamepad_) {
    for (auto controller : controllers_) {
      if (controller && controller->isConnected() &&
          controller->hasData() && controller->isGamepad()) {
        gamepad_ = controller;
        input_.connected = true;
        Serial.println("[BT] Gamepad input ready");
        break;
      }
    }
  }
  if (!gamepad_ || !gamepad_->isConnected() || !gamepad_->hasData()) {
    return false;
  }

  input_.leftX = adjustAxis(gamepad_->axisX());
  input_.leftY = adjustAxis(gamepad_->axisY());
  input_.rightX = adjustAxis(gamepad_->axisRX());
  input_.rightY = adjustAxis(gamepad_->axisRY());
  input_.brake = gamepad_->brake();
  input_.throttle = gamepad_->throttle();
  input_.leftbutton = gamepad_->l1();
  input_.rightbutton = gamepad_->r1();
  input_.buttons = gamepad_->buttons();
  input_.dpad = gamepad_->dpad();

  return true;
}

