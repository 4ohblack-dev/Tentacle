#pragma once
#include <ArduinoBluepad32.h>

class GamepadController {
public:
  struct ControllerInput{
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

  // Bluepad32 has one global callback registry.
  static GamepadController& instance();
  void begin();
  bool update();
  const ControllerInput& input() const { return input_; }
  GamepadController(const GamepadController&) = delete;
  GamepadController& operator=(const GamepadController&) = delete;

private:
  GamepadController() = default;
  static void onConnected(ControllerPtr ctl);
  static void onDisconnected(ControllerPtr ctl);
  static int adjustAxis(float value);
  bool read();
  ControllerInput input_;
  ControllerPtr gamepad_ = nullptr;
  ControllerPtr controllers_[BP32_MAX_GAMEPADS] = {};
};
