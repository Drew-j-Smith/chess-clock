
#include "display.h"
#include "state.h"

#include <InputDebounce.h>

constexpr int leftButtonPin = 2;
constexpr int rightButtonPin = 3;
constexpr int resetButtonPin = 13;

InputDebounce leftButton;
InputDebounce rightButton;
InputDebounce resetButton;

void leftButtonPress(uint8_t pinIn) {
  int now = millis();
  switch (state) {
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::RIGHT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    leftIsWhite = false;
  }
  case State::PAUSED: {
    lastButtonPressTime = now;
    leftTime -= leftIncrement;
    break;
  }
  default:
    break;
  }
  state = State::RIGHT_TIME_RUNNING;
  auto diff = now - lastButtonPressTime;
  if (diff > leftDelay) {
    leftTime += -diff + leftDelay;
  }
  leftTime += leftIncrement;
  lastButtonPressTime = now;
  changed = true;
}

void rightButtonPress(uint8_t pinIn) {
  int now = millis();
  switch (state) {
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::LEFT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    leftIsWhite = false;
  }
  case State::PAUSED: {
    lastButtonPressTime = now;
    rightTime -= leftIncrement;
    break;
  }
  default:
    break;
  }
  state = State::LEFT_TIME_RUNNING;
  auto diff = now - lastButtonPressTime;
  if (diff > rightDelay) {
    rightTime += -diff + rightDelay;
  }
  lastButtonPressTime = now;
  changed = true;
}

void reset(uint8_t pinIn) {
  auto now = millis();
  switch (state) {
  case State::LEFT_TIME_RUNNING: {
    leftTime -= now - lastButtonPressTime;
    state = State::PAUSED;
    break;
  }
  case State::RIGHT_TIME_RUNNING: {
    rightTime -= now - lastButtonPressTime;
    state = State::PAUSED;
    break;
  }
  case State::STOPPED:
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::PAUSED: {
    initializeDisplay();
    leftTime = 10000;
    rightTime = 10000;
    state = State::STOPPED;
    changed = true;
    break;
  }
  default:
    break;
  }
}

void initializeButtons() {
  leftButton.registerCallbacks(leftButtonPress, nullptr);
  rightButton.registerCallbacks(rightButtonPress, nullptr);
  resetButton.registerCallbacks(reset, nullptr);
  leftButton.setup(leftButtonPin);
  rightButton.setup(rightButtonPin);
  resetButton.setup(resetButtonPin);
}