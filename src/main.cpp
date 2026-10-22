#include <Arduino.h>
#include<Adafruit_PWMServoDriver.h>
#include <ArduinoBluepad32.h>

#define servoMin 500
#define servoMax 2400

Adafruit_PWMServoDriver pca;

constexpr uint8_t SDApin = 21;
constexpr uint8_t SCLpin = 22;

enum servoJoint : uint8_t{
  front = 0,
  rear = 1,
  leftfront = 2,
  rightfront = 3,
  leftrear = 4,
  rightrear = 5,
};

void moveServo(servoJoint joint,int angle){
  int pulseUs = map(constrain(angle,0,180),0,180,servoMin,servoMax);
  pca.writeMicroseconds(joint,pulseUs);
}

struct controllerInput{
  int leftX = 0;
  int leftY = 0;
  int rightX = 0;
  int rightY = 0;

  int brake = 0;
  int throttle = 0;

  bool leftbutton = false;
  bool rightbutton = false;

  uint8_t dpad = 0;
  uint16_t buttons = 0;
  bool connected = false;
};

controllerInput input;

// 今回はゲームパッド1台を追跡する
ControllerPtr gamepad = nullptr;
ControllerPtr controllers[BP32_MAX_GAMEPADS] = {};

void onControllerConnected(ControllerPtr ctl) {
  // 接続時点では入力データの種類が未確定の場合があるため、受信後に判定する。
  for (auto& controller : controllers) {
    if (controller == nullptr) {
      controller = ctl;
      break;
    }
  }
  Serial.print("[BT] Controller connected: ");
  Serial.println(ctl->getModelName());
}

void onControllerDisconnected(ControllerPtr ctl) {
  for (auto& controller : controllers) {
    if (controller == ctl) controller = nullptr;
  }
  if (ctl != gamepad) {
    return;
  }

  gamepad = nullptr;
  input = controllerInput{};  // connectedを含めて初期値に戻す

  Serial.println("[BT] Gamepad disconnected");
}

int adjustAxis(float value){
  value = constrain(value,-508,512);
  if(value < 4){
    return map(value,-508,4,-512,0);
  } else {
    return map(value,4,512,0,512);
  }
}

bool readController(){
  if (!gamepad) {
    for (auto controller : controllers) {
      if (controller && controller->isConnected() &&
          controller->hasData() && controller->isGamepad()) {
        gamepad = controller;
        input.connected = true;
        Serial.println("[BT] Gamepad input ready");
        break;
      }
    }
  }
  if (!gamepad || !gamepad->isConnected() || !gamepad->hasData()) {
    return false;
  }

  input.leftX = adjustAxis(gamepad->axisX());
  input.leftY = adjustAxis(gamepad->axisY());
  input.rightX = adjustAxis(gamepad->axisRX());
  input.rightY = adjustAxis(gamepad->axisRY());
  input.brake = gamepad->brake();
  input.throttle = gamepad->throttle();
  input.leftbutton = gamepad->l1();
  input.rightbutton = gamepad->r1();
  input.buttons = gamepad->buttons();
  input.dpad = gamepad->dpad();

  return true;
}

void setup(){
  Serial.begin(115200);
  Wire.begin(SDApin,SCLpin);


  if(!pca.begin()){
    Serial.println("pca not found, continuing bluetooth test");
  } else{
    pca.setPWMFreq(50);
    Serial.println("pca ready");
  }
  Serial.print("[BT] Bluepad32 firmware: ");
  Serial.println(BP32.firmwareVersion());

  BP32.setup(onControllerConnected, onControllerDisconnected);
  BP32.enableNewBluetoothConnections(true);

  Serial.println("[BT] Waiting for a gamepad...");
}

void loop(){
  static unsigned long lastPrint = 0;
  if (BP32.update() && readController()) {
    if (millis() - lastPrint >= 100) {
      lastPrint = millis();
      Serial.printf("LX=%f LY=%f RX=%f RY=%f L2=%d R2=%d L1=%d R1=%d dpad=0x%02x buttons=0x%04x\n",
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