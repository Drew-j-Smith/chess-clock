
#include "state.h"

#include "bluetooth.h"
#include "buttons.h"
#include "display.h"

void setup() {
  delay(2000); // wait for lcd
  initializeDisplay();
  initializeButtons();
  intializeBluetooth();
}

void loop() {
  int now = millis();
  leftButton.process(now);
  rightButton.process(now);
  resetButton.process(now);
  int actualLeftTime = leftTime;
  int actualRightTime = rightTime;
  switch (state) {
  case State::LEFT_TIME_RUNNING: {
    actualLeftTime = leftTime - now + lastButtonPressTime;
    break;
  }
  case State::RIGHT_TIME_RUNNING: {
    actualRightTime = rightTime - now + lastButtonPressTime;
    break;
  }
  case State::LEFT_FLAG: {
    flashLed(true);
    break;
  }
  case State::RIGHT_FLAG: {
    flashLed(false);
    break;
  }
  default: {
    break;
  }
  }
  if (actualLeftTime < 0) {
    state = State::LEFT_FLAG;
    leftTime = 0;
    changed = true;
  }
  if (actualRightTime < 0) {
    state = State::RIGHT_FLAG;
    rightTime = 0;
    changed = true;
  }
  if (state != State::LEFT_FLAG && state != State::RIGHT_FLAG) {
    setTime(actualLeftTime, actualRightTime);
  }
  if (changed) {
    updateBluetooth(actualLeftTime, actualRightTime);
    changed = false;
  }
  BLE.poll();
}