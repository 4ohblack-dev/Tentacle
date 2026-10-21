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

  int leftAxis = 0;
  int rightAxis = 0;

  bool leftbutton = false;
  bool rightbutton = false;

  uint8_t dpad = 0;
  bool connected = false;
};

controllerInput input;

// 今回はゲームパッド1台を追跡する
ControllerPtr gamepad = nullptr;

void onControllerConnected(ControllerPtr ctl) {
  if (!ctl->isGamepad()) {
    Serial.println("[BT] Connected device is not a gamepad");
    return;
  }

  if (gamepad != nullptr) {
    Serial.println("[BT] Another gamepad connected; ignored");
    return;
  }

  gamepad = ctl;
  input.connected = true;

  Serial.print("[BT] Gamepad connected: ");
  Serial.println(ctl->getModelName());
}

void onControllerDisconnected(ControllerPtr ctl) {
  if (ctl != gamepad) {
    return;
  }

  gamepad = nullptr;
  input = controllerInput{};  // connectedを含めて初期値に戻す

  Serial.println("[BT] Gamepad disconnected");
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
  BP32.update();
  delay(10);
}