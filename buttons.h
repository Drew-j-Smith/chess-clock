
#include "display.h"
#include "state.h"

constexpr int leftButtonPin = 2;
constexpr int rightButtonPin = 3;
constexpr int resetButtonPin = 13;

void leftButtonPress() {
  int now = millis();
  switch (state) {
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::RIGHT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    lastButtonPressTime = now;
    leftIsWhite = false;
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
  if (leftTime <= 0) {
    state = State::LEFT_FLAG;
    leftTime = 0;
  }
  lastButtonPressTime = now;
  changed = true;
}

void rightButtonPress() {
  int now = millis();
  switch (state) {
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::LEFT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    lastButtonPressTime = now;
    leftIsWhite = true;
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
  rightTime += rightIncrement;
  if (rightTime <= 0) {
    state = State::RIGHT_FLAG;
    rightTime = 0;
  }
  lastButtonPressTime = now;
  changed = true;
}

void reset() {
  if (digitalRead(resetButtonPin) != LOW)
    return;
  initializeDisplay();
  leftTime = 10000;
  rightTime = 10000;
  state = State::STOPPED;
  changed = true;
}

void initializeButtons() {
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), leftButtonPress,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), rightButtonPress,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(resetButtonPin), reset, FALLING);
}