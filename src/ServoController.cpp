#include <Arduino.h>
#include "ServoController.h"

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

void ServoController ::moveServo(servoJoint joint,int angle){
    if (!initialized_) {
        return;
    }
    int pulseUs = map(constrain(angle,0,180),0,180,servoMin,servoMax);
    pca.writeMicroseconds(joint,pulseUs);

}
