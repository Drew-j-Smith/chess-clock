
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
  auto diff = now - lastButtonPressTime;
  switch (state) {
  case State::LEFT_TIME_RUNNING: {
    if (diff < leftDelay) {
      actualLeftTime = leftDelay - diff;
    } else {
      actualLeftTime = leftTime - diff + leftDelay;
    }
    break;
  }
  case State::RIGHT_TIME_RUNNING: {
    if (diff < rightDelay) {
      actualRightTime = rightDelay - diff;
    } else {
      actualRightTime = rightTime - diff + rightDelay;
    }
  }
  default: {
    break;
  }
  }
  if (actualLeftTime < 0) {
    state = State::LEFT_FLAG;
    leftTime = 0;
    actualLeftTime = 0;
    changed = true;
  }
  if (actualRightTime < 0) {
    state = State::RIGHT_FLAG;
    rightTime = 0;
    actualRightTime = 0;
    changed = true;
  }
  setTime(now, actualLeftTime, actualRightTime);
  if (changed) {
    updateBluetooth(actualLeftTime, actualRightTime);
    changed = false;
  }
  BLE.poll();
}