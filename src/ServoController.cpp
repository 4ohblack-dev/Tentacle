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
    if (joint.type == servoType::Continuous &&
        angle >= joint.stopMin && angle <= joint.stopMax) {
        angle = joint.servoStopValue;
    }
    int pulseUs = map(angle,0,180,servoMin,servoMax);
    pca.writeMicroseconds(joint.channel,pulseUs);
}

void ServoController::stopContinuousServos() {
    if (!initialized_) return;

    const servoJoint* joints[] = {&baseX, &baseY, &frontX, &frontY};

    for (const auto* joint : joints) {
        int pulseUs = map(constrain(joint->servoStopValue, 0, 180),0, 180, servoMin, servoMax);
        pca.writeMicroseconds(joint->channel, pulseUs);
    }
}


void ServoController::controll_tentacle(GamepadController& controller) {
    bool updated = controller.update();
    const auto& input = controller.input();

    if (!initialized_) return;

    if (!input.connected) {
        stopContinuousServos();
        return;
    }

    if (updated) {
        lastInputMs_ = millis();
        // A centered stick must command the calibrated stop value, not 90.
        // Ignore small center drift so the continuous servos remain stopped.
        auto toContinuousCommand = [](int value, const servoJoint& joint) -> int {
            value = constrain(value, -512, 512);
            if (abs(value) <= 30) {
                return joint.servoStopValue;
            }
            if (value < 0) {
                return map(value, -512, -31, 0, joint.servoStopValue);
            }
            return map(value, 31, 512, joint.servoStopValue, 180);
        };

        moveServo(baseX,  toContinuousCommand(input.leftX, baseX));
        moveServo(baseY,  toContinuousCommand(input.leftY, baseY));
        moveServo(frontX, toContinuousCommand(input.rightX, frontX));
        moveServo(frontY, toContinuousCommand(input.rightY, frontY));
    } else if (millis() - lastInputMs_ >= 500) {
        stopContinuousServos();
    }
}
