#include <Arduino.h>
#include "ServoController.h"
#include "GamepadController.h"

void ServoController::pca_begin(){
    initialized_ = false;
    if(!pca.begin()){
        Serial.println("pca not found, continuing bluetooth test");
    } else{
        pca.setPWMFreq(50);
        initialized_ = true;
        Serial.println("pca ready");
    }
}

int ServoController::deadzone(int value,int stopMin,int stopMax){
    if (stopMin <= value && value <= stopMax){
        return servoStopValue;
    } else {
        return value;
    }
}

void ServoController ::moveServo(const servoJoint& joint,int angle){
    if (!initialized_) {
        return;
    }
    angle = constrain(angle,0,180);
    if(joint.type == servoType::Continuous){
        deadzone(angle,joint.stopMin,joint.stopMax);
    }
    int pulseUs = map(angle,0,180,servoMin,servoMax);
    pca.writeMicroseconds(joint.channel,pulseUs);

}

void ServoController::controll_tentacle(GamepadController& controller){
    if(!controller.update()) return;

    const auto& input = controller.input();
    if(!input.connected||!initialized_) return;


    auto toAngle = [](int value)->int{
        return map(constrain(value,-512,512),-512,512,0,180);
    };

    moveServo(baseX,  toAngle(input.leftX));
    moveServo(baseY,  toAngle(input.leftY));
    moveServo(frontX, toAngle(input.rightX));
    moveServo(frontY, toAngle(input.rightY));
}
