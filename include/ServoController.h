#pragma once
#include<Adafruit_PWMServoDriver.h>
#include "GamepadController.h"

class ServoController {
public:
    Adafruit_PWMServoDriver pca;
    int servoMin = 500;
    int servoMax = 2400;
    int servoStopValue = 100;

    enum class servoType : uint8_t{
        Continuous,
        Positional,
    };

    struct servoJoint {
        uint8_t channel;
        servoType type;
        int stopMin;
        int stopMax;
        int servoStopValue;

        servoJoint(
            uint8_t ch,
            servoType servoKind,
            int minValue,
            int maxValue,
            int stopValue = 100
        )
            : channel(ch),
              type(servoKind),
              stopMin(minValue),
              stopMax(maxValue),
              servoStopValue(stopValue) {}
    };

    // 停止範囲は仮の値。各サーボに合わせて調整
    servoJoint baseX  {0,servoType::Continuous,96,106};
    servoJoint baseY  {1,servoType::Continuous,97,106};
    servoJoint frontX {2,servoType::Continuous,96,105};
    servoJoint frontY {3,servoType::Continuous,97,106};

    servoJoint wrist  {4,servoType::Positional,0,0};
    servoJoint grip   {5,servoType::Positional,0,0};

    void pca_begin();
    int deadzone(int value,int stopMin,int stopMax);
    void moveServo(const servoJoint& joint, int angle);
    void controll_tentacle(GamepadController& controller);
    void stopContinuousServos();

private:
    bool initialized_ = false;
    unsigned long lastInputMs_ = 0;
};

