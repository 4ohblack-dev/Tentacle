#include <Arduino.h>
#include<Adafruit_PWMServoDriver.h>

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


void setup(){
  Serial.begin(115200);
  Wire.begin(SDApin,SCLpin);


  if(!pca.begin()){
    Serial.println("pca not found");
    while(true){
      delay(100);
    }
  }
  pca.setPWMFreq(50);
  delay(1000);
}

void loop(){


}