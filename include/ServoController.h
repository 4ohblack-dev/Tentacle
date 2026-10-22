#pragma once
#include<Adafruit_PWMServoDriver.h>

class ServoController {
public:
    Adafruit_PWMServoDriver pca;
    int servoMin = 500;
    int servoMax = 2400;

    enum servoJoint : uint8_t{
      baseX = 0,
      baseY = 1,
      frontX = 2,
      frontY = 3,
      wrist = 4,
      grip = 5,
    };

    void pca_begin();
    void moveServo(servoJoint joint, int angle);

private:
    bool initialized_ = false;
};

